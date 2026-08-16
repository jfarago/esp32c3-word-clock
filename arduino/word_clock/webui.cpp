// GENERATED FILE - do not edit directly.
// Regenerate with scripts/generate_arduino_sketch.sh after editing src/webui.cpp.

#include "webui.h"

#include <ESPmDNS.h>
#include <WebServer.h>
#include <WiFi.h>

static const char *MDNS_HOSTNAME = "wordclock";

static WebServer server(80);
static Settings *g_settings = nullptr;
static void (*g_onChange)() = nullptr;
static void (*g_onPreviewAnimation)(AnimationType) = nullptr;
static void (*g_onPreviewColor)() = nullptr;
static void (*g_onPreviewBootAnimation)(BootAnimation) = nullptr;

static const char INDEX_HTML[] PROGMEM = R"HTML(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Word Clock</title>
<style>
  :root { color-scheme: light dark; }
  body {
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
    max-width: 420px;
    margin: 0 auto;
    padding: 24px 20px 60px;
    background: #111;
    color: #eee;
  }
  h1 { font-size: 1.3rem; margin-bottom: 4px; }
  p.sub { color: #999; margin-top: 0; font-size: 0.9rem; }
  label { display: block; margin-top: 28px; font-weight: 600; font-size: 0.95rem; }
  select, input[type=range] { width: 100%; margin-top: 10px; }
  select {
    padding: 10px;
    font-size: 1rem;
    border-radius: 8px;
    border: 1px solid #444;
    background: #1c1c1c;
    color: #eee;
  }
  .row { display: flex; justify-content: space-between; align-items: baseline; }
  .value { color: #999; font-variant-numeric: tabular-nums; }
  input[type=color] {
    width: 100%;
    margin-top: 10px;
    height: 44px;
    padding: 4px;
    border-radius: 8px;
    border: 1px solid #444;
    background: #1c1c1c;
  }
  button {
    width: 100%;
    margin-top: 10px;
    padding: 10px;
    font-size: 0.95rem;
    border-radius: 8px;
    border: 1px solid #444;
    background: #263a26;
    color: #eee;
  }
  button:active { background: #32502f; }
  #status { margin-top: 24px; font-size: 0.85rem; color: #6c6; min-height: 1.2em; }
</style>
</head>
<body>
  <h1>Word Clock</h1>
  <p class="sub">Brightness applies immediately. Animation, speed, and color
  choices preview on the clock right away and are used for the next real
  time change too.</p>

  <label for="animation">Transition animation</label>
  <select id="animation">
    <option value="SNAP">Instant (no animation)</option>
    <option value="CROSSFADE">Crossfade</option>
    <option value="FADE">Fade out / fade in</option>
    <option value="WIPE">Wipe</option>
    <option value="RIPPLE">Ripple</option>
  </select>
  <button type="button" id="previewBtn">&#9654; Preview on clock</button>

  <label>
    <div class="row"><span>Speed</span><span class="value" id="speedValue"></span></div>
  </label>
  <input type="range" id="speed" min="3" max="40" step="1">

  <label for="colorMode">Color</label>
  <select id="colorMode">
    <option value="SOLID">Solid color</option>
    <option value="RAINBOW_FACE">Rainbow (whole face)</option>
    <option value="RAINBOW_WORD">Rainbow (each word)</option>
    <option value="RAINBOW_PHRASE">Rainbow (across phrase)</option>
  </select>
  <input type="color" id="solidColor" value="#ffffff">

  <label>
    <div class="row"><span>Brightness</span><span class="value" id="brightnessValue"></span></div>
  </label>
  <input type="range" id="brightness" min="1" max="255" step="1">

  <label for="bootAnimation">Boot animation</label>
  <p class="sub" style="margin-top:0">Plays while connecting to WiFi and
  syncing the time, before the clock knows what to display.</p>
  <select id="bootAnimation">
    <option value="RAINBOW_MOVE">Moving rainbow</option>
    <option value="TWINKLE">Twinkle</option>
  </select>
  <button type="button" id="bootPreviewBtn">&#9654; Preview on clock (3s)</button>

  <div id="status"></div>

<script>
const animationEl = document.getElementById('animation');
const previewBtn = document.getElementById('previewBtn');
const speedEl = document.getElementById('speed');
const speedValueEl = document.getElementById('speedValue');
const colorModeEl = document.getElementById('colorMode');
const solidColorEl = document.getElementById('solidColor');
const brightnessEl = document.getElementById('brightness');
const brightnessValueEl = document.getElementById('brightnessValue');
const bootAnimationEl = document.getElementById('bootAnimation');
const bootPreviewBtn = document.getElementById('bootPreviewBtn');
const statusEl = document.getElementById('status');

function showStatus(msg) {
  statusEl.textContent = msg;
  clearTimeout(showStatus._t);
  showStatus._t = setTimeout(() => statusEl.textContent = '', 1500);
}

function updateColorPickerVisibility() {
  solidColorEl.style.display = colorModeEl.value === 'SOLID' ? '' : 'none';
}

function formatSpeed(speedTenths) {
  return (speedTenths / 10).toFixed(1) + 'x';
}

function loadSettings() {
  fetch('/api/settings').then(r => r.json()).then(s => {
    animationEl.value = s.animation;
    speedEl.value = s.speed;
    speedValueEl.textContent = formatSpeed(s.speed);
    colorModeEl.value = s.colorMode;
    solidColorEl.value = s.color;
    updateColorPickerVisibility();
    brightnessEl.value = s.brightness;
    brightnessValueEl.textContent = s.brightness;
    bootAnimationEl.value = s.bootAnimation;
  });
}

// Posting "animation" plays that transition on the physical clock; posting
// "colorMode"/"color" recolors it immediately. Either way the value is also
// saved as the setting used going forward.
function postSettings(body, statusMsg) {
  fetch('/api/settings', {
    method: 'POST',
    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
    body
  }).then(r => {
    if (r.ok) showStatus(statusMsg);
  });
}

function previewSelectedAnimation() {
  postSettings('animation=' + encodeURIComponent(animationEl.value), 'Previewing…');
}

animationEl.addEventListener('change', previewSelectedAnimation);
previewBtn.addEventListener('click', previewSelectedAnimation);

speedEl.addEventListener('input', () => {
  speedValueEl.textContent = formatSpeed(speedEl.value);
});
speedEl.addEventListener('change', () => {
  postSettings('speed=' + encodeURIComponent(speedEl.value), 'Previewing…');
});

colorModeEl.addEventListener('change', () => {
  updateColorPickerVisibility();
  postSettings('colorMode=' + encodeURIComponent(colorModeEl.value), 'Previewing…');
});
solidColorEl.addEventListener('change', () => {
  postSettings('color=' + encodeURIComponent(solidColorEl.value), 'Previewing…');
});

brightnessEl.addEventListener('input', () => {
  brightnessValueEl.textContent = brightnessEl.value;
});
brightnessEl.addEventListener('change', () => {
  postSettings('brightness=' + encodeURIComponent(brightnessEl.value), 'Saved');
});

function previewSelectedBootAnimation() {
  postSettings('bootAnimation=' + encodeURIComponent(bootAnimationEl.value), 'Previewing…');
}
bootAnimationEl.addEventListener('change', previewSelectedBootAnimation);
bootPreviewBtn.addEventListener('click', previewSelectedBootAnimation);

loadSettings();
</script>
</body>
</html>
)HTML";

static void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

// Parses a "#rrggbb" (or "rrggbb") string; returns `fallback` if malformed.
static CRGB parseHexColor(const String &hex, CRGB fallback) {
  String s = hex.startsWith("#") ? hex.substring(1) : hex;
  if (s.length() != 6) return fallback;
  char buf[7];
  s.toCharArray(buf, sizeof(buf));
  char *endPtr = nullptr;
  unsigned long value = strtoul(buf, &endPtr, 16);
  if (endPtr != buf + 6) return fallback;
  return CRGB((uint32_t)value);
}

static void appendHexColor(String &json, const CRGB &color) {
  char buf[8];
  snprintf(buf, sizeof(buf), "#%02x%02x%02x", color.r, color.g, color.b);
  json += buf;
}

static void handleGetSettings() {
  String json = "{\"animation\":\"";
  json += animationTypeName(g_settings->animation);
  json += "\",\"speed\":";
  json += String(g_settings->animationSpeedTenths);
  json += ",\"brightness\":";
  json += String(g_settings->brightness);
  json += ",\"colorMode\":\"";
  json += colorModeName(g_settings->colorMode);
  json += "\",\"color\":\"";
  appendHexColor(json, g_settings->solidColor);
  json += "\",\"bootAnimation\":\"";
  json += bootAnimationName(g_settings->bootAnimation);
  json += "\"}";
  server.send(200, "application/json", json);
}

static void handlePostSettings() {
  bool changed = false;
  bool previewAnimation = false;
  bool previewColor = false;
  bool previewBootAnimation = false;

  if (server.hasArg("animation")) {
    g_settings->animation = animationTypeFromName(server.arg("animation"));
    changed = true;
    previewAnimation = true; // re-preview even if the value didn't change
  }
  if (server.hasArg("speed")) {
    long s = server.arg("speed").toInt();
    if (s < 3) s = 3;
    if (s > 40) s = 40;
    g_settings->animationSpeedTenths = (uint8_t)s;
    changed = true;
    previewAnimation = true; // re-run the current animation at the new speed
  }
  if (server.hasArg("brightness")) {
    long b = server.arg("brightness").toInt();
    if (b < 1) b = 1;
    if (b > 255) b = 255;
    g_settings->brightness = (uint8_t)b;
    changed = true;
  }
  if (server.hasArg("colorMode")) {
    g_settings->colorMode = colorModeFromName(server.arg("colorMode"));
    changed = true;
    previewColor = true;
  }
  if (server.hasArg("color")) {
    g_settings->solidColor = parseHexColor(server.arg("color"), g_settings->solidColor);
    changed = true;
    previewColor = true;
  }
  if (server.hasArg("bootAnimation")) {
    g_settings->bootAnimation = bootAnimationFromName(server.arg("bootAnimation"));
    changed = true;
    previewBootAnimation = true; // re-preview even if the value didn't change
  }

  if (changed) {
    settingsSave(*g_settings);
    if (g_onChange) g_onChange();
  }

  // handleGetSettings()'s response goes out before the (possibly ~1s)
  // animation runs, so the UI doesn't hang waiting for it.
  handleGetSettings();

  if (previewAnimation && g_onPreviewAnimation) {
    g_onPreviewAnimation(g_settings->animation);
  } else if (previewColor && g_onPreviewColor) {
    g_onPreviewColor();
  } else if (previewBootAnimation && g_onPreviewBootAnimation) {
    g_onPreviewBootAnimation(g_settings->bootAnimation);
  }
}

void webUiBegin(Settings *settings, void (*onChange)(),
                void (*onPreviewAnimation)(AnimationType type),
                void (*onPreviewColor)(),
                void (*onPreviewBootAnimation)(BootAnimation type)) {
  g_settings = settings;
  g_onChange = onChange;
  g_onPreviewAnimation = onPreviewAnimation;
  g_onPreviewColor = onPreviewColor;
  g_onPreviewBootAnimation = onPreviewBootAnimation;

  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/settings", HTTP_GET, handleGetSettings);
  server.on("/api/settings", HTTP_POST, handlePostSettings);
  server.begin();

  if (MDNS.begin(MDNS_HOSTNAME)) {
    MDNS.addService("http", "tcp", 80);
    Serial.printf("Web UI: http://%s.local/\n", MDNS_HOSTNAME);
  } else {
    Serial.println("mDNS start failed; use the device's IP address instead.");
  }
  Serial.printf("Web UI: http://%s/\n", WiFi.localIP().toString().c_str());
}

void webUiHandle() {
  server.handleClient();
}
