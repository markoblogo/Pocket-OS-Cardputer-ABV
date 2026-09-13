#!/usr/bin/env python3
"""Verify that documented M5GFX RGB tuples are encoded as RGB565."""

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
source = (ROOT / "components/M5GFX/src/lgfx/v1/misc/enum.hpp").read_text()
start = source.index("static constexpr int TFT_ALICEBLUE")
end = source.index("namespace gradient_fill_styles", start)
table = source[start:end]
entries = re.findall(
    r"TFT_[A-Z]+\s*=\s*0x([0-9A-Fa-f]+);\s*/\*\s*(\d+),\s*(\d+),\s*(\d+)\s*\*/",
    table,
)
assert len(entries) >= 100
for encoded, red, green, blue in entries:
    r, g, b = map(int, (red, green, blue))
    expected = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
    assert int(encoded, 16) == expected, (encoded, r, g, b, f"{expected:04X}")

print(f"M5GFX color table: {len(entries)} RGB565 values OK")
