#include "display.h"
#include "Arduino.h"

namespace {
constexpr uint8_t kLcdColumns = 16;
constexpr uint8_t kProgressPixelsPerCell = 5;
constexpr uint8_t kTimerBarWidth = 15;
constexpr uint8_t kHoldBarWidth = 14;
constexpr uint8_t kRowBufferSize = kLcdColumns + 1;

enum CustomChar : uint8_t {
  BAR_1 = 0,
  BAR_2,
  BAR_3,
  BAR_4,
  BAR_5,
  FOCUS_ICON,
  BREAK_ICON,
  CHECK_ICON
};

uint8_t bar1[8] = {
  0b00000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b00000,
};

uint8_t bar2[8] = {
  0b00000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b00000,
};

uint8_t bar3[8] = {
  0b00000,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b00000,
};

uint8_t bar4[8] = {
  0b00000,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b00000,
};

uint8_t bar5[8] = {
  0b00000,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b00000,
};

uint8_t focusIcon[8] = {
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b10101,
  0b01110,
  0b00100,
  0b00000,
};

uint8_t breakIcon[8] = {
  0b00110,
  0b01100,
  0b11000,
  0b11000,
  0b11000,
  0b01100,
  0b00110,
  0b00000,
};

uint8_t checkIcon[8] = {
  0b00000,
  0b00001,
  0b00010,
  0b10100,
  0b01000,
  0b00000,
  0b00000,
  0b00000,
};

// Start with a blank 16-character row so each screen can build its text in memory
// before writing a full row overwrite to the LCD in one pass.
void fillRow(char row[kRowBufferSize]) {
  for (uint8_t i = 0; i < kLcdColumns; ++i) {
    row[i] = ' ';
  }
  row[kLcdColumns] = '\0';
}

// Copy text into the row buffer without spilling past the fixed 16-column width of the diplay.
void copyText(char row[kRowBufferSize], uint8_t startCol, const char* text) {
  uint8_t col = startCol;
  while (col < kLcdColumns && *text != '\0') {
    row[col++] = *text++;
  }
}

// Write a clamped, zero-padded 2-digit number directly into a row buffer. Keep it 
// in the range of 00 - 99
void writeTwoDigits(char row[kRowBufferSize], uint8_t startCol, int value) {
  if (startCol + 1 >= kLcdColumns) return;

  int clamped = value;
  if (clamped < 0) clamped = 0;
  if (clamped > 99) clamped = 99;

  row[startCol] = static_cast<char>('0' + (clamped / 10));
  row[startCol + 1] = static_cast<char>('0' + (clamped % 10));
}

// Format a compact MM:SS timestamp at a fixed column position inside the row buffer.
void writeTime(char row[kRowBufferSize], uint8_t startCol, int minutes, int seconds) {
  if (startCol + 4 >= kLcdColumns) return;

  writeTwoDigits(row, startCol, minutes);
  row[startCol + 2] = ':';
  writeTwoDigits(row, startCol + 3, seconds);
}
}

Display::Display(LiquidCrystal& dp)
  : lcd_(dp) {}

void Display::begin() {
  lcd_.begin(16, 2);
  lcd_.clear();
  loadCustomChars();
  lcd_.setCursor(0, 0);
}

// LiquidCrystal exposes only 8 CGRAM slots, so we spend them on the visuals that
// add the most value on our 16x2 screen: smooth progress, mode icons, and confirmation.
void Display::loadCustomChars() {
  lcd_.createChar(BAR_1, bar1);
  lcd_.createChar(BAR_2, bar2);
  lcd_.createChar(BAR_3, bar3);
  lcd_.createChar(BAR_4, bar4);
  lcd_.createChar(BAR_5, bar5);
  lcd_.createChar(FOCUS_ICON, focusIcon);
  lcd_.createChar(BREAK_ICON, breakIcon);
  lcd_.createChar(CHECK_ICON, checkIcon);
}

// Always write a full row so content from the previous screen cannot remain visible
// when the new text is shorter and we are intentionally avoiding lcd.clear().
void Display::writePaddedRow(uint8_t row, const char* text) {
  lcd_.setCursor(0, row);

  uint8_t col = 0;
  while (col < kLcdColumns && text[col] != '\0') {
    lcd_.write(text[col]);
    ++col;
  }

  while (col < kLcdColumns) {
    lcd_.write(' ');
    ++col;
  }
}

