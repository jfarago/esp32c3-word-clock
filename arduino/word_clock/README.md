# word_clock (Arduino IDE version)

This is an Arduino IDE-compatible copy of the word clock sketch, for
people who don't want to install PlatformIO. Functionally identical to
the PlatformIO project in the repo root.

`word_clock.ino`, `words.cpp`, and `words.h` are generated from `src/` and
`include/` by `scripts/generate_arduino_sketch.sh` — don't edit them
directly, edit the PlatformIO source and re-run the script:

```sh
./scripts/generate_arduino_sketch.sh
```

`config.h` is the exception: it's gitignored and seeded once from
`config.example.h` (placeholder credentials), never overwritten, since it
holds your personal WiFi/LED settings.

A git pre-commit hook runs this script automatically and stages any updated
files, so this folder can't drift out of sync with `src/`/`include/`. See
`githooks/README.md` to enable it.

## Setup

1. Install the [Arduino IDE](https://www.arduino.cc/en/software) (2.x).
2. Add ESP32 board support: File > Preferences > "Additional boards manager
   URLs" > add:
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   Then Tools > Board > Boards Manager > install "esp32" (by Espressif Systems).
3. Select the board: Tools > Board > esp32 > "XIAO_ESP32C3".
4. Install the FastLED library: Tools > Manage Libraries > search "FastLED"
   > install.
5. Edit `config.h` in this folder: set your WiFi SSID/password, timezone,
   and LED pin/count/brightness for your build.
6. Open `word_clock.ino` in the Arduino IDE (this whole folder must stay
   named `word_clock` — Arduino requires the sketch folder and the main
   `.ino` file to share a name).
7. Plug in the board, select the right Port, then Upload.

## Files

- `word_clock.ino` — WiFi connect, NTP time sync, main render loop.
- `words.cpp` / `words.h` — maps a time to the set of LED pixels that spell
  it out.
- `config.example.h` — tracked placeholder, mirrors the PlatformIO project's
  `include/config.example.h`.
- `config.h` — WiFi/NTP/LED settings. Edit before flashing; gitignored, not
  the same file as the PlatformIO project's `include/config.h`.
