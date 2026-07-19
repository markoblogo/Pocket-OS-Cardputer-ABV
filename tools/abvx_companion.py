#!/usr/bin/env python3
"""ABVx Mac Companion Core for direct SD preparation and clock sync."""

import argparse
import os
import shutil
import subprocess
import sys
import unicodedata
from pathlib import Path


LAYOUT = ("music", "books", "notes", "CARDPTR")
MAX_BOOK_BYTES = 64 * 1024 * 1024


def volume_score(path):
    return sum((path / name).is_dir() for name in LAYOUT)


def resolve_sd(explicit=None, allow_empty=False):
    if explicit:
        path = Path(explicit).expanduser().resolve()
        if not path.is_dir():
            raise RuntimeError(f"SD path is not a directory: {path}")
        if not os.access(path, os.W_OK):
            raise RuntimeError(f"SD path is not writable: {path}")
        return path

    volumes = Path("/Volumes")
    candidates = []
    if volumes.is_dir():
        for path in volumes.iterdir():
            if path.is_dir() and not path.name.startswith(".") and os.access(path, os.W_OK):
                score = volume_score(path)
                if score >= 2:
                    candidates.append((score, path))
    if len(candidates) == 1:
        return candidates[0][1].resolve()
    if not candidates and allow_empty:
        raise RuntimeError("Specify a mounted SD explicitly with --sd /Volumes/NAME")
    if not candidates:
        raise RuntimeError("No mounted ABVx SD detected; use --sd /Volumes/NAME")
    names = ", ".join(str(path) for _, path in sorted(candidates, reverse=True))
    raise RuntimeError(f"Multiple ABVx volumes detected: {names}; use --sd")


def ensure_layout(sd):
    for name in LAYOUT:
        (sd / name).mkdir(parents=True, exist_ok=True)


def human_size(value):
    amount = float(value)
    for unit in ("B", "KB", "MB", "GB", "TB"):
        if amount < 1024 or unit == "TB":
            return f"{amount:.0f}{unit}" if unit == "B" else f"{amount:.1f}{unit}"
        amount /= 1024


def visible_files(directory, suffix=None):
    if not directory.is_dir():
        return []
    return [path for path in directory.iterdir()
            if path.is_file() and not path.name.startswith(".") and
            (suffix is None or path.suffix.lower() == suffix)]


def print_status(sd):
    usage = shutil.disk_usage(sd)
    print("OK ABVX SD")
    print(f"path={sd}")
    print(f"total={human_size(usage.total)}")
    print(f"used={human_size(usage.used)}")
    print(f"free={human_size(usage.free)}")
    print(f"music={len(visible_files(sd / 'music', '.mp3'))}")
    print(f"books={len(visible_files(sd / 'books', '.txt'))}")
    print(f"notes={len(visible_files(sd / 'notes', '.txt'))}")


def sanitize_title(name):
    title = unicodedata.normalize("NFC", Path(name).stem)
    title = " ".join(title.replace("|", " ").split())
    return title[:160] or "Untitled"


def read_index(path):
    entries = {}
    if path.exists():
        for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
            if "|" in line:
                stored, title = line.split("|", 1)
                if stored and title:
                    entries[stored.upper()] = title
    return entries


def write_index(path, entries):
    temporary = path.with_suffix(".NEW")
    content = "".join(f"{name}|{entries[name]}\n" for name in sorted(entries))
    with temporary.open("w", encoding="utf-8", newline="\n") as output:
        output.write(content)
        output.flush()
        os.fsync(output.fileno())
    os.replace(temporary, path)


def next_numbered_name(directory, prefix, extension, limit=9999):
    width = 3 if prefix == "M" else 4
    for number in range(1, limit + 1):
        name = f"{prefix}{number:0{width}d}.{extension}"
        if not (directory / name).exists():
            return name
    raise RuntimeError(f"No free {prefix}xxx.{extension} names")


def atomic_copy(source, destination):
    if destination.exists():
        raise RuntimeError(f"destination exists: {destination.name}")
    temporary = destination.with_suffix(".TMP")
    try:
        temporary.unlink(missing_ok=True)
        with source.open("rb") as src, temporary.open("wb") as dst:
            shutil.copyfileobj(src, dst, length=1024 * 1024)
            dst.flush()
            os.fsync(dst.fileno())
        os.replace(temporary, destination)
    except Exception:
        temporary.unlink(missing_ok=True)
        raise


def has_mp3_sync(source):
    with source.open("rb") as stream:
        header = stream.read(10)
        offset = 0
        if len(header) == 10 and header[:3] == b"ID3":
            size = header[6:10]
            if all((byte & 0x80) == 0 for byte in size):
                offset = 10 + ((size[0] & 0x7F) << 21) + ((size[1] & 0x7F) << 14) + ((size[2] & 0x7F) << 7) + (size[3] & 0x7F)
        stream.seek(offset)
        data = stream.read(64 * 1024)
    return any(data[index] == 0xFF and (data[index + 1] & 0xE0) == 0xE0
               for index in range(max(0, len(data) - 1)))


