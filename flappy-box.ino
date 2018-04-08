/* Flappy box for Arduino.
 * by Zach Collins*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

const int buttonPin = 3;
volatile int elevation;
volatile bool fall = true;
volatile bool start = false;
volatile uint8_t updateCounter = 0;
uint16_t shiftCounter = 0;
Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

void setup() {
  Serial.begin(9600);
  Serial.println("8x8 LED Matrix Test");

  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), button_ISR, RISING);
  matrix.begin(0x70);  // pass in the address
  matrix.clear();
  matrix.writeDisplay();
  elevation = 3;
  
  while (start == false);
  randomSeed(analogRead(0));
}

uint8_t walls[8];
uint8_t createWall = 0;
uint16_t score = 0;
uint16_t hiscore = 0;

void loop() {
  matrix.drawBitmap(0, 0, walls, 8, 8, LED_GREEN);
  matrix.drawPixel(1, elevation, LED_YELLOW);
  matrix.writeDisplay();
  matrix.clear();
  if (elevation < 0 || elevation > 7 || (walls[elevation] & B01000000)) {
    gameEnd();
  }
  if (updateCounter >= 255) {
    if (fall) {
      elevation += 1;
    } else {
      fall = true;
      elevation -= 1;
    }
    updateCounter = 0;
  } else {
    updateCounter++;
  }
  if (shiftCounter == 255) {
    shiftWalls();
    shiftCounter = 0;
  } else {
    shiftCounter++;
  }
}

void gameEnd() {
  matrix.fillRect(0, 0, 8, 8, LED_RED);
  matrix.writeDisplay();
  delay(200);
  matrix.clear();
  matrix.writeDisplay();
  displayScore();
  for (uint8_t i = 0; i < 8; i++) {
    walls[i] = 0;
  }
  start = false;
  while (start == false);
  elevation = 3;
  updateCounter = 0;
  shiftCounter = 0;
  score = 0;
}

void displayScore() {
  String msg;
  if (score > hiscore) {
    hiscore = score;
    msg = "New high score! ";
  } else {
    msg = "Score: ";
  }
  msg += String(score);
  matrix.setTextWrap(false);
  matrix.setTextSize(1);
  matrix.setTextColor(LED_RED);
  long msglength = msg.length();
  for (int8_t i = 7; i >= -6*msglength; i--) {
    matrix.clear();
    matrix.setCursor(i, 0);
    matrix.print(msg);
    matrix.writeDisplay();
    delay(60);
  }
}

void shiftWalls() {
  for (uint8_t i = 0; i < 8; i++) {
    walls[i] = walls[i] << 1;
  }
  if (createWall == 3) {
    uint8_t wallHeight = random(4, 7);
    uint8_t topHalf = random(1, wallHeight);
    for (uint8_t i = 0; i < topHalf; i++) {
      walls[i] |= 1;
    }
    for (uint8_t i = 7; i >= 8 - (wallHeight - topHalf); i--) {
      walls[i] |= 1;
    }
    createWall = 0;
    score++;
  } else {
    createWall++;
  }
}

void button_ISR() {
  start = true;
  fall = false;
  if (updateCounter < 255) {
    updateCounter = 255;
  }
}




