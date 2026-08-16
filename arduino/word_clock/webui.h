// GENERATED FILE - do not edit directly.
// Regenerate with scripts/generate_arduino_sketch.sh after editing include/webui.h.

#pragma once

#include "settings.h"

// Starts the on-device HTTP server (and mDNS as "wordclock.local") for the
// phone-facing settings UI. `settings` is read/written in place as the user
// changes settings; `onChange` is invoked after any successful update so the
// caller can apply the new values (e.g. FastLED.setBrightness) immediately.
// `onPreviewAnimation` is invoked (with the selected style) whenever the
// user picks or re-triggers an animation from the UI, so it can be played
// on the clock right away instead of waiting for the next time change.
// `onPreviewColor` is invoked whenever the color mode or solid color
// changes, so the new coloring shows up on the clock immediately.
// `onPreviewBootAnimation` is invoked (with the selected style) when the
// user picks or re-triggers a boot animation, so they can see it without
// rebooting.
void webUiBegin(Settings *settings, void (*onChange)(),
                 void (*onPreviewAnimation)(AnimationType type),
                 void (*onPreviewColor)(),
                 void (*onPreviewBootAnimation)(BootAnimation type));

// Services pending HTTP requests. Call this frequently from loop() (and from
// inside long-running animations) so the UI stays responsive.
void webUiHandle();
