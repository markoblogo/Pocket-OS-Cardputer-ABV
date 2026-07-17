#!/usr/bin/env python3
"""Synchronize ABVx Pocket OS clock through the Connections AP."""

import argparse
import datetime
import sys
import time
import urllib.error
import urllib.parse
import urllib.request


def request(url, data=None, timeout=10):
    method = "POST" if data is not None else "GET"
    req = urllib.request.Request(url, data=data, method=method)
    try:
        with urllib.request.urlopen(req, timeout=timeout) as response:
            return response.read(8192).decode("utf-8", errors="replace")
    except urllib.error.HTTPError as exc:
        body = exc.read(8192).decode("utf-8", errors="replace").strip()
        raise RuntimeError(f"HTTP {exc.code}: {body}") from exc


def fields(text):
    result = {}
    for line in text.splitlines():
        if "=" in line:
            key, value = line.split("=", 1)
            result[key.strip()] = value.strip()
    return result


def endpoint(base, name, **query):
    encoded = urllib.parse.urlencode(query)
    return f"{base.rstrip('/')}/api/{name}" + (f"?{encoded}" if encoded else "")


def local_offset_minutes(epoch):
    instant = datetime.datetime.fromtimestamp(epoch, datetime.timezone.utc).astimezone()
    offset = instant.utcoffset() or datetime.timedelta()
    return int(offset.total_seconds() // 60)


def sync(args):
    epoch = args.epoch if args.epoch is not None else int(time.time())
    offset = args.offset if args.offset is not None else local_offset_minutes(epoch)
    reply = request(endpoint(args.url, "time-sync", epoch=epoch, offset=offset), data=b"", timeout=args.timeout)
    if not reply.startswith("OK TIME QUEUED"):
        raise RuntimeError(f"unexpected response: {reply.strip()}")
    print(reply.strip())
    for _ in range(10):
        time.sleep(0.15)
        status_text = request(endpoint(args.url, "status"), timeout=args.timeout)
        status = fields(status_text)
        if status.get("time") == "APPLIED" and status.get("epoch") == str(epoch):
            print(f"OK TIME APPLIED\nclock={status.get('clock', '?')}\noffset={offset}")
            return
    raise RuntimeError("Cardputer did not confirm time application")


def main():
    parser = argparse.ArgumentParser(description="ABVx Connections clock sync")
    parser.add_argument("--url", default="http://192.168.4.1")
    parser.add_argument("--timeout", type=float, default=10)
    sub = parser.add_subparsers(dest="command", required=True)
    sync_parser = sub.add_parser("sync", help="send the Mac's current time")
    sync_parser.add_argument("--epoch", type=int, help="override Unix timestamp")
    sync_parser.add_argument("--offset", type=int, help="override UTC offset in minutes")
    sub.add_parser("status", help="show Connections and clock status")
    args = parser.parse_args()
    if args.command == "sync":
        sync(args)
    else:
        print(request(endpoint(args.url, "status"), timeout=args.timeout).strip())


if __name__ == "__main__":
    try:
        main()
    except Exception as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        sys.exit(1)
