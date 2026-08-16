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

void loop() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    delay(1000);
    return;
  }

  FastLED.clear();
  renderTimeToWords(leds, timeinfo.tm_hour, timeinfo.tm_min);
  FastLED.show();

  delay(1000);
}
