//#include "SmartMatrix3.h"
#include "FastLED.h"

// FireworksXY
// Quick and dirty 2-D fireworks simulation using FastLED.
//
// Originaly designed an Adafruit 5x8 WS2811 shield, but works fine
// on other XY matricies.
//
// by Mark Kriegsman, July 2013
// (and not updated too much since then, so it's a little stale, 
//  but it's a good starting point, if rather uncommented.)
//
// adapted for SmartMatrix by Jason Coon, July 2015

#define PIXEL_WIDTH  16
#define PIXEL_HEIGHT 16

#define LED_TYPE     WS2811
#define COLOR_ORDER  GRB
#define DATA_PIN     6
//#define CLK_PIN    4

#define BRIGHTNESS   30


#define NUM_LEDS (PIXEL_WIDTH * PIXEL_HEIGHT)
CRGB leds[NUM_LEDS];

#define MODEL_BORDER 4
#define MODEL_WIDTH  (MODEL_BORDER + PIXEL_WIDTH  + MODEL_BORDER)
#define MODEL_HEIGHT (MODEL_BORDER + PIXEL_HEIGHT + MODEL_BORDER)

#define PIXEL_X_OFFSET ((MODEL_WIDTH  - PIXEL_WIDTH ) / 2)
#define PIXEL_Y_OFFSET ((MODEL_HEIGHT - PIXEL_HEIGHT) / 2)

#define WINDOW_X_MIN (PIXEL_X_OFFSET)
#define WINDOW_X_MAX (WINDOW_X_MIN + PIXEL_WIDTH - 1)
#define WINDOW_Y_MIN (PIXEL_Y_OFFSET)
#define WINDOW_Y_MAX (WINDOW_Y_MIN + PIXEL_HEIGHT - 1)

CRGB overrun;

CRGB& XY( byte x, byte y) 
{
  
  x -= PIXEL_X_OFFSET;
  y -= PIXEL_Y_OFFSET;

  if( y & 0x01) {
      x = (PIXEL_WIDTH - 1) - x;
    }
  
  if( x < PIXEL_WIDTH && y < PIXEL_HEIGHT) {
    return leds[ (((PIXEL_HEIGHT - 1) - y) * PIXEL_WIDTH) + x ] ;
  } else
    return overrun;
}

void screenscale( accum88 a, byte N, byte& screen, byte& screenerr)
{
  byte ia = a >> 8;
  screen = scale8( ia, N);
  byte m = screen * (256 / N);
  screenerr = (ia - m) * scale8(255,N);
  return;
}


void plot88( byte x, byte y, CRGB& color)
{
  byte ix = scale8( x, MODEL_WIDTH);
  byte iy = scale8( y, MODEL_HEIGHT);
  CRGB& px = XY( ix, iy);
  px = color;
}


static int16_t scale15by8_local( int16_t i, fract8 scale )
{
    int16_t result;
    result = (int32_t)((int32_t)i * scale) / 256;
    return result;
}

saccum78 gGravity = 10;
fract8  gBounce = 200;
fract8  gDrag = 255;
bool gSkyburst = 0;

accum88 gBurstx;
accum88 gBursty;
saccum78 gBurstxv;
saccum78 gBurstyv;
CRGB gBurstcolor;

#define NONE 0
#define SHELL 1
#define SPARK 2

class Dot {
public:
  byte    show;
  byte    theType;
  accum88 x;
  accum88 y;
  saccum78 xv;
  saccum78 yv;
  accum88 r;
  CRGB    color;

  Dot()
  {
    show = 0;
    theType = 0;
    x =  0;
    y =  0;
    xv = 0;
    yv = 0;
    r  = 0;
    color.setRGB( 0, 0, 0);
  }

  void Draw()
  {
    if( !show) return;
    byte ix, xe, xc;
    byte iy, ye, yc;
    screenscale( x, MODEL_WIDTH, ix, xe);
    screenscale( y, MODEL_HEIGHT, iy, ye);
    yc = 255 - ye;
    xc = 255 - xe;
    
    CRGB c00 = CRGB( dim8_video( scale8( scale8( color.r, yc), xc)), 
                     dim8_video( scale8( scale8( color.g, yc), xc)), 
                     dim8_video( scale8( scale8( color.b, yc), xc))
                     );
    CRGB c01 = CRGB( dim8_video( scale8( scale8( color.r, ye), xc)), 
                     dim8_video( scale8( scale8( color.g, ye), xc)), 
                     dim8_video( scale8( scale8( color.b, ye), xc))
                     );

    CRGB c10 = CRGB( dim8_video( scale8( scale8( color.r, yc), xe)), 
                     dim8_video( scale8( scale8( color.g, yc), xe)), 
                     dim8_video( scale8( scale8( color.b, yc), xe))
                     );
    CRGB c11 = CRGB( dim8_video( scale8( scale8( color.r, ye), xe)), 
                     dim8_video( scale8( scale8( color.g, ye), xe)), 
                     dim8_video( scale8( scale8( color.b, ye), xe))
                     );

    XY(ix, iy) += c00;
    XY(ix, iy + 1) += c01;
    XY(ix + 1, iy) += c10;
    XY(ix + 1, iy + 1) += c11;
  }
  
