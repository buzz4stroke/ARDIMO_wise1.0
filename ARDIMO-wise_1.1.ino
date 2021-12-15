#include <MaerklinMotorola.h>
#include <Servo.h>
#include <EEPROM.h>

#define INPUT_PIN 2

uint8_t PortAddress;
uint8_t PortAddressMem;
unsigned int NumberAcc;
unsigned int NumberAccMem;
unsigned int NumberAccVar;
unsigned int Buddy;
uint8_t ModeAcc;
uint8_t ModeAccRed;
uint8_t ModeAccGreen;
uint8_t ModeAccMem;
uint8_t ModeAccRedMem;
uint8_t ModeAccGreenMem;
uint8_t PinRed;
uint8_t PinRedMem;
uint8_t PinGreen;
uint8_t PinGreenMem;
bool StatusRed;
bool StatusRedMem;
bool StatusGreen;
bool StatusGreenMem;
uint8_t posServo;
uint8_t posServoMem;
bool Inverted;
bool InvertedMem;
uint8_t TaskRGcommand=0;
uint8_t TaskRGcommandMem;
uint8_t TaskRG=0;
unsigned long TimeCommand;


Servo myservo;

volatile MaerklinMotorola mm(INPUT_PIN);
volatile bool hasNewData;


////Procedures when starting up the ARDIMO
//Start sequence
void StartupARDIMO () {
  String ConfigValue;
  unsigned long StartTime = millis(); //startup timing mark
  unsigned long WaitingTime = 6000UL;
  Serial.println(F("Would you reconfigure the decoder Y/N?"));
  while (Serial.available()==0 && (millis()-StartTime < WaitingTime)) {}
    ConfigValue=Serial.readString();
      if (ConfigValue=="Y" or ConfigValue=="y") {
        Wipe_EEPROM();
        Programmer();
        Serial.println(F("Leaving programming mode..."));
        Serial.println(F(""));
        Serial.println(F(""));
        }
  Serial.println(F("Loading saved variables, please wait...."));
  delay(1000);
  StartupLoadVariables();
}
//Wipe EEPROM at startup setting all values at 0, first time when all values are 255 wipe goes automatic.
void Wipe_EEPROM() {
  if(EEPROM.read(0)>0) {
    for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
    }  
  Serial.println(F("  Initial formatting done."));
  }
  else {
    String WipeValue;
    Serial.print(F("Would you format the decoder Y/N?"));
      while (Serial.available()==0) {}
      WipeValue=Serial.readString();
        if (WipeValue=="Y" or WipeValue=="y") {
        Serial.println(F(""));
          for (int i = 0 ; i < EEPROM.length() ; i++) {
          EEPROM.put(i, 0);
          }
        }
  Serial.println(F("Formatting done."));
  Serial.println(F(""));
  }
}
//Programmer sets the values for controlling the functions, user input nessesary.
void Programmer() {
  String ProgValue;
  uint8_t DelayProg=200;
  uint8_t DelayWrite=5;
  bool ChoosePin=0;
  Serial.println(F("Would you program the decoder Y/N?"));
    while (Serial.available()==0) {}
    ProgValue=Serial.readString();
      if (ProgValue=="Y" or ProgValue=="y") {
        NumberAcc=0; PortAddress=0; Buddy=0; PinRed=0; ModeAcc=0; PinGreen=0; posServo=0; Inverted=0;
        Serial.println(F("")); delay(DelayWrite);
        Serial.println(F("What is the assigned number for the accessorie?")); delay(DelayWrite);
        Serial.println(F("  - Primary mandatory number in range 1-16")); delay(DelayWrite);
        Serial.println(F("  - First optional buddy in range 201-216")); delay(DelayWrite);
        Serial.println(F("  - Second optional buddy in range 401-416")); delay(DelayWrite);
        Serial.println(F("  - Third optional buddy in range 601-616")); delay(DelayWrite);
        Serial.println(F("  - Fourth optional buddy in range 801-816")); delay(DelayWrite);
        while (Serial.available()==0) {}
        NumberAcc=Serial.parseInt();
        delay(DelayProg);
        if((NumberAcc<17)||
          ((NumberAcc>200)&&(NumberAcc<217))||
          ((NumberAcc>400)&&(NumberAcc<417))||
          ((NumberAcc>600)&&(NumberAcc<617))||
          ((NumberAcc>800)&&(NumberAcc<817))) {
          Serial.println(F("What is the Digital Motorala Address? (1-255)."));
          while (Serial.available()==0) {}
          PortAddress=Serial.parseInt();
          delay(DelayProg);
            Serial.println(F("What is the operation mode? (1-7).")); delay(DelayWrite);
            Serial.println(F("     1. Turnout with coil")); delay(DelayWrite);
            Serial.println(F("     2. Turnout with servo")); delay(DelayWrite);
            Serial.println(F("     3. Signal with coil")); delay(DelayWrite);
            Serial.println(F("     4. Signal with servo")); delay(DelayWrite);
            Serial.println(F("     5. Signal with lights")); delay(DelayWrite);
            Serial.println(F("     6. Lights on/off")); delay(DelayWrite);
            Serial.println(F("     7. Decoupler rail with coil"));
              while (Serial.available()==0) {}
              ModeAcc=Serial.parseInt();
              delay(DelayProg);
              if((ModeAcc==2)||(ModeAcc==4)) {
              Serial.println(F("Which is the Arduino pin for the servo?")); delay(DelayWrite);
              Serial.println(F("--Nano uses only port 3,5,6,9,10 and 11--")); delay(DelayWrite);
                while (Serial.available()==0) {}
                PinRed=Serial.parseInt();
                PinGreen=PinRed;
                ModeAccRed=ModeAcc;
                ModeAccGreen=ModeAcc;
                posServo=90;
                }
              else if((ModeAcc==1)||(ModeAcc==3)||(ModeAcc==5)) {  
                Serial.println(F("Which is the Arduino pin for red button?")); delay(DelayWrite);
                while (Serial.available()==0) {}
                PinRed=Serial.parseInt();
                ModeAccRed=ModeAcc;
                delay(DelayProg);
                Serial.println(F("Which is the Arduino pin for green button?")); delay(DelayWrite);
                while (Serial.available()==0) {}
                PinGreen=Serial.parseInt();
                ModeAccGreen=ModeAcc;
                delay(DelayProg);}
              else if((ModeAcc==6)||(ModeAcc==7)) {
                Serial.println(F("Which button has the function red(0) or green(1)?")); delay(DelayWrite);
                while (Serial.available()==0) {}
                ChoosePin=Serial.parseInt();
                delay(DelayProg);
                if (ChoosePin==0){
                  Serial.println(F("Which is the Arduino pin for red button?")); delay(DelayWrite);
                  while (Serial.available()==0) {}
                  PinRed=Serial.parseInt();
                  ModeAccRed=ModeAcc;
                  delay(DelayProg);
                  }
                else if (ChoosePin==1) {
                  Serial.println(F("Which is the Arduino pin for green button?")); delay(DelayWrite);
                  while (Serial.available()==0) {}
                  PinGreen=Serial.parseInt();
                  ModeAccGreen=ModeAcc;
                  delay(DelayProg);
                  }
                }
                delay(DelayProg);
                Serial.println(F("Is there a buddy? (J=1/N=0).")); delay(DelayWrite);
                while (Serial.available()==0) {}
                Buddy=Serial.parseInt();
                delay(DelayProg);
                if((ModeAcc==1)||(ModeAcc==2)||(ModeAcc==3)||(ModeAcc==4)||(ModeAcc==5)) { 
                Serial.println(F("Are the functions inverted? (J=1/N=0).")); delay(DelayWrite);
                while (Serial.available()==0) {}
                Inverted=Serial.parseInt();
                delay(DelayProg);
                }
                Serial.println(F(""));
                Serial.println(F("Thank you! Values are:")); delay(DelayWrite);
                Serial.print(F("     Number acc: ")); Serial.println(NumberAcc); delay(DelayWrite);
                Serial.print(F("     PortAddress: ")); Serial.println(PortAddress); delay(DelayWrite);
                Serial.print(F("     Buddy active: "));Serial.println(Buddy); delay(DelayWrite);
                if(ChoosePin==0 && (ModeAcc==6)||(ModeAcc==7)) {
                Serial.print(F("     Type mode red: "));Serial.println(ModeAccRed); delay(DelayWrite);
                Serial.print(F("     Number red pin: "));Serial.println(PinRed); delay(DelayWrite);
                Serial.print(F("     Status Red: "));Serial.println(StatusRed); delay(DelayWrite);
                }
                if(ChoosePin==1 && (ModeAcc==6)||(ModeAcc==7)) { 
                Serial.print(F("     Type mode green: "));Serial.println(ModeAccGreen); delay(DelayWrite);
                Serial.print(F("     Number green pin: "));Serial.println(PinGreen); delay(DelayWrite);
                Serial.print(F("     Status Green: "));Serial.println(StatusGreen); delay(DelayWrite);
                }
                if ((ModeAcc!=6)&&(ModeAcc!=7)) {
                Serial.print(F("     Type mode red: "));Serial.println(ModeAccRed); delay(DelayWrite);
                Serial.print(F("     Number red pin: "));Serial.println(PinRed); delay(DelayWrite);
                Serial.print(F("     Status Red: "));Serial.println(StatusRed); delay(DelayWrite);
                 Serial.print(F("     Type mode green: "));Serial.println(ModeAccRed); delay(DelayWrite);
                Serial.print(F("     Number green pin: "));Serial.println(PinGreen); delay(DelayWrite);
                Serial.print(F("     Status Green: "));Serial.println(StatusGreen); delay(DelayWrite);
                Serial.print(F("     Servo Position: "));Serial.println(posServo); delay(DelayWrite);
                Serial.print(F("     Inverted operating mode: "));Serial.println(Inverted); delay(DelayWrite); 
                }
                Serial.println(F("")); delay(DelayWrite);
                delay(DelayProg);
                EEPROM.put(NumberAcc,PortAddress);
                EEPROM.put(NumberAcc+16,Buddy);
                if(ChoosePin==0 && (ModeAcc==6)||(ModeAcc==7)) {
                EEPROM.put(NumberAcc+32,PinRed);
                EEPROM.put(NumberAcc+64,ModeAcc);
                }
                if(ChoosePin==1 && (ModeAcc==6)||(ModeAcc==7)) {
                EEPROM.put(NumberAcc+80,PinGreen);
                EEPROM.put(NumberAcc+112,ModeAcc);
                }
                if ((ModeAcc!=6)&&(ModeAcc!=7)) {
                EEPROM.put(NumberAcc+32,PinRed);
                EEPROM.put(NumberAcc+64,ModeAcc);
                EEPROM.put(NumberAcc+80,PinGreen);
                EEPROM.put(NumberAcc+112,ModeAcc);
                }
                if(EEPROM.read(NumberAcc+128) == 0) {EEPROM.put(NumberAcc+128,posServo);}
                EEPROM.put(NumberAcc+144,Inverted);
                
      
                            
  
  Serial.println(F("Values saved succesfully")); delay(DelayWrite);
  delay(200);
  Programmer();
  delay(DelayProg);
}
else {
  Serial.println(F("")); delay(DelayWrite);
  Serial.println(F("Value incorrect, must be in range 1-16 / 201-216 / 401-416 / 601-616 / 801-816")); delay(DelayWrite);
  Serial.println(F("")); delay(DelayWrite);
  delay(200);
  Programmer();
  delay(DelayProg);
}
} NumberAcc=0;
}
//Uploading stored EEPROM values to accessories before in operating mode.
void StartupLoadVariables() {
  for (NumberAcc = 0 ; NumberAcc < 816 ; NumberAcc++) {
    SetVariables();
    if(((NumberAcc>0 && NumberAcc<17) ||
        (NumberAcc>=201 && NumberAcc<=216) ||
        (NumberAcc>=401 && NumberAcc<=416) ||
        (NumberAcc>=601 && NumberAcc<=616) ||
        (NumberAcc>=801 && NumberAcc<=816)) && (PinRed>0 || PinGreen>0)) {
          ModeAccExecRed();
          ModeAccExecGreen();
          ShowHeader();
          ShowStoreVal(); 
          }
    }
}


