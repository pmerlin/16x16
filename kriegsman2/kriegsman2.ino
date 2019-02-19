//#include <SmartMatrix.h>
#include <FastLED.h>

const uint8_t kMatrixWidth  = 16;
const uint8_t kMatrixHeight = 16;
const uint8_t kBorderWidth = 1;
#define LED_PIN 6
#define NUM_LEDS    (kMatrixWidth*kMatrixHeight)
CRGB leds[NUM_LEDS];

// Color correction for the SmartMatrix
#define COLOR_CORRECTION CRGB(255,110,178)

void setup() 
{
//  FastLED.addLeds<SMART_MATRIX>(leds, NUM_LEDS).setCorrection(COLOR_CORRECTION);
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(60);

}

void loop()
{
  // Apply some blurring to whatever's already on the matrix
  // Note that we never actually clear the matrix, we just constantly
  // blur it repeatedly.  Since the blurring is 'lossy', there's
  // an automatic trend toward black -- by design.
  uint8_t blurAmount = beatsin8(2,10,255);
  blur2d( leds, kMatrixWidth, kMatrixHeight, blurAmount);

  // Use two out-of-sync sine waves
  uint8_t  i = beatsin8( 27, kBorderWidth, kMatrixHeight-kBorderWidth);
  uint8_t  j = beatsin8( 41, kBorderWidth, kMatrixWidth-kBorderWidth);
  // Also calculate some reflections
  uint8_t ni = (kMatrixWidth-1)-i;
  uint8_t nj = (kMatrixWidth-1)-j;
  
  // The color of each point shifts over time, each at a different speed.
  uint16_t ms = millis();  
  leds[XY( i, j)] += CHSV( ms / 11, 200, 255);
  leds[XY( j, i)] += CHSV( ms / 13, 200, 255);
  leds[XY(ni,nj)] += CHSV( ms / 17, 200, 255);
  leds[XY(nj,ni)] += CHSV( ms / 29, 200, 255);
  leds[XY( i,nj)] += CHSV( ms / 37, 200, 255);
  leds[XY(ni, j)] += CHSV( ms / 41, 200, 255);
  
  FastLED.show();
}

// Trivial XY function for the SmartMatrix; use a different XY 
// function for different matrix grids.  See XYMatrix example for code.
//uint16_t XY( uint8_t x, uint8_t y) { return (y * kMatrixWidth) + x; }

// normal
#if 0
uint16_t XY(uint8_t x, uint8_t y) {
  return y * kSquareWidth + x;
}
#else
// alternate (zigzag)
uint16_t XY(uint8_t x, uint8_t y) {
  uint16_t i;
  if( y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * kMatrixWidth) + x;
    }
    return i;
}
#endif
