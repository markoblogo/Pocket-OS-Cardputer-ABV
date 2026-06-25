#include "TerminalUI.h"

#include <M5Cardputer.h>

void TerminalUI::begin() {
  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextFont(1);
  charW_ = 6;
  charH_ = 8;
  rows_ = M5Cardputer.Display.height() / charH_;
  cols_ = M5Cardputer.Display.width() / charW_;
  clear();
}

void TerminalUI::clear() {
  M5Cardputer.Display.fillScreen(Black);
}

void TerminalUI::header(const char* title) {
  M5Cardputer.Display.fillRect(0, 0, M5Cardputer.Display.width(), charH_ + 3, Black);
  clippedText(0, 0, String("> ") + title, Green);
  M5Cardputer.Display.drawFastHLine(0, charH_ + 2, M5Cardputer.Display.width(), Green);
}

void TerminalUI::footer(const char* help) {
  int y = footerRow() * charH_;
  M5Cardputer.Display.fillRect(0, y - 1, M5Cardputer.Display.width(), charH_ + 1, Black);
  M5Cardputer.Display.drawFastHLine(0, y - 2, M5Cardputer.Display.width(), Dim);
  clippedText(0, y, help, Yellow);
}

void TerminalUI::line(uint8_t row, const String& text, uint16_t color) {
  if (row >= rows_) return;
  int y = row * charH_;
  M5Cardputer.Display.fillRect(0, y, M5Cardputer.Display.width(), charH_, Black);
  clippedText(0, y, text, color);
}

void TerminalUI::status(const String& text, uint16_t color) {
  line(footerRow() - 1, text, color);
}

void TerminalUI::listItem(uint8_t row, const String& text, bool selected) {
  if (row >= rows_) return;
  int y = row * charH_;
  uint16_t bg = selected ? Green : Black;
  uint16_t fg = selected ? Black : White;
  M5Cardputer.Display.fillRect(0, y, M5Cardputer.Display.width(), charH_, bg);
  clippedText(0, y, String(selected ? "> " : "  ") + text, fg, bg);
}

void TerminalUI::clippedText(int x, int y, const String& text, uint16_t color, uint16_t bg) {
  String clipped = text;
  if (clipped.length() > static_cast<size_t>(cols_)) clipped = clipped.substring(0, cols_);
  M5Cardputer.Display.setTextColor(color, bg);
  M5Cardputer.Display.setCursor(x, y);
  M5Cardputer.Display.print(clipped);
}
