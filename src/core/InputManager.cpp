#include "InputManager.h"
#include "Features.h"
#include "KeyMap.h"

#include <Arduino.h>
#include <M5Cardputer.h>

namespace {
bool containsHidKey(const Keyboard_Class::KeysState& keys, uint8_t key) {
  for (auto value : keys.hid_keys) {
    if (value == key) {
      return true;
    }
  }
  return false;
}

static const char* actionToName(InputAction action) {
  switch (action) {
    case InputAction::Up: return "Up";
    case InputAction::Down: return "Down";
    case InputAction::Left: return "Left";
    case InputAction::Right: return "Right";
    case InputAction::Select: return "Select";
    case InputAction::Back: return "Back";
    case InputAction::Enter: return "Enter";
    case InputAction::TextChar: return "TextChar";
    case InputAction::Backspace: return "Backspace";
    case InputAction::Help: return "Help";
    case InputAction::Wake: return "Wake";
    default: return "None";
  }
}

static InputAction mapPunctuationToNav(char c, bool fnAware) {
  if (!fnAware) return InputAction::None;
  if (c == ';' || c == ':') return InputAction::Up;
  if (c == ',' || c == '<') return InputAction::Left;
  if (c == '.' || c == '>') return InputAction::Down;
  if (c == '/' || c == '?') return InputAction::Right;
  return InputAction::None;
}

static InputAction mapFallbackToNav(char c) {
  switch (tolower(static_cast<uint8_t>(c))) {
    case 'w': return InputAction::Up;
    case 'a': return InputAction::Left;
    case 's': return InputAction::Down;
    case 'd': return InputAction::Right;
    case 'h': return InputAction::Left;
    case 'j': return InputAction::Down;
    case 'k': return InputAction::Up;
    case 'l': return InputAction::Right;
    case '\t': return InputAction::Right;
    default: return InputAction::None;
  }
}
}  // namespace

void InputManager::begin(const InputTiming& timing) {
  timing_ = timing;
  clear();
  uint32_t now = millis();
  for (auto& state : states_) {
    state = ButtonState{};
    state.rawChangedAt = now;
  }
}

void InputManager::update() {
  M5Cardputer.update();

  uint32_t now = millis();
  bool keyboardChanged = M5Cardputer.Keyboard.isChange();
  auto keys = M5Cardputer.Keyboard.keysState();

  lastRawText_.remove(0);
  lastFn_ = keys.fn;
  lastWakeSuppressed_ = false;

  bool textMode = context_ == InputContext::TextEntry;

  if (!displayAwake_ && keyboardChanged && M5Cardputer.Keyboard.isPressed()) {
    for (auto c : keys.word) {
      if (c >= 0x20 && c <= 0x7E) {
        lastRawText_ += c;
      }
    }
    if (lastRawText_.isEmpty() && (keys.enter || keys.del || keys.fn)) {
      lastRawText_ = String("<special>");
    }
    push(InputAction::Wake, InputEventType::Press, now, '\0', true);
    displayAwake_ = true;
    lastWakeSuppressed_ = true;
    return;
  }

  scanDigitalSource(PhysicalSource::BtnGo, M5Cardputer.BtnA.isPressed(), InputAction::Select, now, false);
  scanDigitalSource(PhysicalSource::KeyUp, containsHidKey(keys, KEY_UP), InputAction::Up, now, true);
  scanDigitalSource(PhysicalSource::KeyDown, containsHidKey(keys, KEY_DOWN), InputAction::Down, now, true);
  scanDigitalSource(PhysicalSource::KeyLeft, containsHidKey(keys, KEY_LEFT), InputAction::Left, now, true);
  scanDigitalSource(PhysicalSource::KeyRight, containsHidKey(keys, KEY_RIGHT), InputAction::Right, now, true);
  scanDigitalSource(PhysicalSource::KeyEnter, keys.enter, InputAction::Enter, now, false);
  scanDigitalSource(PhysicalSource::KeyBackspace, keys.del, InputAction::Backspace, now, true);
  scanDigitalSource(PhysicalSource::KeyHelp, keys.fn, InputAction::Help, now, false);

  bool hadMappedInput = false;
  String mappedActions;
  if (keyboardChanged && M5Cardputer.Keyboard.isPressed()) {
    for (auto c : keys.word) {
      c = static_cast<char>(c);
      if (c >= 0x20 && c <= 0x7E) {
        lastRawText_ += c;
      }

      InputAction nav = InputAction::None;
      if (textMode) {
        nav = mapPunctuationToNav(c, keys.fn);
      } else {
        nav = mapPunctuationToNav(c, true);
        if (nav == InputAction::None) nav = mapFallbackToNav(c);
      }

      if (nav != InputAction::None) {
        queueMappedEvent(nav, now);
        hadMappedInput = true;
        if (mappedActions.length()) mappedActions += ", ";
        mappedActions += actionToName(nav);
      } else {
        queueTextChar(c, now);
      }
    }
  }

#if FEATURE_INPUT_DIAGNOSTICS
  if (keyboardChanged && M5Cardputer.Keyboard.isPressed()) {
    String rawWord;
    for (auto c : keys.word) rawWord += static_cast<char>(c);
    Serial.print("[Input] raw=");
    Serial.print(rawWord.length() ? rawWord.c_str() : "(none)");
    Serial.print(" fn=");
    Serial.print(keys.fn ? "1" : "0");
    Serial.print(" enter=");
    Serial.print(keys.enter ? "1" : "0");
    Serial.print(" del=");
    Serial.print(keys.del ? "1" : "0");
    Serial.print(" mapped=");
    Serial.println(hadMappedInput ? mappedActions.c_str() : "none");
  }
#endif
}

