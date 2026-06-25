#!/usr/bin/env python3
import argparse
import json
import re
import sys
from pathlib import Path

REQUIRED_DIRS = [
    "music", "recordings", "notes", "books", "browser",
    "browser/bookmarks", "browser/saved_pages", "ai", "config", "logs", "tmp",
]
REQUIRED_JSON = [
    "config/settings.json",
    "config/wifi.json",
    "config/ai.json",
    "config/bookmarks.json",
    "config/app_state.json",
]
KEY_PATTERNS = [
    re.compile(r"sk-[A-Za-z0-9_-]{20,}"),
    re.compile(r"AIza[0-9A-Za-z_-]{20,}"),
    re.compile(r"Bearer\s+[A-Za-z0-9._-]{20,}", re.I),
]

def main() -> int:
    parser = argparse.ArgumentParser(description="Validate a prepared Cardputer SD card.")
    parser.add_argument("target", help="SD card mount path")
    parser.add_argument("--template-mode", action="store_true", help="Warn if ai.json appears to contain a real key")
    args = parser.parse_args()
    root = Path(args.target).expanduser()
    errors = []
    warnings = []

    if not root.is_dir():
        print(f"ERROR: target is not a directory: {root}", file=sys.stderr)
        return 2

    for rel in REQUIRED_DIRS:
        if not (root / rel).is_dir():
            errors.append(f"Missing folder: /{rel}")

    for rel in REQUIRED_JSON:
        path = root / rel
        if not path.is_file():
            errors.append(f"Missing config: /{rel}")
            continue
        try:
            json.loads(path.read_text(encoding="utf-8"))
        except Exception as exc:
            errors.append(f"Invalid JSON /{rel}: {exc}")

    if not list((root / "music").glob("*.mp3")):
        warnings.append("No .mp3 files found in /music")
    if not list((root / "books").glob("*.txt")):
        warnings.append("No .txt files found in /books")
    if not list((root / "notes").glob("*.txt")):
        warnings.append("No .txt files found in /notes")

    ai_path = root / "config/ai.json"
    if ai_path.exists():
        text = ai_path.read_text(encoding="utf-8", errors="ignore")
        try:
            ai = json.loads(text)
            api_key = str(ai.get("apiKey", ""))
            if not api_key:
                warnings.append("/config/ai.json has no API key; AI app will report API key missing")
            if args.template_mode and api_key:
                warnings.append("Template mode: ai.json contains a non-empty API key")
        except Exception:
            pass
        if any(p.search(text) for p in KEY_PATTERNS):
            warnings.append("/config/ai.json appears to contain a real-looking API key")

    print("SD card validation")
    print(f"Target: {root}")
    for warning in warnings:
        print(f"WARNING: {warning}")
    for error in errors:
        print(f"ERROR: {error}")
    print(f"Warnings: {len(warnings)}")
    print(f"Errors: {len(errors)}")
    return 1 if errors else 0

if __name__ == "__main__":
    raise SystemExit(main())

