# word-clock

WS2812-based word clock running on a Seeed XIAO ESP32-C3, built with
PlatformIO + Arduino framework.

## Setup

1. Install [PlatformIO](https://platformio.org/install) (VS Code extension
   or CLI: `pip install platformio` / `brew install platformio`).
2. Copy `include/config.example.h` to `include/config.h` (already done, and
   gitignored) and fill in your WiFi credentials, timezone, and LED pin/count.
3. Build and flash:
   ```sh
   pio run -t upload
   pio device monitor
   ```

## Project layout

- `src/main.cpp` — WiFi connect, NTP time sync, main render loop.
- `src/words.cpp` / `include/words.h` — maps a time to the set of LED pixel
  ranges that spell it out. The pixel indices in `words.cpp` are placeholders
  — update them once the physical LED grid is wired and numbered.
- `include/config.h` — WiFi/NTP/LED settings (gitignored, not committed).

## Hardware

- Board: Seeed XIAO ESP32-C3
- LEDs: WS2812B addressable strip/matrix wired into the word grid
- Data pin, LED count, and brightness are set in `include/config.h`
