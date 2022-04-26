This is an Arduino decoder sketch with library for decoding the signals from the Märklin-Motorola-protocol.
Credits for the library are fully for GitHub user Laserlight, all the files are in the forked repository https://github.com/buzz4stroke/MaerklinMotorola 

The sketch for controlling the accessories is my own design.
You can control the Arduino with signals from your central control if it is sending the MM2 protocol.
Examples are: Marklin 6021, Marklin Mobile station (not version 1), Marklin Central Station, Intellibox, ESU ECOS and more, 
computer controlled layouts are no problem, as long as the signal is the Marklin MM2 protocol.

The name ARDIMO_wise is an acronym for ARduino DIgital MOtorola. The suffix is an acronym for wi(ssels) en se(inen) which is
Dutch for turnout and signals.

Accessories to be controlled:
1. Turnout with coil
2. Turnout with servo
3. Signal with coil
4. Signal with servo
5. Signal with lights
6. Lights on/off
7. Decoupler rail with coil


Signal polarity from the rail isn't relevant. Library is working with timings only.

**Example-Circuit to get required signal to Arduino:**
<img width="100%" src="https://github.com/buzz4stroke/ARDIMO_wise1.0/blob/main/OptocouplerArduino.JPG">

**Additional informations about the protocol**

Protocol:
- http://home.mnet-online.de/modelleisenbahn-digital/Dig-tutorial-start.html

- http://www.drkoenig.de/digital/motorola.htm

- http://www.skrauss.de/modellbahn/Schienenformat.pdf (MFX)