def add_music(sd, sources):
    destination_dir = sd / "music"
    destination_dir.mkdir(parents=True, exist_ok=True)
    index_path = destination_dir / "INDEX.TXT"
    entries = read_index(index_path)
    known_titles = {title.casefold() for title in entries.values()}
    copied = 0
    for value in sources:
        source = Path(value).expanduser().resolve()
        if not source.is_file() or source.suffix.lower() != ".mp3":
            raise RuntimeError(f"not an MP3 file: {source}")
        if source.stat().st_size <= 0 or not has_mp3_sync(source):
            raise RuntimeError(f"MP3 validation failed: {source.name}")
        title = sanitize_title(source.name)
        if title.casefold() in known_titles:
            print(f"SKIP {source.name}: title already present")
            continue
        stored = next_numbered_name(destination_dir, "M", "MP3", 999)
        atomic_copy(source, destination_dir / stored)
        entries[stored] = title
        write_index(index_path, entries)
        known_titles.add(title.casefold())
        copied += 1
        print(f"ADD MUSIC {source.name} -> {stored} | {title}")
    print(f"OK MUSIC copied={copied}")


def decode_book(raw):
    if raw.startswith((b"\xff\xfe", b"\xfe\xff")):
        return raw.decode("utf-16"), "utf-16"
    try:
        return raw.decode("utf-8-sig"), "utf-8"
    except UnicodeDecodeError:
        cp1251 = raw.decode("cp1251")
        cyrillic = sum("\u0400" <= char <= "\u04ff" for char in cp1251)
        letters = sum(char.isalpha() for char in cp1251)
        if letters and cyrillic / letters >= 0.08:
            return cp1251, "cp1251"
        return raw.decode("cp1252"), "cp1252"


def write_book_index(directory, stored, title, encoding):
    path = directory / "BOOKS.IDX"
    entries = []
    if path.exists():
        entries = [line for line in path.read_text(encoding="utf-8", errors="replace").splitlines()
                   if line and not line.upper().startswith(stored.upper() + "|")]
    entries.append(f"{stored}|{title}|{encoding}")
    temporary = directory / "BOOKS.NEW"
    with temporary.open("w", encoding="utf-8", newline="\n") as output:
        output.write("\n".join(entries) + "\n")
        output.flush()
        os.fsync(output.fileno())
    os.replace(temporary, path)


def add_books(sd, sources):
    destination_dir = sd / "books"
    destination_dir.mkdir(parents=True, exist_ok=True)
    copied = 0
    for value in sources:
        source = Path(value).expanduser().resolve()
        if not source.is_file() or source.suffix.lower() != ".txt":
            raise RuntimeError(f"not a TXT file: {source}")
        size = source.stat().st_size
        if size <= 0 or size > MAX_BOOK_BYTES:
            raise RuntimeError(f"book size must be 1..{MAX_BOOK_BYTES} bytes: {source.name}")
        text, encoding = decode_book(source.read_bytes())
        text = unicodedata.normalize("NFC", text).replace("\r\n", "\n").replace("\r", "\n")
        if "\0" in text:
            raise RuntimeError(f"binary/NUL content in book: {source.name}")
        stored = next_numbered_name(destination_dir, "B", "TXT")
        destination = destination_dir / stored
        temporary = destination.with_suffix(".TMP")
        try:
            with temporary.open("w", encoding="utf-8", newline="\n") as output:
                output.write(text)
                output.flush()
                os.fsync(output.fileno())
            os.replace(temporary, destination)
        except Exception:
            temporary.unlink(missing_ok=True)
            raise
        title = sanitize_title(source.name)
        write_book_index(destination_dir, stored, title, encoding)
        copied += 1
        print(f"ADD BOOK {source.name} -> {stored} | {encoding} -> utf-8")
    print(f"OK BOOKS copied={copied}")


def sync_time(url):
    helper = Path(__file__).with_name("cardputer_time_sync.py")
    subprocess.run([sys.executable, str(helper), "--url", url, "sync"], check=True)


def main():
    parser = argparse.ArgumentParser(description="ABVx Mac Companion Core")
    parser.add_argument("--sd", help="mounted SD root, for example /Volumes/CARDPUTER")
    sub = parser.add_subparsers(dest="command", required=True)
    sub.add_parser("status", help="show mounted SD capacity and content counts")
    sub.add_parser("init", help="create the ABVx SD folder layout")
    music = sub.add_parser("add-music", help="validate and copy MP3 files")
    music.add_argument("files", nargs="+")
    books = sub.add_parser("add-book", help="normalize TXT books to UTF-8 and copy them")
    books.add_argument("files", nargs="+")
    clock = sub.add_parser("sync-time", help="sync Cardputer clock over its Connections AP")
    clock.add_argument("--url", default="http://192.168.4.1")
    args = parser.parse_args()

    if args.command == "sync-time":
        sync_time(args.url)
        return
    sd = resolve_sd(args.sd, allow_empty=args.command == "init")
    if args.command == "status":
        print_status(sd)
    elif args.command == "init":
        ensure_layout(sd)
        print(f"OK INIT\npath={sd}")
    elif args.command == "add-music":
        add_music(sd, args.files)
    elif args.command == "add-book":
        add_books(sd, args.files)


if __name__ == "__main__":
    try:
        main()
    except subprocess.CalledProcessError as exc:
        print(f"ERROR: time sync exited with {exc.returncode}", file=sys.stderr)
        sys.exit(exc.returncode or 1)
    except Exception as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        sys.exit(1)
