#!/usr/bin/env python3
"""ABVx Mac Companion Core for direct SD preparation and clock sync."""

import argparse
import html.parser
import os
import posixpath
import shutil
import subprocess
import sys
import unicodedata
import urllib.parse
import xml.etree.ElementTree as ET
import zipfile
from pathlib import Path


LAYOUT = ("music", "books", "notes", "CARDPTR")
MAX_BOOK_BYTES = 64 * 1024 * 1024
MAX_BOOK_TEXT_CHARS = 16 * 1024 * 1024
MAX_EPUB_MEMBER_BYTES = 8 * 1024 * 1024


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


def local_name(tag):
    return tag.rsplit("}", 1)[-1] if "}" in tag else tag


def element_text(element):
    if element is None:
        return ""
    return " ".join("".join(element.itertext()).split())


class EpubTextParser(html.parser.HTMLParser):
    BLOCK_TAGS = {"address", "article", "blockquote", "br", "div", "h1", "h2", "h3",
                  "h4", "h5", "h6", "hr", "li", "p", "pre", "section", "title"}
    SKIP_TAGS = {"script", "style", "svg"}

    def __init__(self):
        super().__init__(convert_charrefs=True)
        self.parts = []
        self.skip_depth = 0

    def handle_starttag(self, tag, attrs):
        tag = tag.lower()
        if tag in self.SKIP_TAGS:
            self.skip_depth += 1
        elif not self.skip_depth and tag in self.BLOCK_TAGS:
            self.parts.append("\n")

    def handle_endtag(self, tag):
        tag = tag.lower()
        if tag in self.SKIP_TAGS and self.skip_depth:
            self.skip_depth -= 1
        elif not self.skip_depth and tag in self.BLOCK_TAGS:
            self.parts.append("\n")

    def handle_data(self, data):
        if not self.skip_depth:
            self.parts.append(data)

    def text(self):
        lines = []
        for line in "".join(self.parts).splitlines():
            clean = " ".join(line.split())
            if clean and (not lines or clean != lines[-1]):
                lines.append(clean)
        return "\n\n".join(lines)


def safe_zip_read(archive, name):
    # EPUB manifest hrefs may include a query or fragment; ZIP members do not.
    member_path = urllib.parse.urlsplit(name).path
    normalized = posixpath.normpath(urllib.parse.unquote(member_path)).lstrip("/")
    if normalized == ".." or normalized.startswith("../"):
        raise RuntimeError("EPUB contains an unsafe path")
    try:
        info = archive.getinfo(normalized)
    except KeyError as exc:
        raise RuntimeError(f"EPUB member missing: {normalized}") from exc
    if info.file_size > MAX_EPUB_MEMBER_BYTES:
        raise RuntimeError(f"EPUB member too large: {normalized}")
    return archive.read(info)


def first_by_local_name(root, name):
    return next((element for element in root.iter() if local_name(element.tag) == name), None)


def epub_metadata_and_chapters(source):
    try:
        archive = zipfile.ZipFile(source)
    except (OSError, zipfile.BadZipFile) as exc:
        raise RuntimeError(f"invalid EPUB: {exc}") from exc
    with archive:
        try:
            container = ET.fromstring(safe_zip_read(archive, "META-INF/container.xml"))
            rootfile = first_by_local_name(container, "rootfile")
            opf_name = rootfile.attrib.get("full-path", "") if rootfile is not None else ""
            if not opf_name:
                raise RuntimeError("EPUB package path missing")
            package = ET.fromstring(safe_zip_read(archive, opf_name))
        except ET.ParseError as exc:
            raise RuntimeError(f"invalid EPUB XML: {exc}") from exc

        title = element_text(first_by_local_name(package, "title")) or sanitize_title(source.name)
        creators = [element_text(element) for element in package.iter()
                    if local_name(element.tag) == "creator" and element_text(element)]
        author = ", ".join(creators)
        manifest = {}
        for element in package.iter():
            if local_name(element.tag) == "item":
                item_id = element.attrib.get("id", "")
                href = element.attrib.get("href", "")
                media = element.attrib.get("media-type", "")
                if item_id and href:
                    manifest[item_id] = (href, media)
        spine = [element.attrib.get("idref", "") for element in package.iter()
                 if local_name(element.tag) == "itemref"]
        opf_dir = posixpath.dirname(opf_name)
        chapters = []
        total_chars = 0
        for item_id in spine:
            item = manifest.get(item_id)
            if not item or item[1] not in ("application/xhtml+xml", "text/html", ""):
                continue
            member = posixpath.normpath(posixpath.join(opf_dir, urllib.parse.unquote(item[0])))
            raw = safe_zip_read(archive, member)
            parser = EpubTextParser()
            try:
                parser.feed(raw.decode("utf-8-sig", errors="replace"))
                parser.close()
            except Exception as exc:
                raise RuntimeError(f"EPUB chapter parse failed: {member}: {exc}") from exc
            text = parser.text()
            if not text:
                continue
            total_chars += len(text)
            if total_chars > MAX_BOOK_TEXT_CHARS:
                raise RuntimeError("EPUB text exceeds Companion limit")
            chapters.append(text)
        if not chapters:
            raise RuntimeError("EPUB has no readable spine chapters; encrypted EPUB is unsupported")
        return title, author, chapters


