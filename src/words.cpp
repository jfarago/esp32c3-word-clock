#include "words.h"
#include "config.h"

// The strip is wired as a serpentine (boustrophedon) matrix: it starts at
// the bottom-left of the clock and snakes upward, alternating direction
// every row. Words below are specified by their position in the *text*
// grid (row 0 = top row, col 0 = leftmost letter in that row), which is
// how the physical layout was measured, and translated to the actual
// wiring index by stripIndex().
static const int ROW_WIDTH = 15;
static const int NUM_ROWS = 8;

struct Word {
  uint8_t row;       // 0 = top row of the text grid
  uint8_t colStart;  // 0 = leftmost letter in the row
  uint8_t length;
  const char *name;
};

static const CRGB WORD_COLOR = CRGB::White;

// Row 0 (bottom-most physically, first on the strip) runs left-to-right;
// direction alternates for each row above it.
static uint16_t stripIndex(int row, int col) {
  int physicalRowFromBottom = NUM_ROWS - 1 - row;
  int rowStart = physicalRowFromBottom * ROW_WIDTH;
  bool leftToRight = (physicalRowFromBottom % 2) == 0;
  int withinRow = leftToRight ? col : (ROW_WIDTH - 1 - col);
  return rowStart + withinRow;
}

static void lightWord(CRGB *leds, const Word &w, String &phrase) {
  for (uint8_t i = 0; i < w.length; i++) {
    leds[stripIndex(w.row, w.colStart + i)] = WORD_COLOR;
  }
  if (phrase.length() > 0) phrase += ' ';
  phrase += w.name;
}

static const Word W_IT_IS = {0, 0, 4, "IT IS"};
static const Word W_HALF = {0, 4, 7, "HALF"};
static const Word W_TEN_MIN = {0, 11, 4, "TEN"};
static const Word W_QUARTER = {1, 0, 8, "QUARTER"};
static const Word W_TWENTY = {1, 8, 7, "TWENTY"};
static const Word W_FIVE_MIN = {2, 0, 4, "FIVE"};
static const Word W_MINUTES = {2, 4, 8, "MINUTES"};
static const Word W_TO = {2, 12, 3, "TO"};
static const Word W_PAST = {3, 0, 4, "PAST"};
static const Word W_OCLOCK = {7, 7, 8, "O'CLOCK"};

// Indexed by hour value (0 = twelve, 1 = one, ... 11 = eleven), not by the
// words' physical/wiring order on the strip.
static const Word HOUR_WORDS[12] = {
    {7, 0, 7, "TWELVE"},
    {3, 4, 6, "ONE"},
    {4, 0, 4, "TWO"},
    {3, 10, 5, "THREE"},
    {4, 4, 7, "FOUR"},
    {4, 11, 4, "FIVE"},
    {5, 0, 3, "SIX"},
    {5, 3, 7, "SEVEN"},
    {5, 10, 5, "EIGHT"},
    {6, 0, 4, "NINE"},
    {6, 4, 5, "TEN"},
    {6, 9, 6, "ELEVEN"},
};

void renderTimeToWords(CRGB *leds, int hour24, int minute, String &phrase) {
  phrase = "";
  lightWord(leds, W_IT_IS, phrase);

  int displayHour = hour24 % 12;
  // The caller renders time slightly ahead of actual, so just floor to the
  // nearest 5-minute word; hour rollover at :00 is already reflected in
  // hour24.
  int minuteBlock = (minute / 5) * 5;
  bool showNextHour = false;

  if (minuteBlock == 0) {
    lightWord(leds, W_OCLOCK, phrase);
  } else if (minuteBlock == 30) {
    lightWord(leds, W_HALF, phrase);
    lightWord(leds, W_PAST, phrase);
  } else {
    bool isPast = minuteBlock < 30;
    int distance = isPast ? minuteBlock : 60 - minuteBlock;

    switch (distance) {
      case 5: lightWord(leds, W_FIVE_MIN, phrase); lightWord(leds, W_MINUTES, phrase); break;
      case 10: lightWord(leds, W_TEN_MIN, phrase); lightWord(leds, W_MINUTES, phrase); break;
      case 15: lightWord(leds, W_QUARTER, phrase); break;
      case 20: lightWord(leds, W_TWENTY, phrase); lightWord(leds, W_MINUTES, phrase); break;
      case 25:
        lightWord(leds, W_TWENTY, phrase);
        lightWord(leds, W_FIVE_MIN, phrase);
        lightWord(leds, W_MINUTES, phrase);
        break;
    }
    lightWord(leds, isPast ? W_PAST : W_TO, phrase);
    showNextHour = !isPast; // "to" phrasing names the upcoming hour
  }

  int hourIndex = (showNextHour ? (displayHour + 1) : displayHour) % 12;
  lightWord(leds, HOUR_WORDS[hourIndex], phrase);
}
