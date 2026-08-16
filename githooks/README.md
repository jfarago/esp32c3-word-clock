# githooks

Tracked git hooks for this repo (`.git/hooks/` isn't versioned, so hooks
live here instead and are enabled per-clone).

## Enable

```sh
git config core.hooksPath githooks
```

## What's here

- `pre-commit` — runs `scripts/generate_arduino_sketch.sh` and stages any
  resulting changes to `arduino/word_clock/`, so that folder can't drift out
  of sync with `src/`/`include/`.
