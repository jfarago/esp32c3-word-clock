#pragma once

#include <Arduino.h>
#include <FastLED.h>

// Lights the LEDs that spell out the given time in words. On return,
// `phrase` holds the words that were lit (e.g. "IT IS TEN MINUTES PAST
// TWELVE"), for logging.
void renderTimeToWords(CRGB *leds, int hour24, int minute, String &phrase);
