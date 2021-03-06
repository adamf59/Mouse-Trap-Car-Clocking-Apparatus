/*
   Emile Hamwey and Adam Frank
   https://github.com/adamf59
   af.frank.2001@gmail.com
   
*/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include <Chrono.h>


#define YP A3  
#define XM A2  
#define YM 9   
#define XP 8   
#define TS_MINX 210
#define TS_MINY 210
#define TS_MAXX 915
#define TS_MAXY 910
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4
#define BLACK     0x0000
#define BLUE      0x001F
#define GREY      0xCE79
#define LIGHTGREY 0xDEDB
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define MINPRESSURE 1
#define MAXPRESSURE 1000




#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978


Chrono TRACK_TIMER(Chrono::MICROS);
#include <Adafruit_DotStar.h>
#include <SPI.h>         

#define NUMPIXELS 72 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN    20
#define CLOCKPIN   21
Adafruit_DotStar strip = Adafruit_DotStar(
                           NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 364);

//2.4 = 240 x 320
//Height 319 to fit on screen

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

int X, Y, Z;

double TRACK_AVERAGE = 0.0;
double TRACK_BEST = 9.0;
double TRACK_1_RESULT = 0.0;
double TRACK_2_RESULT = 0.0;
double TRACK_3_RESULT = 0.0;
double TRACK_4_RESULT = 0.0;

int CURRENT_SCREEN = 0; //0 is Ready screen | 1 is Countdown screen | 2 is Clocking screen | 3 is Reset screen
boolean TRACK_1_FINISHED;
boolean TRACK_2_FINISHED;
boolean TRACK_3_FINISHED;
boolean TRACK_4_FINISHED;

int countPixel = 0;


int screenSizer = 15; // add 30 each iteration


void setup() {
  strip.begin(); // Initialize pins for output
  strip.setBrightness(40);
  strip.show();  // Turn all LEDs off ASAP
  Serial.begin(9600);


#ifdef USE_ADAFRUIT_SHIELD_PINOUT
  Serial.println(F("Using Adafruit 2.8\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Adafruit 2.8\" TFT Breakout Board Pinout"));
#endif

  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();
  

  uint16_t identifier = tft.readID();
identifier=0x9341;
  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    return;
  }

  tft.begin(identifier);
  tft.setRotation(1);
  
  
  drawReadyScreen();
  Serial.println(F("setup is complete."));

  TRACK_TIMER.restart();
    tone(35, NOTE_C6, 430);


  pinMode(45, INPUT);
  pinMode(44, INPUT);
  pinMode(43, INPUT);
  pinMode(42, INPUT);

  digitalWrite(45, HIGH); // TRACK 1
  digitalWrite(44, HIGH); // TRACK 1
  digitalWrite(43, HIGH); // TRACK 1
  digitalWrite(42, HIGH); // TRACK 1


}


/**
   WIDTH IS 320
   HEIGHT IS 230
*/
void loop() {

if(CURRENT_SCREEN == 2) {
  
  if (digitalRead(45) == LOW) {
    if (!TRACK_1_FINISHED) {
      clockTimeForTrack(1);
    }
  }
  if (digitalRead(44) == LOW) {
    if (!TRACK_2_FINISHED) {
      clockTimeForTrack(2);
    }
  }
  if (digitalRead(43) == LOW) {
    if (!TRACK_3_FINISHED) {
      clockTimeForTrack(3);
    }
  }
  if (digitalRead(42) == LOW) {
    if (!TRACK_4_FINISHED) {
      clockTimeForTrack(4);
    }
  }
}



  //get the most recent touch update from TFT
  TFT_GET_TOUCH();
  //on a hard enough press, this statement runs:
  if (Z > MINPRESSURE && Z < MAXPRESSURE) {

    if (CURRENT_SCREEN == 0) {
      //"start" button
      if (X > 143 && X < 269 && Y > 43 && Y < 341) {
        beginCountdown();

      }

    }

    

    if (CURRENT_SCREEN == 3 || CURRENT_SCREEN == 2) {
      if (X > 250) {
        Serial.print("RESETTING");
        reset();
        drawReadyScreen();

      }

     

    }


    Serial.println(String("got touch at ") + X + String(",") + Y + String(",") + Z); // use this to get corners of button bounding box


  }

  //delay(250);
  if (CURRENT_SCREEN == 0) {
    setPixelColorNoShow(countPixel + 1, 0, 0, 0);
    setPixelColorNoShow(countPixel, 255, 255, 255);
    setPixelColorNoShow(countPixel - 1, 0, 0, 0);
    strip.show();
    if (countPixel == 72) {
      countPixel = 0;
    }
    countPixel ++;
  }

}


