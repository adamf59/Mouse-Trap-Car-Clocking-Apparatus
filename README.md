# Mouse Trap Car Clocking Apparatus
Mouse Trap Car Timing System:
afrank20@windhamsd.org, ehamwey19@windhamsd.org
## Build 1.1, Released 11/1/19

## USAGE:
1.	Ensure power is supplied to the Arduino via USB cable (the “START!” screen should be shown)
2.	Ensure speaker is plugged in via USB if internal speaker battery is low and speaker is turned on
3.	If system initializes correctly, speaker will beep upon power-up and LED strip will begin the waiting animation (white singular LED chasing up the strip)
4.	Position cars on track 
5.	When teams are ready, press and hold “START”. The LED strip will play the yellow startup animation, and then there is a one second delay between the completion of the yellow startup animation and the countdown beginning.
6.	The LED strip will light up ⅓, ⅔ and then full yellow, for 3, 2, 1, and then green for GO. 
7.	The strip will turn blue once all scores have successfully been recorded.
8.	Press RESET to set the track up for the next race.
9.	During any clocking period, press “CLOCKING” to reset the track if there is a false start or misfire. 

## PINOUT GUIDE:
### Arduino to LED Strip
- 20 - SDA to White solid core wire,  Green LED strip wire
- 21 - SCL to Yellow solid core wire
- Digital GND to Black solid core wire
- Digital 5V to Red solid core wire

### Arduino to Track Switches
- 45 to Track 1 
- 44 to Track 2
- 43 to Track 3
- 42 to Track 4

### Arduino to Speaker
- Any available GND to Speaker / Potentiometer GND
- Digital 35 to Speaker 3.5mm jack (should go through potentiometer)



## REQUIRED LIBRARIES IF EDITING:
The Adafruit TFT LCD uses a customized version of the TFTLCD Library. If editing, follow the following tutorial:
http://educ8s.tv/arduino-touch-screen-tutorial/

- Adafruit GFX Library (https://github.com/adafruit/Adafruit-GFX-Library)
- Adafruit TFTLCD Library (https://github.com/adafruit/TFTLCD-Library)
- Chrono (https://github.com/SofaPirate/Chrono)
- Adafruit DotStar Library (https://github.com/adafruit/Adafruit_DotStar)
