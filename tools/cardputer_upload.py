#!/usr/bin/env python3
import argparse
import os
import re
import sys
import urllib.parse
import urllib.error
import urllib.request

SAFE_UPLOAD_LIMIT = 64 * 1024
MAX_RESPONSE_BYTES = 16 * 1024


def is_safe_83(path):
    name = path.rstrip('/').rsplit('/', 1)[-1]
    if not name or name in ('.', '..') or len(name) > 12:
        return False
    if name.count('.') > 1:
        return False
    if '.' in name:
        base, ext = name.split('.', 1)
    else:
        base, ext = name, ''
    if not base or len(base) > 8 or len(ext) > 3:
        return False
    return re.fullmatch(r'[A-Za-z0-9_-]+', base) is not None and (not ext or re.fullmatch(r'[A-Za-z0-9_-]+', ext) is not None)


def is_music_mp3(path):
    return path.lower().startswith('/music/') and path.lower().endswith('.mp3')


def safe_terminal(text):
    return ''.join(c if c in '\n\t' or 32 <= ord(c) < 127 else '?' for c in text)


def post(url, data=b'', timeout=20):
    req = urllib.request.Request(url, data=data, method='POST')
    with urllib.request.urlopen(req, timeout=timeout) as r:
        body = r.read(MAX_RESPONSE_BYTES + 1)
        if len(body) > MAX_RESPONSE_BYTES:
            raise RuntimeError('response too large')
        return safe_terminal(body.decode('utf-8', errors='replace'))


def main():
    ap = argparse.ArgumentParser(description='ABVx Cardputer chunk uploader')
    ap.add_argument('--url', default='http://192.168.4.1')
    ap.add_argument('local_file')
    ap.add_argument('sd_path', help='Example: /music/T05.MP3 or /books/B1.TXT')
    args = ap.parse_args()

    if not is_safe_83(args.sd_path) and not is_music_mp3(args.sd_path):
        raise SystemExit('ERROR: target filename must be FAT 8.3-safe outside /music/*.mp3')

    size = os.path.getsize(args.local_file)
    if size > SAFE_UPLOAD_LIMIT:
        raise SystemExit(
            'ERROR: Wi-Fi upload is limited to 64KB for now. '
            'Large MP3 upload is unstable on Cardputer ADV; use SD reader.'
        )
    qpath = urllib.parse.quote(args.sd_path, safe='/._-')
    base = args.url.rstrip('/')
    try:
        with open(args.local_file, 'rb') as f:
            payload = f.read(SAFE_UPLOAD_LIMIT + 1)
        if len(payload) != size:
            raise RuntimeError('local file changed while reading')
        print(post(f'{base}/api/upload?path={qpath}', payload, timeout=30).strip())
    except urllib.error.HTTPError as e:
        body = e.read(MAX_RESPONSE_BYTES + 1)
        if len(body) > MAX_RESPONSE_BYTES:
            message = 'response too large'
        else:
            message = safe_terminal(body.decode('utf-8', errors='replace')).strip()
        raise RuntimeError(f'HTTP {e.code}: {message}') from e


if __name__ == '__main__':
    try:
        main()
    except Exception as e:
        print(f'ERROR: {e}', file=sys.stderr)
        sys.exit(1)
