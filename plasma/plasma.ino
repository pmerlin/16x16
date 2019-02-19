// fastplas
//
//  By Andrew Tuline
//
// Date: Aug, 2014
//
// This demonstrates the cool capabilities of using basic trig functions with the FastLED library. Some of the routines are
// commented out, so experiment and have some fun.
//
// Here's some plasma/demo links
//
// - http://lodev.org/cgtutor/plasma.html
// - http://www.flipcode.com/archives/The_Art_of_Demomaking-Issue_04_Per_Pixel_Control.shtml
// - http://www.pouet.net/topic.php?which=4650
// - http://www.mennovanslooten.nl/blog/post/72
// - http://www.bidouille.org/prog/plasma  (see fastplasma)
// - https://github.com/johncarl81/neopixelplasma
// - http://codegolf.stackexchange.com/questions/35569/tweetable-mathematical-art (see fasttweetable)
//
// FastLED 2.1 is available at https://github.com/FastLED/FastLED/tree/FastLED2.1
//


#include <FastLED.h>                                           // FastLED library

#define LED_DT 6                                              // Data pin
#define NUM_LEDS 256                                            // Number of LED's
#define COLOR_ORDER GBR                                        // Change the order as necessary
#define LED_TYPE TypicalSMD5050                                        // What kind of strip are you using?
#define BRIGHTNESS  30                                       // How bright do we want to go

const uint8_t width = 16;
const uint8_t height = 16;


struct CRGB leds[NUM_LEDS];                                    // Initializxe our array


#define qsub(x, b)  ((x>b)?x-b:0)             // Unsigned subtraction macro. if result <0, then => 0
#define _sq(x) ((x)*(x))
#define pi 3.1415926535897932384626433832795


uint8_t wave1 = 0;
uint8_t wave2 = 0;
uint8_t wave3 = 0;
int thisdelay = 20;

// normal
#if 0
uint16_t XY(uint8_t x, uint8_t y) {
  return y * width + x;
}
#else
// alternate (zigzag)
uint16_t XY(uint8_t x, uint8_t y) {
  uint16_t i;
  if( y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (width - 1) - x;
      i = (y * width) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * (width)) + x;
    }
    return i;
}
#endif




void setup() {
  Serial.begin(9600);
  LEDS.addLeds<NEOPIXEL, LED_DT>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
//  LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
//  set_max_power_in_volts_and_milliamps( 5, 1000);
}


void loop () {
  plas3();
}


void plas1() {

  wave1 +=1;
  wave2 +=1;
  wave3 +=10;
  for (int j=0; j<height; j++)
  for (int k=0; k<width; k++) {

    leds[ XY(k,j) ].r = qsub(sin8(15*k + wave1), 160);                 // Another fixed frequency, variable phase sine wave with lowered level
    leds[ XY(k,j) ].g = qsub(cos8(30*k + wave2), 180);                 // A fixed frequency, variable phase sine wave with lowered level
    leds[ XY(k,j) ].b = 0;                                             // Another fixed frequency, variable phase sine wave with lowered level
    //    leds[XY(j,k)].g = qsub(quadwave8(k*k*2 + wave1), 160)*k/NUM_LEDS;   // A variable frequency, variable phase sine wave with lowered level and decaying amplitude
  }
  //show_at_max_brightness_for_power();
  //  FastLED.delay(thisdelay*2.5);
  //  delay_at_max_brightness_for_power(thisdelay*2.5);
  FastLED.show();
  delay(thisdelay);
}


void plas2() {
  wave1 +=2;
  wave2 +=2;
  wave3 +=2;
  for (int j=0; j<height; j++)
  for (int k=0; k<width; k++) {
    leds[XY(k,j)].r = 200;                                           // Another fixed frequency, variable phase sine wave with lowered level
    leds[XY(k,j)].g = qsub(cos8(30*k + wave2), 80);                  // A fixed frequency, variable phase sine wave with lowered level
    leds[XY(k,j)].b = qsub(sin8(30*k + wave3), 80);                  // A fixed frequency, variable phase sine wave with lowered level
  }
  FastLED.show();
  //show_at_max_brightness_for_power();
  //  FastLED.delay(thisdelay*2.5);
  //  delay_at_max_brightness_for_power(thisdelay*2.5);
  delay(thisdelay);
}


void plas3() {

  wave1 +=2;
  wave2 +=1;
  wave3 -=3;
  for (int j=0; j<height; j++)
  for (int k=0; k<width; k++) {
    leds[XY(k,j)].r = qsub(sin8(20*k + wave1), 80);                  // Another fixed frequency, variable phase sine wave with lowered level
    leds[XY(k,j)].g = qsub(cos8(25*k + wave2), 80);                  // A fixed frequency, variable phase sine wave with lowered level
    leds[XY(k,j)].b = qsub(sin8(22*k + wave3), 80);                  // A fixed frequency, variable phase sine wave with lowered level
  }
  //show_at_max_brightness_for_power();
  FastLED.show();
  //  FastLED.delay(thisdelay*2.5);
  //  delay_at_max_brightness_for_power(thisdelay*2.5);
  delay(thisdelay);
}

void plas4()
{
   uint16_t PlasmaTime=0, PlasmaShift;
   PlasmaShift = (random8(0, 5) * 32) + 64;
/*
  if (abs(millis() - LastLoop) >= LoopDelayMS)
  {
    LastLoop = millis();
    FastLED.clear();
    */

    // Fill background with dim plasma
    #define PLASMA_X_FACTOR  24
    #define PLASMA_Y_FACTOR  24

    while(1) {
    for (int16_t x=0; x<width; x++)
    {
      for (int16_t y=0; y<height; y++)
      {
        int16_t r = sin16(PlasmaTime) / 256;
        int16_t h = sin16(x * r * PLASMA_X_FACTOR + PlasmaTime) + cos16(y * (-r) * PLASMA_Y_FACTOR + PlasmaTime) + sin16(y * x * (cos16(-PlasmaTime) / 256) / 2);
//        leds[XY(x, y)] = CHSV((uint8_t)((h / 256) + 128), 255, 64);
        leds[XY(x, y)] = CHSV((uint8_t)((h / 256) + 128), 255, 200);

      }
    }
    uint16_t OldPlasmaTime = PlasmaTime;
    PlasmaTime += PlasmaShift;
    if (OldPlasmaTime > PlasmaTime)
      PlasmaShift = (random8(0, 5) * 32) + 64;
     FastLED.show();
     delay(20);
    }
}
