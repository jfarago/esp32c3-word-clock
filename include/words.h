#pragma once

#include <Arduino.h>
#include <FastLED.h>

// Size of the word text grid (row 0 = top row, col 0 = leftmost letter).
// Exposed so other modules (e.g. animations) can reason about pixel
// positions without duplicating the layout.
constexpr int WORD_GRID_ROWS = 8;
constexpr int WORD_GRID_COLS = 15;

// Translates a (row, col) position in the text grid to its index on the
// physical serpentine LED strip. See words.cpp for the wiring layout.
uint16_t wordGridStripIndex(int row, int col);

// Lights the LEDs that spell out the given time in words. On return,
// `phrase` holds the words that were lit (e.g. "IT IS TEN MINUTES PAST
// TWELVE"), for logging.
void renderTimeToWords(CRGB *leds, int hour24, int minute, String &phrase);

// Every word that can ever appear on the face (fixed phrase words plus the
// 12 hour words), for effects that want to light up "some word" without
// regard to the current time (e.g. the boot sparkle animation).
int wordRegionCount();
void wordRegionAt(int index, int &row, int &colStart, int &length);
