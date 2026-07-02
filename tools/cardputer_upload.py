#!/usr/bin/env python3
import argparse
import os
import sys
import urllib.parse
import urllib.request

CHUNK = 8192


def post(url, data=b''):
    req = urllib.request.Request(url, data=data, method='POST')
    with urllib.request.urlopen(req, timeout=30) as r:
        return r.read().decode('utf-8', errors='replace')


def main():
    ap = argparse.ArgumentParser(description='ABVx Cardputer chunk uploader')
    ap.add_argument('--url', default='http://192.168.4.1')
    ap.add_argument('--chunk', type=int, default=CHUNK)
    ap.add_argument('local_file')
    ap.add_argument('sd_path', help='Example: /music/T05.MP3 or /books/B1.TXT')
    args = ap.parse_args()

    size = os.path.getsize(args.local_file)
    qpath = urllib.parse.quote(args.sd_path, safe='/._-')
    base = args.url.rstrip('/')

    print(post(f'{base}/api/upload-begin?path={qpath}&size={size}').strip())
    sent = 0
    with open(args.local_file, 'rb') as f:
        while True:
            chunk = f.read(args.chunk)
            if not chunk:
                break
            url = f'{base}/api/upload-chunk?path={qpath}&offset={sent}&total={size}'
            post(url, chunk)
            sent += len(chunk)
            pct = int(sent * 100 / size) if size else 100
            print(f'uploaded {sent}/{size} {pct}%', end='\r', flush=True)
    print()
    print(post(f'{base}/api/upload-finish?path={qpath}&size={size}').strip())


if __name__ == '__main__':
    try:
        main()
    except Exception as e:
        print(f'ERROR: {e}', file=sys.stderr)
        sys.exit(1)
