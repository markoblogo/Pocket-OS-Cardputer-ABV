#!/usr/bin/env python3
"""Protect release-facing repository contracts from documentation drift."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


changelog = read("CHANGELOG.md")
assert changelog.count("## Unreleased") == 1, "CHANGELOG must have one Unreleased section"

firmware = read("main/main.cpp")
assert 'connection_ap_password[16] = "cardputer"' not in firmware
assert 'strcmp(connection_ap_password, "cardputer")' not in firmware
assert '"abvx%08lx"' in firmware, "Transfer password must be generated"

smoke = read("docs/SMOKE_TEST.md")
assert "Password is `cardputer`" not in smoke
assert "generated password displayed on Cardputer" in smoke

audit = read("AUDIT.md")
assert "up to 32 MB" in audit
assert "Large upload remains disabled" not in audit
assert "Direct upload is limited" not in audit
assert "one direct upload" not in audit

readme = read("README.md")
assert "release candidate" in readme
assert "MIT License" in readme

packager = read("tools/package_companion.py")
assert "root.glob('*.py')" not in packager, "developer utilities must not enter the app bundle"
assert "runtime_sources" in packager

print("repository contract test: ok")
