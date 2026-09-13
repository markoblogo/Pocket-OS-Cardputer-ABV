#!/usr/bin/env python3
"""Keep safety patches when refreshing the vendored M5Unified tree."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
led = (ROOT / "components/M5Unified/src/utility/LED_Class.cpp").read_text()
rtc = (ROOT / "components/M5Unified/src/utility/rtc/RTC_PowerHub_Class.cpp").read_text()

assert "LED_Class::getLedType(size_t index) const" in led
assert "index >= _led_instance->getCount()" in led
assert "index >= static_cast<size_t>(count)" in led
assert "date && date->date >= 0" in rtc
assert "bitOff(0xD3, 0x01)" in rtc
assert "bitOff(0xD3, 0);" not in rtc

print("M5Unified vendor patches: OK")
