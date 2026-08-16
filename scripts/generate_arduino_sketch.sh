#!/usr/bin/env bash
# Regenerates arduino/word_clock/ from the PlatformIO source of truth
# (src/, include/) so people without PlatformIO can flash the same code
# from the Arduino IDE. Run this after changing src/main.cpp, src/words.cpp,
# or include/words.h.
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
out_dir="$repo_root/arduino/word_clock"

mkdir -p "$out_dir"

generated_header() {
  local source_file="$1"
  echo "// GENERATED FILE - do not edit directly."
  echo "// Regenerate with scripts/generate_arduino_sketch.sh after editing $source_file."
  echo
}

{
  generated_header "src/main.cpp"
  cat "$repo_root/src/main.cpp"
} > "$out_dir/word_clock.ino"

{
  generated_header "src/words.cpp"
  cat "$repo_root/src/words.cpp"
} > "$out_dir/words.cpp"

{
  generated_header "include/words.h"
  cat "$repo_root/include/words.h"
} > "$out_dir/words.h"

# config.example.h is tracked and always kept current with the PlatformIO
# example. config.h holds real credentials (gitignored), edited by hand
# after generation, so only seed it the first time - never overwrite an
# existing one.
cp "$repo_root/include/config.example.h" "$out_dir/config.example.h"

if [ ! -f "$out_dir/config.h" ]; then
  cp "$out_dir/config.example.h" "$out_dir/config.h"
  echo "Created $out_dir/config.h from config.example.h - edit it with your WiFi/LED settings."
fi

echo "Regenerated arduino/word_clock/ from src/ and include/."
