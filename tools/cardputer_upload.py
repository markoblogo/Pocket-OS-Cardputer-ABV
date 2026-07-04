#!/usr/bin/env python3
import argparse
import os
import re
import sys
import time
import urllib.parse
import urllib.error
import urllib.request

CHUNK = 1024


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


def post(url, data=b'', timeout=20):
    req = urllib.request.Request(url, data=data, method='POST')
    with urllib.request.urlopen(req, timeout=timeout) as r:
        return r.read().decode('utf-8', errors='replace')


def post_retry(url, data=b'', tries=3, delay=0.25, timeout=20):
    last = None
    for attempt in range(tries):
        try:
            return post(url, data, timeout)
        except urllib.error.HTTPError:
            raise
        except Exception as e:
            last = e
            if attempt + 1 < tries:
                time.sleep(delay)
    raise last


def main():
    ap = argparse.ArgumentParser(description='ABVx Cardputer chunk uploader')
    ap.add_argument('--url', default='http://192.168.4.1')
    ap.add_argument('--chunk', type=int, default=CHUNK)
    ap.add_argument('--retries', type=int, default=3)
    ap.add_argument('--delay', type=float, default=0.2, help='Delay between chunks in seconds')
    ap.add_argument('local_file')
    ap.add_argument('sd_path', help='Example: /music/T05.MP3 or /books/B1.TXT')
    args = ap.parse_args()

    if args.chunk <= 0 or args.chunk > 2048:
        raise SystemExit('ERROR: chunk must be 1..2048 bytes for current firmware')
    if not is_safe_83(args.sd_path):
        raise SystemExit('ERROR: target filename must be FAT 8.3-safe, e.g. /music/T06.MP3')

    size = os.path.getsize(args.local_file)
    qpath = urllib.parse.quote(args.sd_path, safe='/._-')
    base = args.url.rstrip('/')
    begun = False

    try:
        print(post_retry(f'{base}/api/upload-begin?path={qpath}&size={size}', tries=args.retries).strip())
        begun = True
        sent = 0
        with open(args.local_file, 'rb') as f:
            while True:
                chunk = f.read(args.chunk)
                if not chunk:
                    break
                url = f'{base}/api/upload-chunk?path={qpath}&offset={sent}&total={size}'
                post_retry(url, chunk, tries=args.retries, delay=args.delay)
                sent += len(chunk)
                pct = int(sent * 100 / size) if size else 100
                print(f'uploaded {sent}/{size} {pct}%', end='\r', flush=True)
                time.sleep(args.delay)
        print()
        print(post_retry(f'{base}/api/upload-finish?path={qpath}&size={size}', tries=args.retries).strip())
    except urllib.error.HTTPError as e:
        body = e.read().decode('utf-8', errors='replace')
        raise RuntimeError(f'HTTP {e.code}: {body.strip()}') from e
    except Exception:
        if begun:
            try:
                post(f'{base}/api/upload-abort?path={qpath}', timeout=8)
                print('\naborted partial upload', file=sys.stderr)
            except Exception as abort_error:
                print(f'\nabort failed: {abort_error}', file=sys.stderr)
        raise


if __name__ == '__main__':
    try:
        main()
    except Exception as e:
        print(f'ERROR: {e}', file=sys.stderr)
        sys.exit(1)
