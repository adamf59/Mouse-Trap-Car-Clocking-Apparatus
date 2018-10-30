/*
   Emile Hamwey and Adam Frank
   https://github.com/adamf59
   af.frank.2001@gmail.com
   
*/

#include <Adafruit_GFX.h>
#include <TouchScreen.h>
#include <Adafruit_TFTLCD.h>
#include <Chrono.h>


#define YP A3  
#define XM A2  
#define YM 9   
#define XP 8   
#define TS_MINX 210
#define TS_MINY 210
#define TS_MAXX 915
#define TS_MAXY 910
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
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



#define NOTE_C4  262
#define NOTE_C5  523
#define NOTE_C6  1047

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


int screenSizer = 45; // add 30 each iteration


void setup() {
  strip.begin(); // Initialize pins for output
  strip.setBrightness(40);
  strip.show();  // Turn all LEDs off ASAP
  Serial.begin(9600);
  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.setRotation(1);
  //Background color
  tft.fillScreen(BLACK);

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



  //get the most recent touch update from TFT
  TFT_GET_TOUCH();
  //on a hard enough press, this statement runs:
  if (Z > MINPRESSURE && Z < MAXPRESSURE) {

    if (CURRENT_SCREEN == 0) {
      //"start" button
      if (X > 100 && X < 170 && Y > 35 && Y < 250) {
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

    
    tone(35, NOTE_D2, 1000);

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
  }
  if (track == 2) {
    TRACK_2_FINISHED = true;
    TRACK_2_RESULT = (double)TRACK_TIMER.elapsed() / 1000000;
    tft.println(String("TRACK ") + track + String(": ") + TRACK_2_RESULT);
  }
  if (track == 3) {
    TRACK_3_FINISHED = true;
    TRACK_3_RESULT = (double)TRACK_TIMER.elapsed() / 1000000;
    tft.println(String("TRACK ") + track + String(": ") + TRACK_3_RESULT);
  }
  if (track == 4) {
    TRACK_4_FINISHED = true;
    TRACK_4_RESULT = (double)TRACK_TIMER.elapsed() / 1000000;
    tft.println(String("TRACK ") + track + String(": ") + TRACK_4_RESULT);
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
    delay(800);
  }
  delay(165);
  
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
