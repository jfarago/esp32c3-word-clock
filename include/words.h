#pragma once

#include <FastLED.h>

// Lights the LEDs that spell out the given time in words.
//
// The pixel indices below are placeholders — update WORD_* to match your
// physical wiring once the LED grid/word layout is built and numbered.
void renderTimeToWords(CRGB *leds, int hour24, int minute);
