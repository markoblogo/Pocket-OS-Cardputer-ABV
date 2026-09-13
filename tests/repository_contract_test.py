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

transfer_security = read("main/transfer_security.cpp")
assert '"abvx%08lx"' in transfer_security, "Transfer password must be generated"

smoke = read("docs/SMOKE_TEST.md")
assert "Password is `cardputer`" not in smoke
assert "generated password displayed on Cardputer" in smoke
assert "Record release-gate results in `HARDWARE_ACCEPTANCE.md`" in smoke

audit = read("AUDIT.md")
assert "up to 32 MB" in audit
assert "Large upload remains disabled" not in audit
assert "Direct upload is limited" not in audit
assert "one direct upload" not in audit

readme = read("README.md")
assert "release candidate" in readme
assert "MIT License" in readme

docs_index = read("docs/README.md")
assert "physical-device evidence\nbelongs in `HARDWARE_ACCEPTANCE.md`" in docs_index

packager = read("tools/package_companion.py")
assert "root.glob('*.py')" not in packager, "developer utilities must not enter the app bundle"
assert "runtime_sources" in packager

installer = read("tools/install_companion_app.zsh")
assert 'tools/*.py' not in installer, "installer must not copy developer utilities"
assert "companion_runtime_files.txt" in installer

runtime_files = set(read("tools/companion_runtime_files.txt").splitlines())
assert "abvx_companion_app.py" in runtime_files
assert "check_firmware_size.py" not in runtime_files
assert "package_release.py" not in runtime_files

print("repository contract test: ok")