  void Move()
  {
    saccum78 oyv = yv;
    
    if( !show) return;
    yv -= gGravity;
    xv = scale15by8_local( xv, gDrag);    
    yv = scale15by8_local( yv, gDrag);

    if( theType == SPARK) {
      xv = scale15by8_local( xv, gDrag);    
      yv = scale15by8_local( yv, gDrag);
      color.nscale8( 255);
      if( !color) {
        show = 0;
      }
    }

    // if we'd hit the ground, bounce
    if( yv < 0 && (y < (-yv)) ) {
      if( theType == SPARK ) {
        show = 0;
      } else {
        yv = -yv;
        yv = scale15by8_local( yv, gBounce);
        if( yv < 500 ) {
          show = 0;
        }
      }
    }
    
    if( (yv < -300) /* && (!(oyv < 0))*/ ) {
      // pinnacle
      if( theType == SHELL ) {

        if( (y > (uint16_t)(0x8000)) /*&& (random8() < 64)*/) {
          // boom
          LEDS.showColor( CRGB::White);
          //delay( 1);
          LEDS.showColor( CRGB::Black);
        }

        show = 0;

        gSkyburst = 1;
        gBurstx = x;
        gBursty = y;
        gBurstxv = xv;
        gBurstyv = yv;
        gBurstcolor = color;        
      }
    }
    if( theType == SPARK) {
      if( ((xv >  0) && (x > xv)) ||
          ((xv < 0 ) && (x < (0xFFFF + xv))) )  {
        x += xv;
      } else {
        show = 0;
      }
    } else {
      x += xv;
    }
    y += yv;
    
  }
  
  void GroundLaunch()
  {
    yv = 600 + random16(300 + (25 * PIXEL_HEIGHT));
    if(yv > 1200) yv = 1200;
    xv = (int16_t)random16(600) - (int16_t)300;
    y = 0;
    x = 0x8000;
    hsv2rgb_rainbow( CHSV( random8(), 240, 200), color);
    show = 1;
  }
  
  void Skyburst( accum88 basex, accum88 basey, saccum78 basedv, CRGB& basecolor)
  {
    yv = (int16_t)0 + (int16_t)random16(1500) - (int16_t)500;
    xv = basedv + (int16_t)random16(2000) - (int16_t)1000;
    y = basey;
    x = basex;
    color = basecolor;
    color *= 4;
    theType = SPARK;
    show = 1;
  }

};

#define NUM_SPARKS 12

Dot gDot;
Dot gSparks[NUM_SPARKS];

void setup() {
  delay(2000);
  LEDS.setBrightness(BRIGHTNESS);
//  LEDS.addLeds<SMART_MATRIX>(leds, NUM_LEDS);
  LEDS.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
}

void loop() 
{ 
  random16_add_entropy( random() );
  CRGB sky1(0,0,2);
  CRGB sky2(2,0,2);

  memset8( leds, 0, NUM_LEDS * 3);

#if 0
   for( uint16_t v = 0; v < NUM_LEDS; v++) {
     leds[v] = sky1;
   }
   for( byte u = 0; u < 1; u++) {
    leds[random16(NUM_LEDS)] = sky2;
  }
#endif
  
  gDot.Move();
  gDot.Draw();
  for( byte b = 0; b < NUM_SPARKS; b++) {
    gSparks[b].Move();
    gSparks[b].Draw();
  }
  
  LEDS.show();
  static uint16_t launchcountdown = 0;
  if( gDot.show == 0 ) {
    if( launchcountdown == 0) {
      gDot.GroundLaunch();
      gDot.theType = SHELL;
      launchcountdown = random16( 350) + 1;
    } else {
      launchcountdown --;
    }
  }
  
  if( gSkyburst) {
    byte nsparks = random8( NUM_SPARKS / 2, NUM_SPARKS + 1);
    for( byte b = 0; b < nsparks; b++) {
      gSparks[b].Skyburst( gBurstx, gBursty, gBurstyv, gBurstcolor);
      gSkyburst = 0;
    }
  }

  delay(10);
}
