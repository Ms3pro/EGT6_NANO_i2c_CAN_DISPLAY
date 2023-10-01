#include <Wire.h>
#include <TaskScheduler.h>
#include <iarduino_I2C_connect.h>
#include "Adafruit_ST7735.h"
#include "display.h"

#if DISP1_ACTIVE && defined DISP1_USE_ST7735_SPI
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
#endif

Scheduler ts;

iarduino_I2C_connect I2C2;

int adc0, adc1, adc2, adc3, adc4, adc5;

const int EGT_LEDS[] = {EGT1_LED, EGT2_LED, EGT3_LED, EGT4_LED, EGT5_LED, EGT6_LED};
const unsigned long interval = 500;
unsigned long previousMillis = 0;
int ledIndex = 0;
bool turningOn = true;

bool i2cCommunicationStatus = true; // Initialize the I2C communication status as OK

void egtreadTask() {
  
  adc0 = I2C2.readByte(0x02, 0) << 8;
  adc0 += I2C2.readByte(0x02, 1);
  adc1 = I2C2.readByte(0x02, 2) << 8;
  adc1 += I2C2.readByte(0x02, 3);
  adc2 = I2C2.readByte(0x02, 4) << 8;
  adc2 += I2C2.readByte(0x02, 5);
  adc3 = I2C2.readByte(0x02, 6) << 8;
  adc3 += I2C2.readByte(0x02, 7);
  adc4 = I2C2.readByte(0x02, 8) << 8;
  adc4 += I2C2.readByte(0x02, 9);
  adc5 = I2C2.readByte(0x02, 10) << 8;
  adc5 += I2C2.readByte(0x02, 11);

}

void driveDisplay();

void driveDisplayTask() {
 
  driveDisplay();

}

void EGTBarTask() {
 
 EGTBar();

}

void checkI2CStatusTask() {
 
  // Check I2C communication status
  checkI2CStatus();

}

void checkI2CStatus() {
  
  // Attempt to read from an I2C device
  int i2cDeviceAddress = 0x02; // Replace with the address of the I2C device you want to check
  Wire.beginTransmission(i2cDeviceAddress);
  if (Wire.endTransmission() != 0) {
    // I2C communication failed, set the status to false
    i2cCommunicationStatus = false;
  } else {
    // I2C communication successful, set the status to true
    i2cCommunicationStatus = true;
  }

  // Update the warning LED based on the I2C status
  if (!i2cCommunicationStatus) {
    // If I2C communication is interrupted, blink the warning LED
    blinkWarningLED();
  } else {
    // If I2C communication is restored, turn off the warning LED
    digitalWrite(RED_WARNLED, LOW);
  }
}

void blinkWarningLED() {
  
  static unsigned long previousMillis = 0;
  static bool ledState = LOW;
  const unsigned long blinkInterval = 100; // Blink interval in milliseconds

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= blinkInterval) {
    previousMillis = currentMillis;

    // Toggle the LED state
    ledState = !ledState;
    digitalWrite(RED_WARNLED, ledState);
  
  }
}

Task t1(10, TASK_FOREVER, &egtreadTask, &ts, true);
Task t2(10, TASK_FOREVER, &driveDisplayTask, &ts, true);
Task t3(10, TASK_FOREVER, &EGTBarTask, &ts, true);
Task t4(10, TASK_FOREVER, &checkI2CStatusTask, &ts, true);


void setup() {
 
  Wire.begin();

  pinMode(RED_WARNLED, OUTPUT);
  pinMode(TFT_LED, OUTPUT);
  pinMode(EGT1_LED, OUTPUT);
  pinMode(EGT2_LED, OUTPUT);
  pinMode(EGT3_LED, OUTPUT);
  pinMode(EGT4_LED, OUTPUT);
  pinMode(EGT5_LED, OUTPUT);
  pinMode(EGT6_LED, OUTPUT);

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);

digitalWrite(RED_WARNLED, HIGH);

  while (ledIndex < 6) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      if (turningOn) {
        digitalWrite(EGT_LEDS[ledIndex], HIGH);
        ledIndex++;
        if (ledIndex == 6) {
          turningOn = false;
          ledIndex--;
        }
      } else {
        digitalWrite(EGT_LEDS[ledIndex], LOW);
        ledIndex--;
        if (ledIndex < 0) {
          break;
        }
      }
    }
  }
digitalWrite(RED_WARNLED, LOW);  
  initialise_display();
}

void loop() {
  
  ts.execute();

}
