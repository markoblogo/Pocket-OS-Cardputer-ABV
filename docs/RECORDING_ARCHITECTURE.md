# Recording Architecture

Goal: reliable quick voice notes first, long-form recording later.

## Current implementation

Recorder uses a RAM-first design:

1. Stop speaker.
2. Start microphone at 16 kHz, mono, signed 16-bit PCM.
3. Capture into PSRAM/RAM buffer.
4. On OK/GO or buffer full, stop mic.
5. Save WAV to SD after recording has ended.
6. Restart speaker for playback/UI sounds.

This avoids simultaneous microphone capture and SD writes, which has been unstable on Cardputer ADV during hardware testing.

## Current duration policy

Voice is for quick memory notes, not meetings.

Target: 30 seconds.

Fallback allocation order:

```text
30s, 20s, 10s, 5s, 3s, 2s, 1s
```

The UI shows the actual allocated `MAX`, so the device does not promise a longer recording than RAM can hold on that boot.

At 16 kHz mono 16-bit PCM:

```text
10s ~= 320 KB
20s ~= 640 KB
30s ~= 960 KB
```

## Save strategy

After recording stops, the WAV file is written to SD. Short notes use a simple contiguous write; longer notes use small chunks with short yields.

If save fails, the app shows a visible error and removes the partial file when possible.

## Playback strategy

Playback streams WAV/PCM from SD in chunks. It does not load the whole recording into RAM.

Before playback, Recorder rejects obviously invalid files:

- bad WAV header;
- empty file;
- extremely short/broken recording.

Broken files show `BAD REC` instead of crashing/rebooting.

## Why not stream recording directly to SD yet

Direct live SD writes while recording previously caused I/O errors and SD state loss. A future long recorder should use a dedicated ring buffer and isolated writer task, then be tested separately.

## Future Record v2/v3

- Prove 20-30 second saves on hardware.
- Playback progress.
- Optional lower sample rate mode for longer notes.
- Ring-buffer writer task for multi-minute recordings after SD stability work.
