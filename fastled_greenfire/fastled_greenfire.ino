/* 
 * Copyright (C) 2013 Gilad Dayagi.  All rights reserved.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

 /*
  * An example for the Arduino particle system library
  * Creates a green flame effect.
  */

#include "ParticleSys.h"
#include "Particle_Std.h"
#include "Emitter_Fire.h"
#include "PartMatrix.h"

const byte numParticles = 50;

Particle_Std particles[numParticles];
Emitter_Fire emitter;
ParticleSys pSys(numParticles, particles, &emitter);
PartMatrix pMatrix;

#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERRUPT_RETRY_COUNT 1

#include <FastLED.h>

#define baud 115200

const uint8_t width = 16;
const uint8_t height = 16;

const uint16_t numleds = width * height;

// Wemos
#define LED_PIN 6

CRGB leds[numleds];

// normal
#if 1
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
      i = (y * width) + x;
    }
    return i;
}
#endif

void drawPixel(int16_t x, int16_t y, CRGB color) {
  if ((x < width) && (y < height))
    leds[XY(x, y)] = color;
}

/**
 * Render the particles into a low-resolution matrix
 */
void drawMatrix() {
  FastLED.clear();
    pMatrix.fade();
    pMatrix.render(particles, numParticles);
    //update the actual LED matrix
    for (uint8_t y = 0; y  < height; y++) {
        for(byte x = 0; x < width; x++) {
          drawPixel(x, y, CRGB(pMatrix.matrix[x][y].r, pMatrix.matrix[x][y].g, pMatrix.matrix[x][y].b));
        }
    }
    FastLED.show();
}

void setup() {
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, numleds).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(24);
  FastLED.clear();
  FastLED.show();
    randomSeed(analogRead(0));

    Particle_Std::ay = 1;
    ParticleSys::perCycle = 10;
    Emitter_Fire::baseHue = 75;  //green
//    Emitter_Fire::baseHue = 156; //blue
    Emitter_Fire::maxTtl = 96;
    Emitter_Fire::cycleHue = true;
    PartMatrix::isOverflow = false;

    //init all pixels to zero
    pMatrix.reset();
}

void loop()
{
    pSys.update();
    drawMatrix();
    delay(50);
}
