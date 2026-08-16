// GENERATED FILE - do not edit directly.
// Regenerate with scripts/generate_arduino_sketch.sh after editing src/main.cpp.

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <FastLED.h>

#include "config.h"
#include "words.h"

CRGB leds[LED_COUNT];

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

void setup() {
  Serial.begin(115200);
  delay(200);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, LED_COUNT);
  FastLED.setBrightness(LED_BRIGHTNESS);
  FastLED.clear();
  FastLED.show();

  connectWiFi();
  syncTime();
}

// The clock always rounds display time down to the nearest 5-minute word,
// so it can lag up to 5 minutes behind the actual time. Rendering as if it
// were WORD_LEAD_SECONDS ahead caps that lag at WORD_LEAD_SECONDS instead.
static const time_t WORD_LEAD_SECONDS = 150; // 2 minutes 30 seconds

void loop() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    delay(1000);
    return;
  }

  time_t displayTime = mktime(&timeinfo) + WORD_LEAD_SECONDS;
  struct tm displayTm;
  localtime_r(&displayTime, &displayTm);

  String phrase;
  FastLED.clear();
  renderTimeToWords(leds, displayTm.tm_hour, displayTm.tm_min, phrase);
  FastLED.show();

  Serial.printf("actual %02d:%02d:%02d -> clock reads \"%s\" (running %ldm%02lds fast)\n",
                timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, phrase.c_str(),
                (long)(WORD_LEAD_SECONDS / 60), (long)(WORD_LEAD_SECONDS % 60));

  delay(1000);
}
