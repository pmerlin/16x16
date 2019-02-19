#include "FastLED.h"

// Lights that chase around the outside of a sign, as found
// at carnivals, circuses, and theatres.

#define NUM_LEDS      256
#define LED_TYPE   WS2811
#define COLOR_ORDER   GBR
#define DATA_PIN       6

// analog speed knob attached to analog input 2.
// undefine this to just used a fixed speed
#define KNOB_PIN      A2

#define VOLTS          5
#define MAX_MA       4000

// 3 is normal (1/3rd of lights lit), but 4 or 5 work.
#define CYCLE_LENGTH 3

CRGBArray<NUM_LEDS> leds;

void setup() {
  delay( 3000 ); //safety startup delay
//  FastLED.setMaxPowerInVoltsAndMilliamps( VOLTS, MAX_MA);
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(20);

#ifdef KNOB_PIN
  pinMode( KNOB_PIN, INPUT);
#endif
}

void loop() {
 
  uint8_t knob = 60;
#ifdef KNOB_PIN
  knob = analogRead( KNOB_PIN) / 4;
#endif
  knob = (knob & 0xF0) + 0x0F;
  uint16_t time_divider = knob * 4;

  // fade like the way incandescent bulbs fade
  fadeUsingColor( leds, NUM_LEDS, CRGB(200,185,150));

  uint8_t stepN = (millis() / time_divider) % CYCLE_LENGTH;
  uint8_t modN = 0;
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    uint8_t modN = addmod8( modN, 1, CYCLE_LENGTH);
    if( modN != stepN ) {
      // add some power to the bulbs that are lit right now
      leds[i] += CRGB( 80, 50, 20);
    }
  }
  
  FastLED.delay(10);
}
