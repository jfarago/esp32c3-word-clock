# word-clock

WS2812-based word clock running on a Seeed XIAO ESP32-C3, built with  
PlatformIO + Arduino framework.

## Face layout

![Word clock face layout](assets/clock-face.svg)

The clock reads "IT IS [minutes] [PAST/TO] [hour] O'CLOCK", e.g. "IT IS TEN  
MINUTES PAST TWELVE" or "IT IS QUARTER TO THREE". This diagram shows word  
placement, not physical wiring order — the LED strip is a single serpentine  
run that starts at the bottom-left row and snakes upward, alternating  
direction each row (see `stripIndex()` in `src/words.cpp`).

## Setup

Two ways to build and flash this, using the same source code either way:  
PlatformIO (recommended — this is what the repo is built around) or the  
Arduino IDE (simpler if you don't want to install PlatformIO).

### Option A: PlatformIO

1. Install [PlatformIO](https://platformio.org/install) (VS Code extension
  or CLI: `pip install platformio` / `brew install platformio`).
2. Copy `include/config.example.h` to `include/config.h` (already done, and
  gitignored) and fill in your WiFi credentials, timezone, and LED pin/count.
3. Build and flash:
  ```sh
   pio run -t upload
   pio device monitor
  ```

### Option B: Arduino IDE

`arduino/word_clock/` is a ready-to-open Arduino IDE copy of this same  
sketch, auto-generated from `src/`/`include/` (see  
[Keeping the Arduino sketch in sync](#keeping-the-arduino-sketch-in-sync)).

1. Install the [Arduino IDE](https://www.arduino.cc/en/software) (2.x).
2. Add ESP32 board support: File > Preferences > "Additional boards manager
  URLs" > add  
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`,  
   then Tools > Board > Boards Manager > install "esp32" (by Espressif  
   Systems).
3. Select the board: Tools > Board > esp32 > "XIAO_ESP32C3".
4. Install the FastLED library: Tools > Manage Libraries > search "FastLED"
  > install.
5. Copy `arduino/word_clock/config.example.h` to
  `arduino/word_clock/config.h` (gitignored) and fill in your WiFi  
   credentials, timezone, and LED pin/count.
6. Open `arduino/word_clock/word_clock.ino` in the Arduino IDE (the folder
  must stay named `word_clock` — Arduino requires the sketch folder and  
   main `.ino` file to share a name).
7. Plug in the board, select the right Port, then Upload.

### Keeping the Arduino sketch in sync

`arduino/word_clock/word_clock.ino`, `words.cpp`, `words.h`, and  
`config.example.h` are generated from `src/`/`include/` by  
`./scripts/generate_arduino_sketch.sh` — don't edit them directly. A  
pre-commit hook regenerates and stages them automatically; enable it once  
per clone with:

```sh
git config core.hooksPath githooks
```

## Web UI

The clock hosts a small settings page from the ESP32 itself — no app or
cloud service needed. Once it's connected to WiFi, visit
`http://wordclock.local/` (or the IP address printed over serial) from any
device on the same network, including your phone, to change the transition
animation, letter color, and brightness. Animation and color changes preview
on the clock immediately; everything is saved to flash so it also applies to
future time changes. More settings can be added over time as `Settings`
fields in `include/settings.h`, plumbed through `src/webui.cpp`.

## Project layout

- `src/main.cpp` — WiFi connect, NTP time sync, main render loop.
- `src/words.cpp` / `include/words.h` — maps a time to the set of LED pixels  
that spell it out, using each word's (row, column) position in the text  
grid, translated to the physical serpentine wiring index.
- `src/animations.cpp` / `include/animations.h` — renders the transition  
between the currently-displayed and next phrase (crossfade, fade, wipe,  
ripple, sparkle, pulse, or an instant snap).
- `src/settings.cpp` / `include/settings.h` — user-adjustable settings  
(animation style, color mode/color, brightness), persisted to flash (NVS).
- `src/colorize.cpp` / `include/colorize.h` — recolors a rendered frame's  
lit letters per the color mode: solid color, a rainbow across the whole  
face, or an independent rainbow per word.
- `src/webui.cpp` / `include/webui.h` — on-device HTTP server and HTML page  
for changing settings from a phone or browser.
- `include/config.h` — WiFi/NTP/LED settings (gitignored, not committed).

## Hardware

- Board: Seeed XIAO ESP32-C3
- LEDs: WS2812B addressable strip/matrix wired into the word grid
- Data pin, LED count, and brightness are set in `include/config.h`
