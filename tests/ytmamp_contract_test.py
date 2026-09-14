#!/usr/bin/env python3
"""Keep the firmware client aligned with the vendored YTMamp cast contract."""

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
contract = json.loads((ROOT / "integrations/ytmamp-v1.json").read_text(encoding="utf-8"))
firmware = (ROOT / "main/main.cpp").read_text(encoding="utf-8")

assert contract["api_version"] == 1
assert contract["default_port"] == 18880
assert contract["security"]["token_header"] == "X-YTMAMP-Token"
assert 'cast_port = 18880' in firmware
assert 'cast_host[CAST_MAX_HOST + 1] = "192.168.4.2"' in firmware
assert '"X-YTMAMP-API-Version", "1"' in firmware
assert '"X-YTMAMP-Token", cast_token' in firmware
assert '"/api/cast/status"' in firmware
assert '"/api/cast/cmd"' in firmware
for action in ("toggle", "next", "prev"):
    assert f'"{action}"' in firmware

print("YTMamp cast contract test: ok")
