# Recording Architecture

Goal: reliable voice notes first, longer recording second.

## Current implementation

Recorder uses a RAM-first design:

1. Stop speaker.
2. Start microphone at 16 kHz, mono, signed 16-bit PCM.
3. Capture into PSRAM/RAM buffer.
4. On OK/GO stop, stop mic.
5. Save WAV to SD after recording has ended.
6. Restart speaker for playback/UI sounds.

This avoids simultaneous microphone capture and SD writes, which has been unstable on Cardputer ADV during hardware testing.

## Current duration strategy

The firmware tries to allocate the largest safe capture buffer from this list:

```text
120s, 90s, 60s, 45s, 30s, 15s, 10s, 5s, 3s, 2s, 1s
```

At 16 kHz mono 16-bit PCM, one minute is about 1.9 MB. If PSRAM is available, longer notes should work. If not, the app gracefully falls back to shorter notes.

## Save strategy

After recording stops, the WAV file is written to SD in small chunks with short yields between chunks. This is slower than one large write but safer for the device and UI loop.

## Why not stream directly to SD yet

Direct live SD writes while recording previously caused I/O errors and SD state loss. A future streaming recorder should use a dedicated ring buffer and a carefully isolated writer task, then be tested separately.

## Future Record v2

- Recording progress and remaining capacity.
- Playback progress.
- Optional lower sample rate mode for longer notes.
- Ring-buffer writer task for multi-minute recordings after SD stability work.
