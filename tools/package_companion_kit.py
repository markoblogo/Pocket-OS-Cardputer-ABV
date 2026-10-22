#!/usr/bin/env python3
"""Reproducible Companion source/installer kit, without machine-specific app data."""
import argparse
import hashlib
import re
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def package(root: Path, output: Path) -> Path:
    version = re.search(r'set\(PROJECT_VER "([0-9A-Za-z.-]+)"\)',
                        (root / "CMakeLists.txt").read_text()).group(1)
    runtime = [line for line in (root / "tools/companion_runtime_files.txt").read_text().splitlines()
               if line and not line.startswith("#")]
    files = ["LICENSE", "docs/COMPANION_DESKTOP.md", "docs/VOICE_TO_TEXT.md",
             "docs/DATA_SAFETY.md", "tools/install_companion_app.zsh",
             "tools/companion_runtime_files.txt", "tools/mac/Companion.swift",
             "tools/mac/ABVx.icns", "tools/companion_ui/index.html"]
    files += ["tools/" + name for name in runtime]
    output.mkdir(parents=True, exist_ok=True)
    archive = output / f"ABVx-Companion-{version}-source.zip"
    with zipfile.ZipFile(archive, "w", compression=zipfile.ZIP_DEFLATED) as bundle:
        for name in sorted(set(files)):
            source = root / name
            if source.is_symlink() or not source.is_file():
                raise ValueError(f"Invalid kit member: {name}")
            info = zipfile.ZipInfo("ABVx-Companion/" + name, date_time=(2026, 1, 1, 0, 0, 0))
            info.compress_type = zipfile.ZIP_DEFLATED
            info.external_attr = (0o100755 if name.endswith(".zsh") else 0o100644) << 16
            bundle.writestr(info, source.read_bytes())
        bundle.writestr("ABVx-Companion/START-HERE.txt",
                        "Companion source + local Mac installer; Python is not bundled.\n"
                        "Run: zsh tools/install_companion_app.zsh\n"
                        "Read docs/COMPANION_DESKTOP.md first.\n"
                        "Firmware build/flash needs a full project checkout and ESP-IDF.\n"
                        "No model weights, recordings or personal settings are included.\n")
    checksum = output / "SHA256SUMS.txt"
    old = checksum.read_text() if checksum.exists() else ""
    lines = [line for line in old.splitlines() if not line.endswith("  " + archive.name)]
    lines.append(hashlib.sha256(archive.read_bytes()).hexdigest() + "  " + archive.name)
    checksum.write_text("\n".join(lines) + "\n", encoding="ascii")
    return archive


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--output-dir", type=Path, default=Path("build/release"))
    args = parser.parse_args()
    print(package(ROOT, args.output_dir))