void reset() {
  TRACK_1_FINISHED = false;
  TRACK_2_FINISHED = false;
  TRACK_3_FINISHED = false;
  TRACK_4_FINISHED = false;
  screenSizer = 45;

    
    tone(35, NOTE_E4, 100);

}



void TFT_GET_TOUCH()
{
  digitalWrite(13, HIGH);
  TSPoint point = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  X = tft.width() - map(point.x, TS_MAXX, TS_MINX, 0, tft.width());
  Y = map(point.y, TS_MAXY, TS_MINY, 0, tft.height());
  Z = point.z;
}


void drawClockingScreen() {

  CURRENT_SCREEN = 2;
  tft.fillScreen(BLACK);
  tft.fillRect(40, 3, 240, 50, GREEN);
  tft.setCursor(60, 13);
  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  tft.println("CLOCKING");
  tft.setCursor(60, 15);
  tft.setTextSize(1);
  tft.setCursor(30, 230);
  tft.setTextColor(WHITE);
  tft.println("Tap \"clocking\" at any time to reset.");

  tft.fillRect(40, 3, 240, 50, GREEN);
  tft.setCursor(60, 13);
  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  tft.println("CLOCKING");



//  // DNF 
//  Serial.println("Drawing DNF");
//    tft.setCursor(0, 165);
//  tft.setTextColor(RED);
//  tft.setTextSize(4);
//  tft.println("DQ:");
//  
//  tft.fillRect(0,200, 70, 50, RED);
//    tft.setCursor(0, 200);
//  tft.setTextColor(BLACK);
//  tft.setTextSize(4);
//  tft.println("T1");
//
//
//  tft.fillRect(80,200, 70, 50, RED);
//    tft.setCursor(80, 200);
//  tft.setTextColor(BLACK);
//  tft.setTextSize(4);
//  tft.println("T2");
//
//  tft.fillRect(160,200, 70, 50, RED);
//    tft.setCursor(160, 200);
//  tft.setTextColor(BLACK);
//  tft.setTextSize(4);
//  tft.println("T3");
//
//  tft.fillRect(240,200, 70, 50, RED);
//    tft.setCursor(240, 200);
//  tft.setTextColor(BLACK);
//  tft.setTextSize(4);
//  tft.println("T4");

}




void clockTimeForTrack(int track) {
  //assume screen is already on clocking screen, so we can draw "TRACK xyz: xy:zzz w/o rerunning the entire drawClocking method
  Serial.println(String("GOT TRACK ") + track);

  screenSizer = screenSizer + 35;
  tft.fillRect(40, screenSizer, 240, 30, RED);
  tft.setCursor(60, (screenSizer + 10));
  tft.setTextColor(BLACK);
  tft.setTextSize(2);

  if (track == 1) {
    TRACK_1_FINISHED = true;
    TRACK_1_RESULT = (double)TRACK_TIMER.elapsed() / 1000000;
    tft.println(String("TRACK ") + track + String(": ") + TRACK_1_RESULT);
    setPixelColor(0, 18, 0, 30, 30);

  }
  if (track == 2) {
    TRACK_2_FINISHED = true;
    TRACK_2_RESULT = (double)TRACK_TIMER.elapsed() / 1000000;
    tft.println(String("TRACK ") + track + String(": ") + TRACK_2_RESULT);
        setPixelColor(18, 36, 0, 30, 30);

  }
  if (track == 3) {
    TRACK_3_FINISHED = true;
    TRACK_3_RESULT = (double)TRACK_TIMER.elapsed() / 1000000;
    tft.println(String("TRACK ") + track + String(": ") + TRACK_3_RESULT);
    setPixelColor(36, 54, 0, 30, 30);

  }
  if (track == 4) {
    TRACK_4_FINISHED = true;
    TRACK_4_RESULT = (double)TRACK_TIMER.elapsed() / 1000000;
    tft.println(String("TRACK ") + track + String(": ") + TRACK_4_RESULT);
    setPixelColor(54, 72, 0, 30, 30);

  }


  if (TRACK_1_FINISHED && TRACK_2_FINISHED && TRACK_3_FINISHED && TRACK_4_FINISHED) {
    Serial.println("Completed Clocking Period!");


    tft.fillRect(0, 3, 350, 50, YELLOW);
    tft.setCursor(130, 13);
    tft.setTextColor(BLACK);
    tft.setTextSize(4);
    tft.println("RESET");
    setPixelColor(0, 72, 0, 0, 30);
    countPixel = 0;
    CURRENT_SCREEN = 3;

    updateAverage();
    updateBest();
  }

}






