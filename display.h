#ifndef DISPLAY_H
#define DISPLAY_H

#include "BitmapHelpers.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
//display module options .  
//setup display 1
#define DISP1_ACTIVE     1             // 0 == DISABLED , 1 == ENABLED

//only choose one of the following defines for display type selection  , comment out the unused ones
#define DISP1_USE_ST7735_SPI

//set pins for screens in spi mode
//display 1 
#define RED_WARNLED  11  //Если произашел обрыв связи i2c ЗАЖИГАЕМ красный диод 
#define TFT_LED       9  // Пин для управления подсветкой дисплея
#define TFT_CS        8  //8
#define TFT_DC       10  //10
#define TFT_RST      PB6 //RESET
#define TFT_MOSI     12  //12 
#define TFT_SCLK     13  //13

#define EGT1_LED 2  
#define EGT2_LED 3  
#define EGT3_LED 4  
#define EGT4_LED 5  
#define EGT5_LED 6 
#define EGT6_LED 7 

#define BLUE          0x001F
#define GREEN         0x07E0
#define RED           0xF800
#define YELLOW        0xFFE0
#define ORANGE        0xFD20
#define WHITE         0xFFFF
#define BLACK         0x0000

extern Adafruit_ST7735 tft;

void initialise_display();

void EGTBar();
double rawEgt1,rawEgt2,rawEgt3,rawEgt4,rawEgt5,rawEgt6;
double egt1, egt2, egt3, egt4, egt5, egt6;
bool barBlinking[6] = {false, false, false, false, false, false};
unsigned long lastBlinkTime[6] = {0, 0, 0, 0, 0, 0};

extern int adc0, adc1, adc2, adc3, adc4, adc5;

boolean graph_1 = true;

#endif
