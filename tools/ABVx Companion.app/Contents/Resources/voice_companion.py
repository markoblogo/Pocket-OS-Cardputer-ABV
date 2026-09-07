#!/usr/bin/env python3
"""Verified internal Voice offload and optional, local-only batch transcription."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import tempfile
import urllib.parse
import urllib.request

import abvx_companion as core

DEVICE_URL = os.environ.get("ABVX_DEVICE_URL", "http://192.168.4.1")
MODEL = "mlx-community/parakeet-tdt-0.6b-v3"
MAX_VOICE_BYTES = 2 * 1024 * 1024


def device_url(url):
    parsed = urllib.parse.urlsplit(url)
    if parsed.scheme != "http" or parsed.hostname not in ("192.168.4.1", "127.0.0.1", "localhost") or parsed.username or parsed.password or parsed.query or parsed.fragment or parsed.path not in ("", "/"):
        raise RuntimeError("Voice device URL must be the Cardputer AP or localhost")
    return url.rstrip("/")


def fetch(url, limit):
    # Never follow a device-controlled redirect out of the local connection.
    class NoRedirect(urllib.request.HTTPRedirectHandler):
        def redirect_request(self, *args, **kwargs):
            return None
    opener = urllib.request.build_opener(urllib.request.ProxyHandler({}), NoRedirect())
    with opener.open(url, timeout=20) as response:
        data = response.read(limit + 1)
    if len(data) > limit:
        raise RuntimeError("Device response exceeds limit")
    return data


def inventory(url):
    payload = json.loads(fetch(device_url(url) + "/api/voice/list", 256 * 1024))
    if payload.get("version") != 1 or not re.fullmatch(r"[0-9a-f]{12}", payload.get("device_id", "")):
        raise RuntimeError("Unsupported Voice inventory")
    files = payload.get("files")
    if not isinstance(files, list) or len(files) > 4096:
        raise RuntimeError("Invalid Voice inventory")
    seen = set()
    for item in files:
        name, size, digest = item.get("name", ""), item.get("size"), item.get("sha256", "")
        if not re.fullmatch(r"REC[0-9]{5}\.WAV", name) or name in seen or type(size) is not int or not 44 <= size <= MAX_VOICE_BYTES or not re.fullmatch(r"[0-9a-f]{64}", digest):
            raise RuntimeError("Invalid recording metadata; backup not acknowledged")
        seen.add(name)
    return payload


def offload(destination, url=DEVICE_URL):
    data = inventory(url)
    destination = Path(destination) / data["device_id"]
    destination.mkdir(parents=True, exist_ok=True)
    copied = 0
    for item in data["files"]:
        target = destination / (item["sha256"] + ".wav")
        if target.exists() and target.stat().st_size == item["size"] and core.file_sha256(target).hex() == item["sha256"]:
            continue
        payload = fetch(device_url(url) + "/api/voice/download?name=" + item["name"], MAX_VOICE_BYTES)
        if len(payload) != item["size"] or hashlib.sha256(payload).hexdigest() != item["sha256"]:
            raise RuntimeError(f"Voice verification failed: {item['name']}")
        if payload[:4] != b"RIFF" or payload[8:12] != b"WAVE":
            raise RuntimeError("Invalid WAV; original retained on device")
        core.atomic_replace_bytes(target, payload)
        if core.file_sha256(target).hex() != item["sha256"]:
            raise RuntimeError("Local Voice verification failed")
        copied += 1
    receipt = {**data, "state": "transferred", "copied": copied, "originals_retained": True}
    core.atomic_replace_text(destination / "offload.json", json.dumps(receipt, indent=2))
    print(f"VOICE verified={len(data['files'])} copied={copied} device={data['device_id']}", flush=True)
    return receipt


def transcribe(destination, model=None):
    """Runs serially in a dedicated process; failures keep WAV and retry state."""
    files = sorted(Path(destination).glob("*/*.wav"))
    if not files:
        raise RuntimeError("No offloaded Voice WAV files; run Sync Voice first")
    pending = [p for p in files if not p.with_suffix(".md").exists()]
    if not pending:
        return {"transcribed": 0, "pending": 0}
    if model is None:
        from parakeet_mlx import from_pretrained
        model = from_pretrained(MODEL)
    done, failures = 0, []
    for source in pending:
        state = source.with_suffix(".json")
        try:
            if source.stat().st_size > MAX_VOICE_BYTES:
                raise RuntimeError("Voice file exceeds short-note limit")
            if core.file_sha256(source).hex() != source.stem:
                raise RuntimeError("Voice hash mismatch")
            core.atomic_replace_text(state, json.dumps({"state": "transcribing", "model": MODEL}))
            with tempfile.TemporaryDirectory(prefix="abvx-asr-") as directory:
                normalized = Path(directory) / "speech.wav"
                subprocess.run(["ffmpeg", "-nostdin", "-v", "error", "-y", "-i", str(source), "-ar", "16000", "-ac", "1", "-c:a", "pcm_s16le", str(normalized)], check=True, timeout=120)
                result = model.transcribe(str(normalized))
            text = getattr(result, "text", "")
            if not isinstance(text, str) or not text.strip():
                raise RuntimeError("Empty transcription; retry remains available")
            core.atomic_replace_text(source.with_suffix(".md"), f"# Voice note\n\nSource SHA-256: `{source.stem}`\n\nModel: `{MODEL}`\n\n{text.strip()}\n")
            core.atomic_replace_text(state, json.dumps({"state": "transcribed", "model": MODEL}))
            done += 1
        except Exception as exc:
            core.atomic_replace_text(state, json.dumps({"state": "failed", "error": str(exc)}))
            failures.append(source.name)
    print(f"ASR transcribed={done} failed={len(failures)}", flush=True)
    if failures:
        raise RuntimeError("Transcription failures; original WAVs retained, retry the command")
    return {"transcribed": done, "pending": 0}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("action", choices=("offload", "transcribe"))
    parser.add_argument("--destination", type=Path, default=Path.home() / "ABVxCompanionBackup/Voice")
    parser.add_argument("--url", default=DEVICE_URL)
    args = parser.parse_args()
    with core.storage_lock():
        if args.action == "offload":
            offload(args.destination, args.url)
        else:
            transcribe(args.destination)


if __name__ == "__main__":
    main()
