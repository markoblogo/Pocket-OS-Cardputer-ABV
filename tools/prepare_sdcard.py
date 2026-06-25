#!/usr/bin/env python3
import argparse
import shutil
import sys
from pathlib import Path

REQUIRED_DIRS = [
    "music", "recordings", "notes", "books", "browser",
    "browser/bookmarks", "browser/saved_pages", "ai", "config", "logs", "tmp",
]

def is_unsafe(target: Path, project_root: Path) -> bool:
    target = target.resolve()
    return (
        str(target) == "/" or
        target == project_root or
        project_root in target.parents or
        str(target) in {str(Path.home()), "/Volumes", "/mnt", "/media"}
    )

def main() -> int:
    parser = argparse.ArgumentParser(description="Prepare a Cardputer SD card without deleting user files.")
    parser.add_argument("target", help="SD card mount path, e.g. /Volumes/CARDPUTER")
    parser.add_argument("--force", action="store_true", help="Overwrite template config/readme files")
    args = parser.parse_args()

    project_root = Path(__file__).resolve().parents[1]
    template = project_root / "sdcard_template"
    target = Path(args.target).expanduser()

    if not template.is_dir():
      print("ERROR: sdcard_template not found", file=sys.stderr)
      return 2
    if not target.exists() or not target.is_dir():
      print(f"ERROR: target does not exist or is not a directory: {target}", file=sys.stderr)
      return 2
    if is_unsafe(target, project_root):
      print(f"ERROR: refusing unsafe target path: {target}", file=sys.stderr)
      return 2

    created_dirs = skipped_dirs = copied = skipped = overwritten = 0
    for rel in REQUIRED_DIRS:
        dest = target / rel
        if dest.exists():
            skipped_dirs += 1
        else:
            dest.mkdir(parents=True, exist_ok=True)
            created_dirs += 1

    for src in template.rglob("*"):
        rel = src.relative_to(template)
        dest = target / rel
        if src.is_dir():
            continue
        dest.parent.mkdir(parents=True, exist_ok=True)
        if dest.exists() and not args.force:
            skipped += 1
            continue
        if dest.exists() and args.force:
            overwritten += 1
        else:
            copied += 1
        shutil.copy2(src, dest)

    print("SD card preparation summary")
    print(f"Target: {target}")
    print(f"Created dirs: {created_dirs}")
    print(f"Skipped dirs: {skipped_dirs}")
    print(f"Copied files: {copied}")
    print(f"Skipped files: {skipped}")
    print(f"Overwritten files: {overwritten}")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())

