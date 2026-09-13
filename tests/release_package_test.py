#!/usr/bin/env python3

import hashlib
import importlib.util
import tempfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location("package_release", ROOT / "tools/package_release.py")
module = importlib.util.module_from_spec(spec)
assert spec.loader
spec.loader.exec_module(module)

with tempfile.TemporaryDirectory() as directory:
    root = Path(directory)
    build = root / "build"
    (build / "bootloader").mkdir(parents=True)
    (build / "partition_table").mkdir()
    fixtures = {
        build / "cardputer-abvx-minimal.bin": b"application",
        build / "bootloader/bootloader.bin": b"bootloader",
        build / "partition_table/partition-table.bin": b"partitions",
    }
    for path, content in fixtures.items():
        path.write_bytes(content)
    output = root / "release"
    lines = module.package(build, output)
    assert len(lines) == 3
    checksums = (output / "SHA256SUMS.txt").read_text(encoding="ascii")
    for path, content in fixtures.items():
        output_name = path.name if path.name != "partition-table.bin" else "partition-table.bin"
        assert f"{hashlib.sha256(content).hexdigest()}  {output_name}" in checksums

print("release package test: ok")
