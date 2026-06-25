#include "TextEditor.h"

void TextEditor::setText(const String& text) {
  text_ = text;
  cursor_ = text_.length();
  dirty_ = false;
}

void TextEditor::clear() {
  text_ = "";
  cursor_ = 0;
  dirty_ = false;
}

void TextEditor::onInput(const InputEvent& event, bool enterNewline) {
  if (event.type != InputEventType::Press && event.type != InputEventType::Repeat) return;
  if (event.action == InputAction::TextChar) {
    text_ = text_.substring(0, cursor_) + event.text + text_.substring(cursor_);
    ++cursor_;
    dirty_ = true;
  } else if (event.action == InputAction::Backspace && cursor_ > 0) {
    text_.remove(cursor_ - 1, 1);
    --cursor_;
    dirty_ = true;
  } else if (event.action == InputAction::Enter && enterNewline) {
    text_ = text_.substring(0, cursor_) + '\n' + text_.substring(cursor_);
    ++cursor_;
    dirty_ = true;
  } else if (event.action == InputAction::Left) {
    moveCursor(-1);
  } else if (event.action == InputAction::Right) {
    moveCursor(1);
  }
}

void TextEditor::moveCursor(int delta) {
  int next = static_cast<int>(cursor_) + delta;
  if (next < 0) next = 0;
  if (next > static_cast<int>(text_.length())) next = text_.length();
  cursor_ = next;
}

String TextEditor::visibleLine(uint16_t maxChars) const {
  uint16_t start = cursor_ > maxChars ? cursor_ - maxChars : 0;
  String out = text_.substring(start, min<uint16_t>(text_.length(), start + maxChars));
  out.replace('\n', ' ');
  return out;
}