bool InputManager::pollEvent(InputEvent& event) {
  if (count_ == 0) {
    return false;
  }

  event = queue_[tail_];
  tail_ = (tail_ + 1) % kQueueSize;
  --count_;
  lastWakeSuppressed_ = event.wakeSuppressed;
  return true;
}

bool InputManager::hasEvent() const {
  return count_ > 0;
}

void InputManager::clear() {
  head_ = 0;
  tail_ = 0;
  count_ = 0;
}

void InputManager::setInputContext(InputContext context) {
  context_ = context;
}

InputContext InputManager::getInputContext() const {
  return context_;
}

const String& InputManager::lastRawText() const {
  return lastRawText_;
}

bool InputManager::lastFnState() const {
  return lastFn_;
}

bool InputManager::lastWakeSuppressed() const {
  return lastWakeSuppressed_;
}

void InputManager::setDisplayAwake(bool awake) {
  displayAwake_ = awake;
}

bool InputManager::isDisplayAwake() const {
  return displayAwake_;
}

void InputManager::scanDigitalSource(
    PhysicalSource source,
    bool rawDown,
    InputAction action,
    uint32_t now,
    bool repeatable) {
  auto& state = states_[static_cast<uint8_t>(source)];

  if (rawDown != state.lastRawDown) {
    state.lastRawDown = rawDown;
    state.rawChangedAt = now;
  }

  if ((now - state.rawChangedAt) < timing_.debounceMs || rawDown == state.stableDown) {
    if (state.stableDown) {
      if (!state.longSent && (now - state.pressedAt) >= timing_.longPressMs) {
        state.longSent = true;
        if (state.suppressRelease) return;
        if (source == PhysicalSource::BtnGo) {
          push(InputAction::Back, InputEventType::LongPress, now, '\0');
        }
      }

      if (repeatable && !state.suppressRelease && now >= state.nextRepeatAt && !state.longSent) {
        push(action, InputEventType::Repeat, now);
        state.nextRepeatAt = now + timing_.repeatEveryMs;
      }
    }
    return;
  }

  state.stableDown = rawDown;

  if (state.stableDown) {
    state.longSent = false;
    state.suppressRelease = false;
    state.pressedAt = now;
    state.nextRepeatAt = now + timing_.repeatStartMs;
    emitPressOrWake(source, action, now);
    return;
  }

  if (source == PhysicalSource::BtnGo) {
    return;
  }

  if (state.suppressRelease) {
    state.suppressRelease = false;
    return;
  }

  push(action, InputEventType::Release, now);
}

void InputManager::emitPressOrWake(PhysicalSource source, InputAction action, uint32_t now) {
  auto& state = states_[static_cast<uint8_t>(source)];
  if (!displayAwake_) {
    displayAwake_ = true;
    state.suppressRelease = true;
    lastWakeSuppressed_ = true;
    push(InputAction::Wake, InputEventType::Press, now, '\0', true);
    return;
  }

  push(action, InputEventType::Press, now);
}

void InputManager::push(InputAction action, InputEventType type, uint32_t now, char text, bool wakeSuppressed) {
  if (count_ == kQueueSize) {
    tail_ = (tail_ + 1) % kQueueSize;
    --count_;
  }

  queue_[head_].action = action;
  queue_[head_].type = type;
  queue_[head_].text = text;
  queue_[head_].timestamp = now;
  queue_[head_].wakeSuppressed = wakeSuppressed;
  lastEvent_ = queue_[head_];
  head_ = (head_ + 1) % kQueueSize;
  ++count_;
}

bool InputManager::queueTextChar(char c, uint32_t now) {
  if (!displayAwake_) {
    displayAwake_ = true;
    push(InputAction::Wake, InputEventType::Press, now, '\0', true);
    return false;
  }

  if (c == '\0') return false;
  push(InputAction::TextChar, InputEventType::Press, now, c);
  return true;
}

bool InputManager::queueMappedEvent(InputAction action, uint32_t now) {
  if (action == InputAction::None) return false;
  if (!displayAwake_) {
    displayAwake_ = true;
    lastWakeSuppressed_ = true;
    push(InputAction::Wake, InputEventType::Press, now, '\0', true);
    return false;
  }
  push(action, InputEventType::Press, now);
  return true;
}

const InputEvent& InputManager::lastEvent() const {
  return lastEvent_;
}
