#!/usr/bin/env python3
"""Create the small, checksummed firmware asset set used by releases."""

import argparse
import hashlib
import shutil
from pathlib import Path


ASSETS = {
    "cardputer-abvx-minimal.bin": Path("cardputer-abvx-minimal.bin"),
    "bootloader.bin": Path("bootloader/bootloader.bin"),
    "partition-table.bin": Path("partition_table/partition-table.bin"),
}


def package(build_dir: Path, output_dir: Path) -> list[str]:
    output_dir.mkdir(parents=True, exist_ok=True)
    lines = []
    for output_name, relative_source in ASSETS.items():
        source = build_dir / relative_source
        if not source.is_file():
            raise FileNotFoundError(source)
        destination = output_dir / output_name
        shutil.copyfile(source, destination)
        digest = hashlib.sha256(destination.read_bytes()).hexdigest()
        lines.append(f"{digest}  {output_name}")
    (output_dir / "SHA256SUMS.txt").write_text("\n".join(lines) + "\n", encoding="ascii")
    return lines


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--build-dir", type=Path, default=Path("build"))
    parser.add_argument("--output-dir", type=Path, default=Path("build/release"))
    args = parser.parse_args()
    try:
        lines = package(args.build_dir, args.output_dir)
    except OSError as error:
        raise SystemExit(f"ERROR: {error}") from error
    print(f"release assets: {len(lines)} binaries + SHA256SUMS.txt")


if __name__ == "__main__":
    main()