////Procedures for debugging
//Print MM command
void MMcommand(String message) {
  Serial.print(String(millis()));Serial.print(F(" [MÄRKLIN INPUT]: "));Serial.println(message);
}
//Serial print header with number accessorie
void ShowHeader() {
  Serial.print(F("----------| Values accessorie number: "));Serial.print(NumberAcc);Serial.println(F(" |----------"));
}
//Serial print variable values when receiving Motorola command
void ShowComVal() {
  Serial.print(F("Digital address: "));Serial.println(PortAddress);
  Serial.print(F("Value MM command: "));Serial.println(TaskRGcommand);
  }
//Serial print variable values before EEPROM storing
void ShowStoreVal() {
  Serial.print(F("Inverted: "));Serial.println(Inverted); 
  Serial.print(F("Command TaskRG: "));Serial.println(TaskRG);
  Serial.print(F("Buddy active: "));Serial.println(Buddy);
  Serial.print(F("Number red and green pin: "));Serial.print(PinRed);Serial.print(F(" / "));Serial.println(PinGreen);
  Serial.print(F("Status Red and Green: "));Serial.print(StatusRed);Serial.print(F(" / "));Serial.println(StatusGreen);
  Serial.print(F("Type mode red and green: "));Serial.print(ModeAccRed);Serial.print(F(" / "));Serial.println(ModeAccGreen);
  Serial.print(F("Servo Position: "));Serial.println(posServo);
  Serial.println(F(""));
}
//Serial print variable values used in memorie sketch
void ShowMemVal() {
  Serial.print(F("Number red and green pin mem: "));Serial.print(PinRedMem);Serial.print(F(" / "));Serial.println(PinGreenMem);
  Serial.print(F("Status Red and Green mem: "));Serial.print(StatusRedMem);Serial.print(F(" / "));Serial.println(StatusGreenMem);
  Serial.print(F("Type mode mem in sketch code: "));Serial.println(ModeAccMem);
  Serial.print(F("Type mode red and green mem: "));Serial.print(ModeAccRedMem);Serial.print(F(" / "));Serial.println(ModeAccGreenMem);
  Serial.print(F("Servo Position mem: "));Serial.println(posServoMem);
  Serial.print(F("Inverted: "));Serial.println(InvertedMem); 
  Serial.println(F(""));
}
//Serial print stored EEPROM values (trigger NumberAcc)
void ShowEEPROMVal() {
  if (TaskRG != 0) {ShowComVal();}
  Serial.print(F("Buddy active: "));Serial.println(EEPROM.read(NumberAcc+16));
  Serial.print(F("Number red and green pin: "));Serial.print(EEPROM.read(NumberAcc+32));Serial.print(F(" / "));Serial.println(EEPROM.read(NumberAcc+80));
  Serial.print(F("Status Red and Green: "));Serial.print(EEPROM.read(NumberAcc+48));Serial.print(F(" / "));Serial.println(EEPROM.read(NumberAcc+96));
  Serial.print(F("Type mode red and green: "));Serial.println(EEPROM.read(NumberAcc+64));Serial.print(F(" / "));Serial.println(EEPROM.read(NumberAcc+64));
  Serial.print(F("Servo Position: "));Serial.println(EEPROM.read(NumberAcc+128)); 
  Serial.println(F(""));
}



