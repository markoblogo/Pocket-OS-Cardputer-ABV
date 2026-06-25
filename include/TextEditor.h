#pragma once

#include <Arduino.h>
#include "InputManager.h"

class TextEditor {
public:
  void setText(const String& text);
  const String& text() const { return text_; }
  void clear();
  void onInput(const InputEvent& event, bool enterNewline = true);
  void moveCursor(int delta);
  bool dirty() const { return dirty_; }
  void markSaved() { dirty_ = false; }
  uint16_t cursor() const { return cursor_; }
  String visibleLine(uint16_t maxChars) const;

private:
  String text_;
  uint16_t cursor_ = 0;
  bool dirty_ = false;
};
