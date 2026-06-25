#pragma once

#include <Arduino.h>

class TerminalUI {
public:
  static constexpr uint16_t Black = 0x0000;
  static constexpr uint16_t White = 0xFFFF;
  static constexpr uint16_t Green = 0x07E0;
  static constexpr uint16_t Yellow = 0xFFE0;
  static constexpr uint16_t Red = 0xF800;
  static constexpr uint16_t Dim = 0x7BEF;

  void begin();
  void clear();
  void header(const char* title);
  void footer(const char* help);
  void line(uint8_t row, const String& text, uint16_t color = White);
  void status(const String& text, uint16_t color = Yellow);
  void listItem(uint8_t row, const String& text, bool selected);
  int rows() const { return rows_; }
  int cols() const { return cols_; }
  int footerRow() const { return rows_ - 1; }

private:
  int charW_ = 6;
  int charH_ = 8;
  int rows_ = 17;
  int cols_ = 40;
  void clippedText(int x, int y, const String& text, uint16_t color, uint16_t bg = Black);
};
