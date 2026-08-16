#include "boot_animation.h"

#include <math.h>

#include "config.h"
#include "words.h"

static const unsigned long FRAME_INTERVAL_MS = 40;

static BootAnimation g_type = BootAnimation::RAINBOW_MOVE;
static unsigned long g_lastFrameMs = 0;
static uint8_t g_hueOffset = 0;

struct Star {
  bool active;
  int wordIndex;
  unsigned long startMs;
  unsigned long durationMs;
  uint8_t peakBrightness; // varies per star so not all twinkles are equally bright
};
static const int MAX_STARS = 15;
static Star g_stars[MAX_STARS];

void bootAnimationBegin(BootAnimation type) {
  g_type = type;
  g_lastFrameMs = 0;
  g_hueOffset = 0;
  for (int i = 0; i < MAX_STARS; i++) g_stars[i].active = false;
}

// How many full rainbow spectrums are packed across the face at once. 1
// would stretch a single red-to-violet sweep across the whole grid (barely
// a few colors visible at a glance); higher values repeat it, so more of
// the spectrum is visible on screen at once.
static const int RAINBOW_REPEATS = 10;

// Fills the whole face (not just lit words - there's no phrase to show yet)
// with a repeating rainbow that sweeps across it, moving a little more each
// frame.
static void tickRainbowMove(CRGB *leds) {
  for (int row = 0; row < WORD_GRID_ROWS; row++) {
    for (int col = 0; col < WORD_GRID_COLS; col++) {
      uint16_t idx = wordGridStripIndex(row, col);
      uint8_t hue = (uint8_t)(((row * WORD_GRID_COLS + col) * 255 * RAINBOW_REPEATS) /
                                   (WORD_GRID_ROWS * WORD_GRID_COLS) +
                               g_hueOffset);
      leds[idx] = CHSV(hue, 255, 255);
    }
  }
  FastLED.show();
  g_hueOffset += 5;
}

// Randomly picks whole words (regardless of what time it is) and twinkles
// each one in and out on its own timer, several at once - like stars
// appearing in a night sky, not a strict on/off blink.
static void tickTwinkle(CRGB *leds) {
  unsigned long now = millis();
  fill_solid(leds, LED_COUNT, CRGB::Black);

  for (int i = 0; i < MAX_STARS; i++) {
    Star &s = g_stars[i];
    if (!s.active && random8() < 8) {
      s.active = true;
      s.wordIndex = random16(wordRegionCount());
      s.startMs = now;
      s.durationMs = 600 + random16(900);       // 0.6-1.5s per twinkle
      s.peakBrightness = 90 + random8(166);      // varied intensity, 90-255
    }
    if (!s.active) continue;

    unsigned long elapsed = now - s.startMs;
    if (elapsed >= s.durationMs) {
      s.active = false;
      continue;
    }

    // A sine bell curve eases in and out smoothly (unlike a linear ramp),
    // which reads as a soft twinkle rather than a mechanical fade.
    float t = (float)elapsed / (float)s.durationMs;
    float envelope = sinf((float)M_PI * t);
    uint8_t brightness = (uint8_t)(envelope * s.peakBrightness);

    int row, colStart, length;
    wordRegionAt(s.wordIndex, row, colStart, length);
    for (int k = 0; k < length; k++) {
      uint16_t idx = wordGridStripIndex(row, colStart + k);
      leds[idx] = CRGB(brightness, brightness, brightness);
    }
  }

  FastLED.show();
}

void bootAnimationTick(CRGB *leds) {
  unsigned long now = millis();
  if (now - g_lastFrameMs < FRAME_INTERVAL_MS) return;
  g_lastFrameMs = now;

  switch (g_type) {
    case BootAnimation::TWINKLE: tickTwinkle(leds); break;
    case BootAnimation::RAINBOW_MOVE:
    default: tickRainbowMove(leds); break;
  }
}