////Procedures for loading and saving variables
//Loads stored EEPROM values of the NumberAcc into the variables
void SetVariables(){
  PinRed=EEPROM.read(NumberAcc+32);
  StatusRed=EEPROM.read(NumberAcc+48);    
  ModeAccRed=EEPROM.read(NumberAcc+64);
  PinGreen=EEPROM.read(NumberAcc+80);
  StatusGreen=EEPROM.read(NumberAcc+96);
  ModeAccGreen=EEPROM.read(NumberAcc+112);
  posServo=EEPROM.read(NumberAcc+128);
  Inverted=EEPROM.read(NumberAcc+144);
  SetBuddy();
}
//Loads stored EEPROM values of the NumberAcc into the memvariables
void SetVariablesMem(){
  PinRedMem=EEPROM.read(NumberAccVar+32);
  StatusRedMem=EEPROM.read(NumberAccVar+48);    
  ModeAccRedMem=EEPROM.read(NumberAccVar+64);
  PinGreenMem=EEPROM.read(NumberAccVar+80);
  StatusGreenMem=EEPROM.read(NumberAccVar+96);
  ModeAccGreenMem=EEPROM.read(NumberAccVar+112);
  posServoMem=EEPROM.read(NumberAccVar+128);
  InvertedMem=EEPROM.read(NumberAccVar+144);
  SetBuddy();
}
//Converts the stored buddy value into buddy NumberAcc variable 
void SetBuddy() {
    if(EEPROM.read(NumberAcc+16)==1){Buddy=NumberAcc+200;}
    else {Buddy=0;}
}
//Saves variables of the NumberAcc into the EEPROM
void SaveVariables(){
    EEPROM.put(NumberAcc+32,PinRed);    
    EEPROM.put(NumberAcc+48,StatusRed);
    EEPROM.put(NumberAcc+64,ModeAccRed);
    EEPROM.put(NumberAcc+80,PinGreen);
    EEPROM.put(NumberAcc+96,StatusGreen);
    EEPROM.put(NumberAcc+112,ModeAccGreen);
    EEPROM.put(NumberAcc+128,posServo);
    EEPROM.put(NumberAcc+144,Inverted);
}
//Saves mem variables of the NumberAcc into the EEPROM
void SaveVariablesMem(){
    EEPROM.put(NumberAccVar+48,StatusRed);
    EEPROM.put(NumberAccVar+96,StatusGreen);
    EEPROM.put(NumberAccVar+128,posServo);
}

