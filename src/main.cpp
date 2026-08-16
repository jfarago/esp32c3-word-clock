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

  FastLED.clear();
  renderTimeToWords(leds, displayTm.tm_hour, displayTm.tm_min);
  FastLED.show();

  delay(1000);
}