// Convert progress into 5-pixel cells so partially filled custom glyphs can produce
// a smoother bar than a simple on/off character fill.
void Display::writeSmoothBar(uint8_t row, uint8_t col, uint8_t width, unsigned long elapsedMs, unsigned long totalMs) {
  const unsigned long totalPixels = static_cast<unsigned long>(width) * kProgressPixelsPerCell;
  const unsigned long clampedElapsed = totalMs == 0 ? totalMs : (elapsedMs > totalMs ? totalMs : elapsedMs);
  const unsigned long filledPixels = totalMs == 0 ? totalPixels : (clampedElapsed * totalPixels) / totalMs;

  lcd_.setCursor(col, row);

  for (uint8_t i = 0; i < width; ++i) {
    const unsigned long cellStartPixel = static_cast<unsigned long>(i) * kProgressPixelsPerCell;

    uint8_t pixelsInCell = 0;
    if (filledPixels > cellStartPixel) {
      const unsigned long pixelsRemaining = filledPixels - cellStartPixel;
      pixelsInCell = pixelsRemaining >= kProgressPixelsPerCell ? kProgressPixelsPerCell : static_cast<uint8_t>(pixelsRemaining);
    }

    if (pixelsInCell == 0) {
      lcd_.write(' ');
    } else {
      lcd_.write(static_cast<uint8_t>(pixelsInCell - 1));
    }
  }
}

void Display::renderMenu(int selectedIndex) {
  writePaddedRow(0, selectedIndex == 0 ? "> Start timer" : "  Start timer");
  writePaddedRow(1, selectedIndex == 1 ? "> Set presets" : "  Set presets");
}

void Display::renderConfig(int selectedIndex, bool buzzerEnabled) {
  if(selectedIndex < 1) {
    writePaddedRow(0, selectedIndex == 0 ? "> Quick     5/3 " : "  Quick     5/3 ");
    writePaddedRow(1, selectedIndex == 1 ? "> Long      10/5" : "  Long      10/5");
  } else {
    writePaddedRow(0, selectedIndex == 1 ? "> Long      10/5" : "  Long      10/5");

    char row[kRowBufferSize];
    fillRow(row);
    copyText(row, 0, selectedIndex == 2 ? "> " : " ");
    copyText(row, 2, "Buzzer");
    copyText(row, 12, buzzerEnabled ? "ON" : "OFF");
    writePaddedRow(1, row);
    
  }
}

// the timer screen keeps the top row purely visual and reserves the second row for
// the mode label, paused marker, and right-aligned timer.
void Display::renderTimer(int minutes, int seconds, bool isFocused, bool isPaused, long remainingMs, unsigned long sessionDurationMs) {
  unsigned long clampedRemainingMs = 0;
  if (remainingMs > 0) {
    clampedRemainingMs = static_cast<unsigned long>(remainingMs);
    if (clampedRemainingMs > sessionDurationMs) {
      clampedRemainingMs = sessionDurationMs;
    }
  }

  const unsigned long elapsedMs = sessionDurationMs > clampedRemainingMs ? sessionDurationMs - clampedRemainingMs : 0;

  lcd_.setCursor(0, 0);
  lcd_.write(static_cast<uint8_t>(isFocused ? FOCUS_ICON : BREAK_ICON));
  writeSmoothBar(0, 1, kTimerBarWidth, elapsedMs, sessionDurationMs);

  char row[kRowBufferSize];
  fillRow(row);
  copyText(row, 0, isFocused ? "Focus" : "Break");

  if (isPaused) {
    copyText(row, 8, "||");
  }

  writeTime(row, 11, minutes, seconds);
  writePaddedRow(1, row);
}

// The hold overlay reuses the same progress language as the timer, then briefly swaps
// the label for a confirmation state once the hold has completed.
void Display::renderHold(const char* label, unsigned long elapsedMs, unsigned long holdMs, bool isConfirmed) {
  if (isConfirmed) {
    char row[kRowBufferSize];
    fillRow(row);
    copyText(row, 1, "Confirmed");

    lcd_.setCursor(0, 0);
    lcd_.write(static_cast<uint8_t>(CHECK_ICON));
    for (uint8_t i = 1; i < kLcdColumns; ++i) {
      lcd_.write(row[i]);
    }
  } else {
    writePaddedRow(0, label);
  }

  lcd_.setCursor(0, 1);
  lcd_.write('[');
  writeSmoothBar(1, 1, kHoldBarWidth, elapsedMs, holdMs);
  lcd_.setCursor(kLcdColumns - 1, 1);
  lcd_.write(']');
}