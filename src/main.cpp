#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <FastLED.h>

#include "animations.h"
#include "colorize.h"
#include "config.h"
#include "settings.h"
#include "webui.h"
#include "words.h"

CRGB leds[LED_COUNT];
static CRGB currentFrame[LED_COUNT];

static Settings settings;

static void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.printf("Connecting to WiFi \"%s\"", WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConnected, IP: %s\n", WiFi.localIP().toString().c_str());
}

static void syncTime() {
  configTzTime(TZ_INFO, NTP_SERVER);

  struct tm timeinfo;
  Serial.print("Waiting for NTP time sync");
  while (!getLocalTime(&timeinfo)) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.printf("Time synced: %s", asctime(&timeinfo));
}

// Applies settings that take effect immediately (as opposed to ones only
// used on the next rendered frame, like the animation style).
static void applySettings() {
  FastLED.setBrightness(settings.brightness);
  FastLED.show();
}

// True while a transition animation is running. Guards against the web
// server (serviced as a pump callback from inside playTransition) kicking
// off a second, nested transition from a preview request that arrives
// mid-animation, which would corrupt the in-flight one.
static bool animationBusy = false;

static void runTransition(const CRGB *from, const CRGB *to, AnimationType type) {
  animationBusy = true;
  playTransition(leds, from, to, type, settings.animationSpeedTenths, webUiHandle);
  animationBusy = false;
}

// The clock always rounds display time down to the nearest 5-minute word,
// so it can lag up to 5 minutes behind the actual time. Rendering as if it
// were WORD_LEAD_SECONDS ahead caps that lag at WORD_LEAD_SECONDS instead.
static const time_t WORD_LEAD_SECONDS = 150; // 2 minutes 30 seconds

// Renders the phrase for (now + extraSeconds) into `outFrame`, colored per
// the current settings. `phraseOut`, if non-null, receives the phrase text.
static void renderFrameAtOffset(time_t extraSeconds, CRGB *outFrame, String *phraseOut = nullptr) {
  memset(outFrame, 0, sizeof(CRGB) * LED_COUNT);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;

  time_t displayTime = mktime(&timeinfo) + WORD_LEAD_SECONDS + extraSeconds;
  struct tm displayTm;
  localtime_r(&displayTime, &displayTm);

  String phrase;
  renderTimeToWords(outFrame, displayTm.tm_hour, displayTm.tm_min, phrase);
  colorizeFrame(outFrame, settings);
  if (phraseOut) *phraseOut = phrase;
}

// Requested from the web UI when the user picks an animation, so they can
// see what it looks like on the clock right away instead of waiting for the
// next real time change. Alternates between the real current phrase and the
// upcoming 5-minute phrase, so repeated preview clicks show the transition
// running both ways.
static bool previewShowingNext = false;

static void previewAnimation(AnimationType type) {
  if (animationBusy) return; // ignore requests that land mid-animation

  CRGB targetFrame[LED_COUNT];
  renderFrameAtOffset(previewShowingNext ? 0 : 300, targetFrame);
  previewShowingNext = !previewShowingNext;

  runTransition(currentFrame, targetFrame, type);
  memcpy(currentFrame, targetFrame, sizeof(currentFrame));
}

// Requested from the web UI when the user changes the color mode or solid
// color, so they can see it on the clock immediately. Just a straight
// recolor of what's already lit, not a full transition, since only the
// color (not which letters are lit) has changed.
static void previewColor() {
  if (animationBusy) return;
  colorizeFrame(currentFrame, settings);
  memcpy(leds, currentFrame, sizeof(currentFrame));
  FastLED.show();
}

void setup() {
  Serial.begin(115200);
  delay(200);

  settingsLoad(settings);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, LED_COUNT);
  FastLED.setBrightness(settings.brightness);
  FastLED.clear();
  FastLED.show();
  memset(currentFrame, 0, sizeof(currentFrame));

  connectWiFi();
  syncTime();
  webUiBegin(&settings, applySettings, previewAnimation, previewColor);
}

static int lastMinuteBlock = -1;
static int lastHour = -1;
static unsigned long lastCheckMs = 0;

void loop() {
  webUiHandle();

  unsigned long now = millis();
  if (now - lastCheckMs < 1000) {
    delay(5);
    return;
  }
  lastCheckMs = now;

  if (animationBusy) return; // a preview is mid-flight; check again next tick

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;

  time_t displayTime = mktime(&timeinfo) + WORD_LEAD_SECONDS;
  struct tm displayTm;
  localtime_r(&displayTime, &displayTm);

  int minuteBlock = (displayTm.tm_min / 5) * 5;
  if (minuteBlock == lastMinuteBlock && displayTm.tm_hour == lastHour) return;
  lastMinuteBlock = minuteBlock;
  lastHour = displayTm.tm_hour;

  CRGB nextFrame[LED_COUNT];
  String phrase;
  renderFrameAtOffset(0, nextFrame, &phrase);

  runTransition(currentFrame, nextFrame, settings.animation);
  memcpy(currentFrame, nextFrame, sizeof(currentFrame));

  Serial.printf("actual %02d:%02d:%02d -> clock reads \"%s\" (running %ldm%02lds fast, %s)\n",
                timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, phrase.c_str(),
                (long)(WORD_LEAD_SECONDS / 60), (long)(WORD_LEAD_SECONDS % 60),
                animationTypeName(settings.animation));
}
