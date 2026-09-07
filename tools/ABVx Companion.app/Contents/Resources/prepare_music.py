#!/usr/bin/env python3
"""Prepare an ABVx music directory without FAT long-filename dependency."""

import argparse
import os
import re
import shutil
import sys
import unicodedata
from pathlib import Path

CYRILLIC = {
    'а':'a','б':'b','в':'v','г':'g','ґ':'g','д':'d','е':'e','ё':'yo','є':'ye','ж':'zh','з':'z',
    'и':'i','і':'i','ї':'yi','й':'y','к':'k','л':'l','м':'m','н':'n','о':'o','п':'p','р':'r',
    'с':'s','т':'t','у':'u','ф':'f','х':'kh','ц':'ts','ч':'ch','ш':'sh','щ':'sch','ъ':'','ы':'y',
    'ь':'','э':'e','ю':'yu','я':'ya',
}
HEBREW = {
    'א':'a','ב':'b','ג':'g','ד':'d','ה':'h','ו':'v','ז':'z','ח':'kh','ט':'t','י':'y','כ':'k','ך':'k',
    'ל':'l','מ':'m','ם':'m','נ':'n','ן':'n','ס':'s','ע':'a','פ':'p','ף':'p','צ':'ts','ץ':'ts',
    'ק':'k','ר':'r','ש':'sh','ת':'t',
}


def safe_83(name):
    if not name or len(name) > 12 or name.count('.') > 1:
        return False
    base, ext = name.split('.', 1) if '.' in name else (name, '')
    return bool(base) and len(base) <= 8 and len(ext) <= 3 and all(
        c.isascii() and (c.isalnum() or c in '_-') for c in base + ext
    )


def display_title(filename):
    out = []
    for char in Path(filename).stem:
        mapped = CYRILLIC.get(char.lower(), HEBREW.get(char))
        if mapped is not None:
            out.append(mapped.upper() if char.isupper() else mapped)
            continue
        normalized = unicodedata.normalize('NFKD', char)
        ascii_part = normalized.encode('ascii', 'ignore').decode('ascii')
        out.append(ascii_part if ascii_part else ' ')
    return (re.sub(r'\s+', ' ', ''.join(out).replace('|', ' ')).strip()[:120]
            or 'Untitled track')


def mp3_files(directory):
    return sorted((p for p in directory.iterdir()
                   if p.is_file() and p.suffix.lower() == '.mp3' and not p.name.startswith('.')),
                  key=lambda p: p.name.casefold())


def load_index(directory):
    entries = {}
    path = directory / 'INDEX.TXT'
    if path.exists():
        for line in path.read_text(encoding='utf-8', errors='replace').splitlines():
            if '|' in line:
                stored, title = line.split('|', 1)
                if stored and title:
                    entries[stored.upper()] = title
    return entries


def write_index(directory, entries):
    temporary = directory / 'INDEX.NEW'
    lines = [f'{name}|{entries[name]}' for name in sorted(entries)]
    temporary.write_text('\n'.join(lines) + ('\n' if lines else ''), encoding='utf-8')
    os.replace(temporary, directory / 'INDEX.TXT')


def next_storage_name(directory, reserved):
    for number in range(1, 1000):
        name = f'M{number:03d}.MP3'
        if name not in reserved and not (directory / name).exists():
            reserved.add(name)
            return name
    raise RuntimeError('No free Mxxx.MP3 names')


def normalize_in_place(directory):
    entries = load_index(directory)
    reserved = {p.name.upper() for p in mp3_files(directory)}
    changed = 0
    for source in mp3_files(directory):
        if safe_83(source.name) and source.name.isascii():
            continue
        stored = next_storage_name(directory, reserved)
        title = display_title(source.name)
        source.rename(directory / stored)
        entries[stored] = title
        print(f'{source.name} -> {stored} | {title}')
        changed += 1
    write_index(directory, entries)
    return changed


def copy_library(source_dir, destination):
    entries = load_index(destination)
    reserved = {p.name.upper() for p in mp3_files(destination)}
    known_titles = set(entries.values())
    copied = 0
    for source in mp3_files(source_dir):
        title = display_title(source.name)
        if title in known_titles:
            print(f'SKIP {source.name}: title already indexed')
            continue
        stored = next_storage_name(destination, reserved)
        temporary = destination / f'{Path(stored).stem}.TMP'
        shutil.copyfile(source, temporary)
        os.replace(temporary, destination / stored)
        entries[stored] = title
        known_titles.add(title)
        print(f'{source.name} -> {stored} | {title}')
        copied += 1
    write_index(destination, entries)
    return copied


def main():
    parser = argparse.ArgumentParser(description='Prepare MP3 files for ABVx Music')
    parser.add_argument('--in-place', action='store_true')
    parser.add_argument('source', help='Source folder, or music folder with --in-place')
    parser.add_argument('destination', nargs='?', help='Mounted SD music folder')
    args = parser.parse_args()
    source = Path(args.source).expanduser().resolve()
    if not source.is_dir():
        raise SystemExit('ERROR: source directory not found')
    if args.in_place:
        if args.destination:
            raise SystemExit('ERROR: destination is not used with --in-place')
        print(f'OK normalized={normalize_in_place(source)}')
        return
    if not args.destination:
        raise SystemExit('ERROR: destination directory is required')
    destination = Path(args.destination).expanduser().resolve()
    destination.mkdir(parents=True, exist_ok=True)
    if source == destination:
        raise SystemExit('ERROR: use --in-place for one directory')
    print(f'OK copied={copy_library(source, destination)}')


if __name__ == '__main__':
    try:
        main()
    except Exception as exc:
        print(f'ERROR: {exc}', file=sys.stderr)
        sys.exit(1)
