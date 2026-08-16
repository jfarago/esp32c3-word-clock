// GENERATED FILE - do not edit directly.
// Regenerate with scripts/generate_arduino_sketch.sh after editing include/boot_animation.h.

#pragma once

#include <FastLED.h>

#include "settings.h"

// Resets internal boot-animation state. Call once, right before the
// WiFi/NTP wait loops start.
void bootAnimationBegin(BootAnimation type);

// Advances and displays one frame of the boot animation into `leds`
// (LED_COUNT CRGB values), if enough time has passed since the last frame.
// Frame-rate limited internally, so it's cheap to call on every pass of a
// fast polling loop (e.g. while waiting on WiFi/NTP).
void bootAnimationTick(CRGB *leds);
