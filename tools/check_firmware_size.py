#!/usr/bin/env python3
"""Fail CI when the application image exceeds its deliberate growth budget."""

import argparse
from pathlib import Path


DEFAULT_MAX_BYTES = 1_835_008  # 1.75 MiB; current RC is about 1.42 MB.


def validate_size(path: Path, maximum: int = DEFAULT_MAX_BYTES) -> int:
    size = path.stat().st_size
    if size > maximum:
        raise ValueError(f"firmware is {size} bytes; budget is {maximum} bytes")
    return size


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("image", type=Path)
    parser.add_argument("--max-bytes", type=int, default=DEFAULT_MAX_BYTES)
    args = parser.parse_args()
    try:
        size = validate_size(args.image, args.max_bytes)
    except (OSError, ValueError) as error:
        raise SystemExit(f"ERROR: {error}") from error
    print(f"firmware size: {size}/{args.max_bytes} bytes")


if __name__ == "__main__":
    main()
