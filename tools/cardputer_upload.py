#!/usr/bin/env python3
import argparse
import os
import re
import sys
import time
import urllib.error
import urllib.parse
import urllib.request

MAX_UPLOAD_BYTES = 32 * 1024 * 1024
DEFAULT_CHUNK = 2048
MAX_RESPONSE_BYTES = 16 * 1024


def is_safe_83(path):
    name = path.rstrip('/').rsplit('/', 1)[-1]
    if not name or name in ('.', '..') or len(name) > 12 or name.count('.') > 1:
        return False
    base, ext = name.split('.', 1) if '.' in name else (name, '')
    if not base or len(base) > 8 or len(ext) > 3:
        return False
    return re.fullmatch(r'[A-Za-z0-9_-]+', base) is not None and (
        not ext or re.fullmatch(r'[A-Za-z0-9_-]+', ext) is not None
    )


def is_music_mp3(path):
    return path.lower().startswith('/music/') and path.lower().endswith('.mp3')


def safe_terminal(text):
    return ''.join(c if c in '\n\t' or 32 <= ord(c) < 127 else '?' for c in text)


def request(url, data=None, timeout=30):
    method = 'POST' if data is not None else 'GET'
    req = urllib.request.Request(url, data=data, method=method)
    try:
        with urllib.request.urlopen(req, timeout=timeout) as response:
            body = response.read(MAX_RESPONSE_BYTES + 1)
    except urllib.error.HTTPError as exc:
        body = exc.read(MAX_RESPONSE_BYTES + 1)
        message = safe_terminal(body[:MAX_RESPONSE_BYTES].decode('utf-8', errors='replace')).strip()
        raise RuntimeError(f'HTTP {exc.code}: {message}') from exc
    if len(body) > MAX_RESPONSE_BYTES:
        raise RuntimeError('response too large')
    return safe_terminal(body.decode('utf-8', errors='replace'))


def parse_fields(text):
    fields = {}
    for line in text.splitlines():
        if '=' in line:
            key, value = line.split('=', 1)
            fields[key.strip()] = value.strip()
    return fields


def endpoint(base, name, **query):
    encoded = urllib.parse.urlencode(query, safe='/._-')
    return f'{base}/api/{name}' + (f'?{encoded}' if encoded else '')


def status(base, timeout):
    return parse_fields(request(endpoint(base, 'status'), timeout=timeout))


def send_chunk(base, stored, total, offset, payload, timeout, retries):
    expected = offset + len(payload)
    url = endpoint(base, 'upload-chunk', path=stored, offset=offset, total=total)
    last_error = None
    for attempt in range(retries + 1):
        try:
            reply = request(url, data=payload, timeout=timeout)
            fields = parse_fields(reply)
            done = int(fields.get('done', '-1'))
            if done != expected:
                raise RuntimeError(f'chunk acknowledgement mismatch: {done} != {expected}')
            return
        except Exception as exc:
            last_error = exc
            try:
                remote_done = int(status(base, timeout).get('done', '-1'))
                if remote_done == expected:
                    return
                if remote_done != offset:
                    raise RuntimeError(f'remote offset {remote_done}, expected {offset}') from exc
            except Exception as status_exc:
                last_error = status_exc
            if attempt < retries:
                time.sleep(0.4 * (attempt + 1))
    raise RuntimeError(f'chunk at {offset} failed: {last_error}')


def abort(base, stored, timeout):
    if not stored:
        return
    try:
        request(endpoint(base, 'upload-abort', path=stored), data=b'', timeout=timeout)
    except Exception as exc:
        print(f'WARN: abort failed: {exc}', file=sys.stderr)


def main():
    parser = argparse.ArgumentParser(description='ABVx Connections v3 staged uploader')
    parser.add_argument('--url', default='http://192.168.4.1')
    parser.add_argument('--chunk', type=int, default=DEFAULT_CHUNK)
    parser.add_argument('--delay', type=float, default=0.02)
    parser.add_argument('--timeout', type=float, default=30)
    parser.add_argument('--retries', type=int, default=3)
    parser.add_argument('local_file')
    parser.add_argument('sd_path', help='Example: /music/track.mp3 or /books/B1.TXT')
    args = parser.parse_args()

    if not os.path.isfile(args.local_file):
        raise SystemExit('ERROR: local file not found')
    if not is_safe_83(args.sd_path) and not is_music_mp3(args.sd_path):
        raise SystemExit('ERROR: target filename must be FAT 8.3-safe outside /music/*.mp3')
    if args.chunk < 128 or args.chunk > DEFAULT_CHUNK:
        raise SystemExit(f'ERROR: --chunk must be 128..{DEFAULT_CHUNK}')
    if args.retries < 0 or args.retries > 10:
        raise SystemExit('ERROR: --retries must be 0..10')

    size = os.path.getsize(args.local_file)
    if size <= 0 or size > MAX_UPLOAD_BYTES:
        raise SystemExit(f'ERROR: file size must be 1..{MAX_UPLOAD_BYTES} bytes')

    base = args.url.rstrip('/')
    stored = ''
    finished = False
    try:
        begin = request(endpoint(base, 'upload-begin', path=args.sd_path, size=size), data=b'', timeout=args.timeout)
        fields = parse_fields(begin)
        stored = fields.get('stored', '')
        if not begin.startswith('OK BEGIN') or not stored:
            raise RuntimeError(f'invalid begin response: {begin.strip()}')
        print(begin.strip())

        done = 0
        with open(args.local_file, 'rb') as source:
            while done < size:
                payload = source.read(min(args.chunk, size - done))
                if not payload:
                    raise RuntimeError('local file changed while reading')
                send_chunk(base, stored, size, done, payload, args.timeout, args.retries)
                done += len(payload)
                percent = done * 100 // size
                print(f'\r{percent:3d}%  {done}/{size}', end='', file=sys.stderr, flush=True)
                if args.delay > 0:
                    time.sleep(args.delay)
        print(file=sys.stderr)

        finish = request(endpoint(base, 'upload-finish', path=stored, size=size), data=b'', timeout=args.timeout)
        if not finish.startswith('OK FINISH'):
            raise RuntimeError(f'invalid finish response: {finish.strip()}')
        finished = True
        print(finish.strip())
        print(f'stored={stored}\nsize={size}')
    finally:
        if stored and not finished:
            abort(base, stored, args.timeout)


if __name__ == '__main__':
    try:
        main()
    except Exception as exc:
        print(f'ERROR: {exc}', file=sys.stderr)
        sys.exit(1)
