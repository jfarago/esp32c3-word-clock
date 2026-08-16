#!/usr/bin/env bash
# Regenerates arduino/word_clock/ from the PlatformIO source of truth
# (src/, include/) so people without PlatformIO can flash the same code
# from the Arduino IDE. Run this after adding/changing any file in src/ or
# include/.
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

# main.cpp becomes word_clock.ino (Arduino requires the sketch's main file
# to share the folder name); every other src/*.cpp is copied as-is.
{
  generated_header "src/main.cpp"
  cat "$repo_root/src/main.cpp"
} > "$out_dir/word_clock.ino"

for src_file in "$repo_root"/src/*.cpp; do
  name="$(basename "$src_file")"
  [ "$name" = "main.cpp" ] && continue
  {
    generated_header "src/$name"
    cat "$src_file"
  } > "$out_dir/$name"
done

for header_file in "$repo_root"/include/*.h; do
  name="$(basename "$header_file")"
  [ "$name" = "config.example.h" ] && continue
  {
    generated_header "include/$name"
    cat "$header_file"
  } > "$out_dir/$name"
done

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
