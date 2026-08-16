// GENERATED FILE - do not edit directly.
// Regenerate with scripts/generate_arduino_sketch.sh after editing include/animations.h.

#pragma once

#include <Arduino.h>
#include <FastLED.h>

#include "settings.h"

// Renders the transition from the currently-displayed frame (`from`) to the
// next one (`to`) using the given animation style, writing intermediate
// states directly to `leds` and calling FastLED.show() as it goes. `leds`,
// `from`, and `to` must each point to LED_COUNT CRGB values.
//
// `speedTenths` scales how long the transition takes: 10 = normal speed,
// 20 = twice as fast, 5 = half speed. Has no effect on AnimationType::SNAP.
//
// `pump`, if non-null, is called after every FastLED.show() so callers can
// keep other periodic work (e.g. handling web requests) running during the
// animation, which can take the better part of a second.
void playTransition(CRGB *leds, const CRGB *from, const CRGB *to, AnimationType type,
                     uint8_t speedTenths, void (*pump)() = nullptr);