////Procedures for excecuting MM commando's
//Check if MMcommand is different
void ExecMMcommand() {
  if(TaskRGcommand==TaskRGcommandMem && PortAddress==PortAddressMem && (millis()-100)<TimeCommand) {}
  else {
  PortAddressMem=PortAddress;
  TaskRGcommandMem=TaskRGcommand;
  NumberAcc=1;
  ChkPortAddress();
  } 
}

//Check PortAddress at NumberAcc
void ChkPortAddress() {
  if(EEPROM.read(NumberAcc)==PortAddressMem) {
    ShowHeader();
    TaskRedGreen();
    TimeCommand=millis();
    }
  else {
    if(NumberAcc<16) {
      NumberAcc=NumberAcc+1;
      ChkPortAddress();
      }
    }
}

//Switching task for variable inverted
void SetInverted() {
  if(TaskRGcommand==1 && Inverted==0) {TaskRG=1;} 
  if(TaskRGcommand==2 && Inverted==0) {TaskRG=2;}
  if(TaskRGcommand==1 && Inverted==1) {TaskRG=2;}
  if(TaskRGcommand==2 && Inverted==1) {TaskRG=1;}
}
//Function to match with MM-commando
void TaskRedGreen() {
  SetVariables();
  SetInverted();  
  if(TaskRG==1 && StatusRed!=1 && NumberAcc>=1) {StatusRed=1;
    if (TaskRG==1 && ModeAcc!=7 && ModeAcc!=8) {StatusGreen=0;}
    ModeAccExecRed();}
  else if(TaskRG==1 && StatusRed==1 && NumberAcc>=1 && ((ModeAcc==6)||(ModeAcc==7))) {
    StatusRed=0; 
    ModeAccExecRed();}
  else if(TaskRG==1 && StatusRed==1 && StatusGreen==1 && NumberAcc>=1 && ((ModeAcc!=6)||(ModeAcc!=7))) {
    StatusRed=1; StatusGreen=0; 
    ModeAccExecRed();}
  else if(TaskRG==2 && StatusGreen!=1 && NumberAcc>=1) {StatusGreen=1;
    if (TaskRG==2 && ModeAcc!=7 && ModeAcc!=8) {StatusRed=0;}
    ModeAccExecGreen();}
  else if(TaskRG==2 && StatusGreen==1 && NumberAcc>=1 && ((ModeAcc==6)||(ModeAcc==7))) {
    StatusGreen=0; 
    ModeAccExecGreen();}
  else if(TaskRG==2 && StatusGreen==1 && StatusRed==1 && NumberAcc>=1 && ((ModeAcc!=6)||(ModeAcc!=7))) {
    StatusGreen=1; StatusRed=0;}
  else{}
  ShowStoreVal();
}
//Select pinmode for red pin
void ModeAccExecRed(){
  ModeAcc=ModeAccRed;
  if(TaskRG==0 || TaskRG==1) {
      if((ModeAcc==1) || (ModeAcc==3)) {SetCoil();}
      else if(ModeAcc==2 || ModeAcc==4) {SetServoPos();}
      else if(ModeAcc==5) {LightsSwitch();}
      else if(ModeAcc==6) {LightsOnOff();}
      else if(ModeAcc==7) {SetCoil();}
      else {}}
    SaveVariables();
   if(Buddy!=0 && TaskRG!=0){
      NumberAcc=Buddy;
     TaskRedGreen();
   }
}
//Select pinmode for green pin
void ModeAccExecGreen() {
  ModeAcc=ModeAccGreen;
  if (TaskRG==0 || TaskRG==2) {
      if((ModeAcc==1) || (ModeAcc==3)) {SetCoil();}
      else if(ModeAcc==2 || ModeAcc==4) {SetServoPos();}
      else if(ModeAcc==5) {LightsSwitch();}
      else if(ModeAcc==6) {LightsOnOff();}
      else if(ModeAcc==7) {SetCoil();}
      else {}}
    SaveVariables();
    if(Buddy!=0 && TaskRG!=0){
      NumberAcc=Buddy;
     TaskRedGreen();
    }
}

