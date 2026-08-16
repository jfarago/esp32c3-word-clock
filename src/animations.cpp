#include "animations.h"

#include <math.h>

#include "config.h"
#include "words.h"

static void pump(void (*cb)()) {
  if (cb) cb();
}

// Scales a base per-step delay by the requested speed (10 = normal, 20 =
// twice as fast, 5 = half speed), clamped to a sane range so an extreme
// slider value can't freeze the animation or make it flash imperceptibly.
static uint16_t scaledDelay(uint16_t baseMs, uint8_t speedTenths) {
  if (speedTenths == 0) speedTenths = 1;
  uint32_t ms = (uint32_t)baseMs * 10 / speedTenths;
  if (ms < 1) ms = 1;
  if (ms > 500) ms = 500;
  return (uint16_t)ms;
}

// Pixel positions in the text grid, indexed by strip position, plus the
// strip's reading order (row-major, top-to-bottom / left-to-right in text
// grid space). Built once on first use.
static int pixelRow[LED_COUNT];
static int pixelCol[LED_COUNT];
static uint16_t readingOrder[LED_COUNT];
static bool layoutBuilt = false;

static void ensurePixelLayout() {
  if (layoutBuilt) return;
  int n = 0;
  for (int row = 0; row < WORD_GRID_ROWS; row++) {
    for (int col = 0; col < WORD_GRID_COLS; col++) {
      uint16_t idx = wordGridStripIndex(row, col);
      pixelRow[idx] = row;
      pixelCol[idx] = col;
      readingOrder[n++] = idx;
    }
  }
  layoutBuilt = true;
}

static void showFrame(CRGB *leds, const CRGB *frame) {
  memcpy(leds, frame, sizeof(CRGB) * LED_COUNT);
  FastLED.show();
}

static void snap(CRGB *leds, const CRGB *to) {
  showFrame(leds, to);
}

static void crossfade(CRGB *leds, const CRGB *from, const CRGB *to, uint8_t speedTenths,
                       void (*cb)()) {
  const int steps = 20;
  for (int s = 1; s <= steps; s++) {
    fract8 amt = (s * 255) / steps;
    for (int i = 0; i < LED_COUNT; i++) leds[i] = blend(from[i], to[i], amt);
    FastLED.show();
    pump(cb);
    delay(scaledDelay(20, speedTenths));
  }
}

static void fade(CRGB *leds, const CRGB *from, const CRGB *to, uint8_t speedTenths,
                  void (*cb)()) {
  const int steps = 12;
  for (int s = steps; s >= 0; s--) {
    fract8 amt = (s * 255) / steps;
    for (int i = 0; i < LED_COUNT; i++) leds[i] = blend(CRGB::Black, from[i], amt);
    FastLED.show();
    pump(cb);
    delay(scaledDelay(20, speedTenths));
  }
  for (int s = 0; s <= steps; s++) {
    fract8 amt = (s * 255) / steps;
    for (int i = 0; i < LED_COUNT; i++) leds[i] = blend(CRGB::Black, to[i], amt);
    FastLED.show();
    pump(cb);
    delay(scaledDelay(20, speedTenths));
  }
}

static void wipe(CRGB *leds, const CRGB *from, const CRGB *to, uint8_t speedTenths,
                  void (*cb)()) {
  ensurePixelLayout();
  memcpy(leds, from, sizeof(CRGB) * LED_COUNT);
  for (int n = 0; n < LED_COUNT; n++) {
    uint16_t idx = readingOrder[n];
    if (leds[idx] == to[idx]) continue;
    leds[idx] = to[idx];
    FastLED.show();
    pump(cb);
    delay(scaledDelay(12, speedTenths));
  }
  showFrame(leds, to);
}

static void ripple(CRGB *leds, const CRGB *from, const CRGB *to, uint8_t speedTenths,
                    void (*cb)()) {
  ensurePixelLayout();
  memcpy(leds, from, sizeof(CRGB) * LED_COUNT);

  const float centerRow = (WORD_GRID_ROWS - 1) / 2.0f;
  const float centerCol = (WORD_GRID_COLS - 1) / 2.0f;
  const int maxRing =
      (int)ceil(sqrt(centerRow * centerRow + centerCol * centerCol)) + 1;

  for (int ring = 0; ring <= maxRing; ring++) {
    bool changed = false;
    for (int i = 0; i < LED_COUNT; i++) {
      if (leds[i] == to[i]) continue;
      float dr = pixelRow[i] - centerRow;
      float dc = pixelCol[i] - centerCol;
      int d = (int)lround(sqrt(dr * dr + dc * dc));
      if (d != ring) continue;
      leds[i] = to[i];
      changed = true;
    }
    if (!changed) continue;
    FastLED.show();
    pump(cb);
    delay(scaledDelay(60, speedTenths));
  }
  showFrame(leds, to);
}

void playTransition(CRGB *leds, const CRGB *from, const CRGB *to, AnimationType type,
                     uint8_t speedTenths, void (*cb)()) {
  switch (type) {
    case AnimationType::CROSSFADE: crossfade(leds, from, to, speedTenths, cb); break;
    case AnimationType::FADE: fade(leds, from, to, speedTenths, cb); break;
    case AnimationType::WIPE: wipe(leds, from, to, speedTenths, cb); break;
    case AnimationType::RIPPLE: ripple(leds, from, to, speedTenths, cb); break;
    case AnimationType::SNAP:
    default:
      snap(leds, to);
      break;
  }
}