void drawReadyScreen() {
  CURRENT_SCREEN = 0;
  //TRACK_FINISHED = false;
  tft.fillScreen(BLACK);
  setPixelColor(0, 71, 0, 0, 0);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(1, 5);
  tft.println(String("Average: ") + TRACK_AVERAGE);
  tft.setCursor(200, 5);
  tft.println(String("Best: ") + TRACK_BEST);
  tft.fillRect(0, 30, 320, 3, RED);

  tft.setTextSize(2);
  tft.setTextColor(MAGENTA);
  tft.setCursor(20, 50);
  tft.println("Ensure tracks are ready!");

  tft.fillRect(40, 95, 240, 50, GREEN);
  tft.setCursor(95, 108);
  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  tft.println("START!");

  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(0, 230);
  tft.println("Designed by Adam Frank and Emile Hamwey");



  
}






void beginCountdown() {
  CURRENT_SCREEN = 1;
  tft.setTextSize(4);
  tft.setCursor(1, 5);
  tft.fillScreen(BLACK);
  tft.setTextColor(YELLOW);
  tft.println("READY?");
  setPixelColor(0, 72, 0, 0, 0);
  for (int i = 0; i < 72; i++) {
    setPixelColor(i, 200, 255, 0);
    delay(20);
  }
  for (int i = 72; i > -1; i--) {
    setPixelColor(i, 0, 0, 0);
    delay(20);
  }

  tft.fillScreen(BLACK); 



  tft.setTextSize(18);
  tft.setTextColor(WHITE);



  for (int i = 3; i > 0; i--) {
    tft.setCursor(50, 50);
    tft.fillScreen(BLACK);
    tft.println(i);
    tone(35, NOTE_C4, 430);

    setPixelColor(24 * (-i + 3), 24 * (-i + 4), 200, 255, 0);
    delay(600);
  }
  delay(400);
  
  setPixelColor(0, 72, 255, 0, 0);
  tone(35, NOTE_C5, 600);

  tft.setCursor(50, 50);
  tft.fillScreen(BLACK);
  tft.setTextColor(GREEN);
  tft.println("GO");
  TRACK_TIMER.restart();
  delay(500);
  drawClockingScreen();
}



/*
  Leaderboard:
*/
boolean firstRun = false;

void updateAverage() {
  if (!firstRun) {
    TRACK_AVERAGE = (TRACK_1_RESULT + TRACK_2_RESULT + TRACK_3_RESULT + TRACK_4_RESULT) / 4;
    firstRun = true;
  } else {
    double this_track_average = (TRACK_1_RESULT + TRACK_2_RESULT + TRACK_3_RESULT + TRACK_4_RESULT) / 4 ;
    TRACK_AVERAGE = (TRACK_AVERAGE + this_track_average) / 2;
  }

}

void updateBest() {
  if (TRACK_1_RESULT < TRACK_BEST) TRACK_BEST = TRACK_1_RESULT;
  if (TRACK_2_RESULT < TRACK_BEST) TRACK_BEST = TRACK_2_RESULT;
  if (TRACK_3_RESULT < TRACK_BEST) TRACK_BEST = TRACK_3_RESULT;
  if (TRACK_4_RESULT < TRACK_BEST) TRACK_BEST = TRACK_4_RESULT;
}

/*
   LED Methods:
*/
void setPixelColor(int startIndex, int endIndex, int g, int r, int b) {
  for (int i = startIndex; i < endIndex; i++) {
    strip.setPixelColor(i, g, r, b);
  }
  strip.show();
}
void setPixelColor(int index, int g, int r, int b) {

  strip.setPixelColor(index, g, r, b);

  strip.show();
}
void setPixelColorNoShow(int index, int g, int r, int b) {

  strip.setPixelColor(index, g, r, b);

 
}