////Procedures for excecuting modes
//Servo mode
void SetServoPos() {
  byte ServoMin=20;
  byte ServoMax=120;
  int DelayServo=10;
  if(TaskRG!=0){SetupMemPin();} 
  if(TaskRG==1 && StatusRed==1 && posServo==posServoMem) {
          myservo.attach(PinRed);
          for (posServo >= ServoMin; posServo < ServoMax; posServo ++){
          myservo.write(posServo);
          delay(DelayServo);}
          }
      if (TaskRG==2 && StatusGreen==1 && posServo==posServoMem) {
          myservo.attach(PinGreen);
          for (posServo <= ServoMax; posServo > ServoMin; posServo --){
          myservo.write(posServo);
          delay(DelayServo);}
          }
      myservo.detach();
      SaveVariablesMem();
}
//Setup search function memory values with same pin as servo
void  SetupMemPin() {
  NumberAccVar=NumberAcc;
  NumberAccMem=NumberAcc;
  ModeAccMem=ModeAcc;
  PinRedMem=PinRed;
  PinRedMem=PinGreen;
  StatusRedMem=StatusRed;
  StatusGreenMem=StatusGreen;
  Inverted=InvertedMem;
  SetMemPin();
}
//Search function memory values with same pin as servo
void  SetMemPin() {
        NumberAccVar++;
        SetVariablesMem();
      if (TaskRG == 1 && PinRed==PinRedMem && ModeAcc==ModeAccMem) {
         ShowMemVal();
         SetVariablesMem();}
      else if (TaskRG == 2 && PinGreen==PinGreenMem && ModeAcc==ModeAccMem) {
         ShowMemVal();
         SetVariablesMem();}
      else {
        if (NumberAcc==NumberAccVar) {
          ShowHeader();
          }
      else if (NumberAccMem!=NumberAccVar && NumberAccVar>=0 && NumberAccVar<=816) {
          if (NumberAccVar==816) {NumberAccVar=0;}
          if ((NumberAccVar==16)||
             (NumberAccVar==216)||
             (NumberAccVar==416)||
             (NumberAccVar==616)) {NumberAccVar+=184;}
          SetMemPin();}
        }
}
//Coil mode
void SetCoil() {
  int DelayCoil; {
    if(ModeAcc==1) {
    DelayCoil = 1000;}
    else if (ModeAcc==3)  {
    DelayCoil = 1000;}
    else if (ModeAcc==7)  {
    DelayCoil = 2000;}
    }
      if (TaskRG==1) {
      digitalWrite(PinRed,HIGH); 
      delay(DelayCoil);
      digitalWrite(PinRed,LOW);
      delay(DelayCoil/2);    
      }
      if (TaskRG==2) {
      digitalWrite(PinGreen,HIGH);
      delay(DelayCoil);
      digitalWrite(PinGreen,LOW);
      delay(DelayCoil/2);    
      }
}
//Mode for switching lights red/green
void LightsSwitch() {
  int DelayLed=100;
  if (TaskRG==0) {
    if (StatusRed == 1) {digitalWrite(PinRed, HIGH);}
    if (StatusGreen == 1) {digitalWrite(PinGreen, HIGH);}
  }
  if (TaskRG==1 && StatusRed == 1 && StatusGreen == 0) {digitalWrite(PinGreen, LOW); delay(DelayLed); digitalWrite(PinRed, HIGH); delay(DelayLed);}
  if (TaskRG==2 && StatusGreen == 1 && StatusRed == 0) {digitalWrite(PinRed, LOW); delay(DelayLed); digitalWrite(PinGreen, HIGH); delay(DelayLed);}
}
//Mode for switching lights on/off
void LightsOnOff() {
  if (TaskRG==0 && StatusRed == 1) {digitalWrite(PinRed, HIGH);}
  if (TaskRG==0 && StatusGreen == 1) {digitalWrite(PinGreen, HIGH);}
  if (TaskRG==1 && StatusRed == 0) {digitalWrite(PinRed, LOW);}
  if (TaskRG==1 && StatusRed == 1) {digitalWrite(PinRed, HIGH);}
  if (TaskRG==2 && StatusGreen == 0) {digitalWrite(PinGreen, LOW);}
  if (TaskRG==2 && StatusGreen == 1) {digitalWrite(PinGreen, HIGH);}
}



