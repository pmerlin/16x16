#define LED_PIN D3
#define NUM_LEDS 256

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
      uint8_t reverseX = (kSquareWidth - 1) - x;
      i = (y * kSquareWidth) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * kSquareWidth) + x;
    }
    return i;
}
#endif

void setup() {
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(7);
  FastLED.clear();
  FastLED.show();
}

void loop() {
  uint8_t blurAmount = dim8_raw( beatsin8(3,64,192) );
  blur2d( leds, kSquareWidth, kSquareWidth, blurAmount);

  // Use three out-of-sync sine waves
  uint8_t  i = beatsin8(  91, kBorderWidth, kSquareWidth-kBorderWidth);
  uint8_t  j = beatsin8( 109, kBorderWidth, kSquareWidth-kBorderWidth);
  uint8_t  k = beatsin8(  73, kBorderWidth, kSquareWidth-kBorderWidth);
  
  // The color of each point shifts over time, each at a different speed.
  uint16_t ms = millis();  
  leds[XY( i, j)] += CHSV( ms / 29, 200, 255);
  leds[XY( j, k)] += CHSV( ms / 41, 200, 255);
  leds[XY( k, i)] += CHSV( ms / 73, 200, 255);
  
  FastLED.show();
  delay(10);
}
