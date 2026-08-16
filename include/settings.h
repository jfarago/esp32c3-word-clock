#pragma once

#include <Arduino.h>
#include <FastLED.h>

// How the display transitions from one rendered phrase to the next.
enum class AnimationType : uint8_t {
  SNAP = 0,      // instant, no transition
  CROSSFADE,     // old and new words fade across each other
  FADE,          // fade to black, then fade up the new words
  WIPE,          // sweep across the grid, reading order, revealing/clearing as it goes
  RIPPLE,        // change spreads outward from the center of the grid
  COUNT
};

const char *animationTypeName(AnimationType type);
AnimationType animationTypeFromName(const String &name);

// How lit letters are colored.
enum class ColorMode : uint8_t {
  SOLID = 0,      // a single user-chosen color
  RAINBOW_FACE,   // one rainbow gradient spanning the whole clock face
  RAINBOW_WORD,   // every lit word independently sweeps through a full rainbow
  RAINBOW_PHRASE, // one full rainbow spread across just the lit phrase
  COUNT
};

const char *colorModeName(ColorMode mode);
ColorMode colorModeFromName(const String &name);

// What plays on the LED grid while waiting on WiFi/NTP at boot, before
// there's a real time to display.
enum class BootAnimation : uint8_t {
  RAINBOW_MOVE = 0, // the whole face lit, a rainbow sweeping across it
  TWINKLE,          // random words fade in and out like stars in the sky
  COUNT
};

const char *bootAnimationName(BootAnimation anim);
BootAnimation bootAnimationFromName(const String &name);

// All persisted, user-adjustable settings. Add new fields here as more
// settings are introduced; settingsLoad()/settingsSave() and the web UI
// (webui.cpp) are the two other places that need to know about a new field.
struct Settings {
  AnimationType animation = AnimationType::CROSSFADE;
  uint8_t brightness = 10; // 0-255, overwritten by LED_BRIGHTNESS on first boot
  ColorMode colorMode = ColorMode::SOLID;
  CRGB solidColor = CRGB::White; // used when colorMode == SOLID
  // Transition speed: 10 = normal, 20 = 2x as fast, 5 = half speed. Kept in
  // tenths (rather than a float) so it stores cleanly in NVS.
  uint8_t animationSpeedTenths = 10;
  BootAnimation bootAnimation = BootAnimation::RAINBOW_MOVE;
};

// Loads persisted settings from flash (NVS) into `settings`, falling back to
// the struct's defaults for any value that's never been saved.
void settingsLoad(Settings &settings);

// Persists the current value of every field in `settings` to flash.
void settingsSave(const Settings &settings);
