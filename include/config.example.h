#pragma once

// Copy this file's values into config.h (already gitignored) and fill in
// your own credentials. config.h is not tracked by git.

#define WIFI_SSID "your-wifi-ssid"
#define WIFI_PASSWORD "your-wifi-password"

// NTP / timezone
#define NTP_SERVER "pool.ntp.org"
// POSIX TZ string, e.g. US Eastern with DST rules
#define TZ_INFO "EST5EDT,M3.2.0,M11.1.0"

// LED matrix
#define LED_PIN 2
#define LED_COUNT 120 // number of WS2812 pixels wired into the word grid
#define LED_BRIGHTNESS 80 // 0-255
