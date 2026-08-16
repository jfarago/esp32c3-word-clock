// GENERATED FILE - do not edit directly.
// Regenerate with scripts/generate_arduino_sketch.sh after editing include/colorize.h.

#pragma once

#include <FastLED.h>

#include "settings.h"

// Recolors every lit (non-black) pixel in `frame` according to
// `settings.colorMode`, leaving unlit pixels untouched. `frame` must point
// to LED_COUNT CRGB values already produced by renderTimeToWords, i.e. with
// lit pixels marked in some non-black placeholder color. Safe to call
// repeatedly on an already-colorized frame (only lit/unlit state matters).
void colorizeFrame(CRGB *frame, const Settings &settings);
