#include "settings.h"

#include <Preferences.h>

static const char *PREFS_NAMESPACE = "wordclock";
static const char *KEY_ANIMATION = "animation";
static const char *KEY_BRIGHTNESS = "brightness";
static const char *KEY_COLOR_MODE = "colorMode";
static const char *KEY_COLOR = "color";
static const char *KEY_SPEED = "speed";

const char *animationTypeName(AnimationType type) {
  switch (type) {
    case AnimationType::SNAP: return "SNAP";
    case AnimationType::CROSSFADE: return "CROSSFADE";
    case AnimationType::FADE: return "FADE";
    case AnimationType::WIPE: return "WIPE";
    case AnimationType::RIPPLE: return "RIPPLE";
    default: return "CROSSFADE";
  }
}

AnimationType animationTypeFromName(const String &name) {
  for (uint8_t i = 0; i < static_cast<uint8_t>(AnimationType::COUNT); i++) {
    AnimationType type = static_cast<AnimationType>(i);
    if (name.equalsIgnoreCase(animationTypeName(type))) return type;
  }
  return AnimationType::CROSSFADE;
}

const char *colorModeName(ColorMode mode) {
  switch (mode) {
    case ColorMode::SOLID: return "SOLID";
    case ColorMode::RAINBOW_FACE: return "RAINBOW_FACE";
    case ColorMode::RAINBOW_WORD: return "RAINBOW_WORD";
    case ColorMode::RAINBOW_PHRASE: return "RAINBOW_PHRASE";
    default: return "SOLID";
  }
}

ColorMode colorModeFromName(const String &name) {
  for (uint8_t i = 0; i < static_cast<uint8_t>(ColorMode::COUNT); i++) {
    ColorMode mode = static_cast<ColorMode>(i);
    if (name.equalsIgnoreCase(colorModeName(mode))) return mode;
  }
  return ColorMode::SOLID;
}

static uint32_t packColor(const CRGB &color) {
  return ((uint32_t)color.r << 16) | ((uint32_t)color.g << 8) | color.b;
}

void settingsLoad(Settings &settings) {
  Preferences prefs;
  prefs.begin(PREFS_NAMESPACE, /*readOnly=*/true);

  String animationName = prefs.getString(KEY_ANIMATION, animationTypeName(settings.animation));
  settings.animation = animationTypeFromName(animationName);

  settings.brightness = prefs.getUChar(KEY_BRIGHTNESS, settings.brightness);

  String colorModeStr = prefs.getString(KEY_COLOR_MODE, colorModeName(settings.colorMode));
  settings.colorMode = colorModeFromName(colorModeStr);

  uint32_t packed = prefs.getUInt(KEY_COLOR, packColor(settings.solidColor));
  settings.solidColor = CRGB(packed);

  settings.animationSpeedTenths = prefs.getUChar(KEY_SPEED, settings.animationSpeedTenths);

  prefs.end();
}

void settingsSave(const Settings &settings) {
  Preferences prefs;
  prefs.begin(PREFS_NAMESPACE, /*readOnly=*/false);
  prefs.putString(KEY_ANIMATION, animationTypeName(settings.animation));
  prefs.putUChar(KEY_BRIGHTNESS, settings.brightness);
  prefs.putString(KEY_COLOR_MODE, colorModeName(settings.colorMode));
  prefs.putUInt(KEY_COLOR, packColor(settings.solidColor));
  prefs.putUChar(KEY_SPEED, settings.animationSpeedTenths);
  prefs.end();
}
