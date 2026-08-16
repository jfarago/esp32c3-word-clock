#include "colorize.h"

#include "config.h"
#include "words.h"

// Sweeps a single hue smoothly across the whole grid, top-to-bottom then
// left-to-right within a row, so the rainbow spans the entire clock face
// regardless of word boundaries.
static CRGB rainbowFaceColorAt(int row, int col) {
  float t = (row * WORD_GRID_COLS + col) / float(WORD_GRID_ROWS * WORD_GRID_COLS - 1);
  return CHSV((uint8_t)(t * 255), 255, 255);
}

// Every Word occupies a contiguous run of columns within a single row (see
// words.cpp), so a contiguous run of lit columns in one row is exactly one
// lit word. Sweeping a full hue cycle across each run's letters gives every
// word its own independent rainbow.
static void colorizeRainbowWord(CRGB *frame) {
  for (int row = 0; row < WORD_GRID_ROWS; row++) {
    int col = 0;
    while (col < WORD_GRID_COLS) {
      uint16_t idx = wordGridStripIndex(row, col);
      if (frame[idx] == CRGB::Black) {
        col++;
        continue;
      }
      int startCol = col;
      while (col < WORD_GRID_COLS && frame[wordGridStripIndex(row, col)] != CRGB::Black) col++;
      int runLength = col - startCol;
      for (int k = 0; k < runLength; k++) {
        uint16_t pixelIdx = wordGridStripIndex(row, startCol + k);
        uint8_t hue = runLength > 1 ? (uint8_t)((k * 255) / (runLength - 1)) : 0;
        frame[pixelIdx] = CHSV(hue, 255, 255);
      }
    }
  }
}

// Spreads one full rainbow across just the lit letters, in reading order,
// regardless of how much (or little) of the face they occupy or how many
// words they span - unlike RAINBOW_FACE, which is stretched across the
// entire grid whether or not much of it is lit.
static void colorizeRainbowPhrase(CRGB *frame) {
  int litCount = 0;
  for (int i = 0; i < LED_COUNT; i++) {
    if (frame[i] != CRGB::Black) litCount++;
  }
  if (litCount == 0) return;

  int litIndex = 0;
  for (int row = 0; row < WORD_GRID_ROWS; row++) {
    for (int col = 0; col < WORD_GRID_COLS; col++) {
      uint16_t idx = wordGridStripIndex(row, col);
      if (frame[idx] == CRGB::Black) continue;
      uint8_t hue = litCount > 1 ? (uint8_t)((litIndex * 255) / (litCount - 1)) : 0;
      frame[idx] = CHSV(hue, 255, 255);
      litIndex++;
    }
  }
}

void colorizeFrame(CRGB *frame, const Settings &settings) {
  switch (settings.colorMode) {
    case ColorMode::RAINBOW_FACE:
      for (int row = 0; row < WORD_GRID_ROWS; row++) {
        for (int col = 0; col < WORD_GRID_COLS; col++) {
          uint16_t idx = wordGridStripIndex(row, col);
          if (frame[idx] == CRGB::Black) continue;
          frame[idx] = rainbowFaceColorAt(row, col);
        }
      }
      return;
    case ColorMode::RAINBOW_WORD:
      colorizeRainbowWord(frame);
      return;
    case ColorMode::RAINBOW_PHRASE:
      colorizeRainbowPhrase(frame);
      return;
    case ColorMode::SOLID:
    default:
      for (int i = 0; i < LED_COUNT; i++) {
        if (frame[i] != CRGB::Black) frame[i] = settings.solidColor;
      }
      return;
  }
}
