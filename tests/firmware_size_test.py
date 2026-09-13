#!/usr/bin/env python3

import importlib.util
import tempfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location("check_firmware_size", ROOT / "tools/check_firmware_size.py")
module = importlib.util.module_from_spec(spec)
assert spec.loader
spec.loader.exec_module(module)

with tempfile.TemporaryDirectory() as directory:
    image = Path(directory) / "firmware.bin"
    image.write_bytes(b"x" * 10)
    assert module.validate_size(image, 10) == 10
    try:
        module.validate_size(image, 9)
    except ValueError as error:
        assert "budget is 9 bytes" in str(error)
    else:
        raise AssertionError("oversized firmware must fail")

print("firmware size test: ok")