//Setup values for first start ARDIMO
void setup() {
attachInterrupt(digitalPinToInterrupt(INPUT_PIN), isr, CHANGE);

//use timer1 to blink onboard LED when there is data on the bus, pin 13 is excluded from usage
pinMode(LED_BUILTIN, OUTPUT);
pinMode(3, OUTPUT);
pinMode(4, OUTPUT);
pinMode(5, OUTPUT);
pinMode(6, OUTPUT);
pinMode(7, OUTPUT);
pinMode(8, OUTPUT);
pinMode(9, OUTPUT);
pinMode(10, OUTPUT);
pinMode(11, OUTPUT);
pinMode(12, OUTPUT);
pinMode(14, OUTPUT);
pinMode(15, OUTPUT);
pinMode(16, OUTPUT);
pinMode(17, OUTPUT);
pinMode(18, OUTPUT);
pinMode(19, OUTPUT);
 
TCCR1A = 0;
TCCR1B = 0;
bitSet(TCCR1B, CS12);  // 256 prescaler
bitSet(TIMSK1, TOIE1); // timer overflow interrupt
 
Serial.begin(115200);
Serial.println(F("Starting up ARDIMO"));
Serial.println(F(""));
delay(1000);
StartupARDIMO();
Serial.println(F("Ready for use!"));

}

//Loop 
void loop() {
  ExecuteMMdata();
}

  
//Function reading data mm commando's  
void ExecuteMMdata() {
  mm.Parse(); 
  MaerklinMotorolaData* Data = mm.GetData();
  if(Data) {
    if(Data->IsMagnet && Data->MagnetState) {
      hasNewData = true;
      MMcommand("Decoder # " + String(Data->PortAddress) + " Switch " + (Data->DecoderState));
      PortAddress=(Data->PortAddress); {
        if (Data->DecoderState == MM2DecoderState_Red) {
        TaskRGcommand=1;             
        } 
        else if (Data->DecoderState == MM2DecoderState_Green) {
        TaskRGcommand=2;
        }
      }
      ExecMMcommand();
    }
  }
}
//Interrupt
void isr() {
  mm.PinChange();
}
ISR(TIMER1_OVF_vect) {
  /*if (hasNewData) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    hasNewData = false;
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }*/
}
