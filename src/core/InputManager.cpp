#include "InputManager.h"
#include "KeyMap.h"

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

  scanDigitalSource(PhysicalSource::BtnGo, M5Cardputer.BtnA.isPressed(), InputAction::Select, now, false);
  scanDigitalSource(PhysicalSource::KeyUp, containsHidKey(keys, KEY_UP), InputAction::Up, now, true);
  scanDigitalSource(PhysicalSource::KeyDown, containsHidKey(keys, KEY_DOWN), InputAction::Down, now, true);
  scanDigitalSource(PhysicalSource::KeyLeft, containsHidKey(keys, KEY_LEFT), InputAction::Left, now, true);
  scanDigitalSource(PhysicalSource::KeyRight, containsHidKey(keys, KEY_RIGHT), InputAction::Right, now, true);
  scanDigitalSource(PhysicalSource::KeyEnter, keys.enter, InputAction::Enter, now, false);
  scanDigitalSource(PhysicalSource::KeyBackspace, keys.del, InputAction::Backspace, now, true);
  scanDigitalSource(PhysicalSource::KeyHelp, keys.fn, InputAction::Help, now, false);

  if (keyboardChanged && M5Cardputer.Keyboard.isPressed() && !displayAwake_ && !keys.word.empty()) {
    displayAwake_ = true;
    push(InputAction::Wake, InputEventType::Press, now);
    return;
  }

  if (keyboardChanged && M5Cardputer.Keyboard.isPressed()) {
    for (auto c : keys.word) {
      if (c >= 32 && c <= 126) {
        queueTextChar(static_cast<char>(c), now);
      }
    }
  }
}

bool InputManager::pollEvent(InputEvent& event) {
  if (count_ == 0) {
    return false;
  }

  event = queue_[tail_];
  tail_ = (tail_ + 1) % kQueueSize;
  --count_;
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
        if (state.suppressRelease) {
          return;
        }
        if (source == PhysicalSource::BtnGo) {
          push(InputAction::Back, InputEventType::LongPress, now);
        } else {
          push(action, InputEventType::LongPress, now);
        }
      }

      if (repeatable && !state.suppressRelease && now >= state.nextRepeatAt) {
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
    if (source == PhysicalSource::BtnGo && displayAwake_) {
      return;
    }
    emitPressOrWake(source, action, now);
    return;
  }

  if (state.suppressRelease) {
    state.suppressRelease = false;
    return;
  }

  if (source == PhysicalSource::BtnGo && state.longSent) {
    return;
  }

  if (source == PhysicalSource::BtnGo) {
    push(InputAction::Select, InputEventType::Press, now);
  }
  push(action, InputEventType::Release, now);
}

void InputManager::emitPressOrWake(PhysicalSource source, InputAction action, uint32_t now) {
  auto& state = states_[static_cast<uint8_t>(source)];
  if (!displayAwake_) {
    displayAwake_ = true;
    state.suppressRelease = true;
    push(InputAction::Wake, InputEventType::Press, now);
    return;
  }
  push(action, InputEventType::Press, now);
}

void InputManager::push(InputAction action, InputEventType type, uint32_t now, char text) {
  if (count_ == kQueueSize) {
    tail_ = (tail_ + 1) % kQueueSize;
    --count_;
  }

  queue_[head_].action = action;
  queue_[head_].type = type;
  queue_[head_].text = text;
  queue_[head_].timestamp = now;
  head_ = (head_ + 1) % kQueueSize;
  ++count_;
}

bool InputManager::queueTextChar(char c, uint32_t now) {
  if (!displayAwake_) {
    displayAwake_ = true;
    push(InputAction::Wake, InputEventType::Press, now);
    return false;
  }

  push(InputAction::TextChar, InputEventType::Press, now, c);
  return true;
}