def fb2_author(root):
    title_info = first_by_local_name(root, "title-info")
    if title_info is None:
        return ""
    author_element = next((child for child in title_info if local_name(child.tag) == "author"), None)
    if author_element is None:
        return ""
    parts = []
    for wanted in ("first-name", "middle-name", "last-name", "nickname"):
        value = next((element_text(child) for child in author_element
                      if local_name(child.tag) == wanted and element_text(child)), "")
        if value:
            parts.append(value)
    return " ".join(parts)


def fb2_section_text(section):
    paragraphs = []
    for child in section:
        name = local_name(child.tag)
        if name == "section":
            continue
        if name == "title":
            continue
        if name in ("p", "subtitle"):
            value = element_text(child)
            if value:
                paragraphs.append(value)
        elif name in ("epigraph", "cite", "poem"):
            for element in child.iter():
                if local_name(element.tag) in ("p", "v", "subtitle"):
                    value = element_text(element)
                    if value:
                        paragraphs.append(value)
    return "\n\n".join(paragraphs)


def collect_fb2_sections(section, output):
    title = element_text(next((child for child in section if local_name(child.tag) == "title"), None))
    body = fb2_section_text(section)
    if body:
        output.append((title, body))
    for child in section:
        if local_name(child.tag) == "section":
            collect_fb2_sections(child, output)


def fb2_metadata_and_chapters(source):
    raw = source.read_bytes()
    try:
        root = ET.fromstring(raw)
    except ET.ParseError as exc:
        raise RuntimeError(f"invalid FB2 XML: {exc}") from exc
    book_title = element_text(first_by_local_name(root, "book-title")) or sanitize_title(source.name)
    author = fb2_author(root)
    chapters = []
    for body in (element for element in root.iter() if local_name(element.tag) == "body"):
        sections = [child for child in body if local_name(child.tag) == "section"]
        if sections:
            structured = []
            for section in sections:
                collect_fb2_sections(section, structured)
            for title, text in structured:
                chapters.append(f"{title}\n\n{text}" if title else text)
        else:
            text = "\n\n".join(element_text(element) for element in body.iter()
                                  if local_name(element.tag) == "p" and element_text(element))
            if text:
                chapters.append(text)
    if not chapters:
        raise RuntimeError("FB2 has no readable body")
    if sum(len(chapter) for chapter in chapters) > MAX_BOOK_TEXT_CHARS:
        raise RuntimeError("FB2 text exceeds Companion limit")
    return book_title, author, chapters


def format_prepared_book(title, author, chapters):
    output = [title]
    if author:
        output.append(author)
    output.append("")
    for index, chapter in enumerate(chapters, 1):
        output.extend((f"=== CHAPTER {index} ===", "", chapter.strip(), ""))
    return unicodedata.normalize("NFC", "\n".join(output)).replace("\r\n", "\n").replace("\r", "\n")


def convert_book(source):
    suffix = source.suffix.lower()
    if suffix == ".txt":
        text, encoding = decode_book(source.read_bytes())
        return (unicodedata.normalize("NFC", text).replace("\r\n", "\n").replace("\r", "\n"),
                sanitize_title(source.name), "", encoding, 0)
    if suffix == ".epub":
        title, author, chapters = epub_metadata_and_chapters(source)
        return format_prepared_book(title, author, chapters), title, author, "epub", len(chapters)
    if suffix == ".fb2":
        title, author, chapters = fb2_metadata_and_chapters(source)
        return format_prepared_book(title, author, chapters), title, author, "fb2", len(chapters)
    raise RuntimeError(f"unsupported book format: {source.suffix or 'none'}")


def write_book_index(directory, stored, title, source_format, author):
    path = directory / "BOOKS.IDX"
    entries = []
    if path.exists():
        entries = [line for line in path.read_text(encoding="utf-8", errors="replace").splitlines()
                   if line and not line.upper().startswith(stored.upper() + "|")]
    clean_author = " ".join(author.replace("|", " ").split())[:160]
    entries.append(f"{stored}|{sanitize_title(title)}|{source_format}|{clean_author}")
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
        if not source.is_file() or source.suffix.lower() not in (".txt", ".epub", ".fb2"):
            raise RuntimeError(f"not a TXT/EPUB/FB2 file: {source}")
        size = source.stat().st_size
        if size <= 0 or size > MAX_BOOK_BYTES:
            raise RuntimeError(f"book size must be 1..{MAX_BOOK_BYTES} bytes: {source.name}")
        text, title, author, source_format, chapters = convert_book(source)
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
        write_book_index(destination_dir, stored, title, source_format, author)
        copied += 1
        print(f"ADD BOOK {source.name} -> {stored} | {source_format} -> utf-8 | chapters={chapters}")
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
    books = sub.add_parser("add-book", help="convert TXT/EPUB/FB2 books to Reader UTF-8 TXT")
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
