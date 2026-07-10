# Recording Architecture

Voice is optimized for quick memory capture, not meetings.

## Current hardware-verified design

- One recording mode: up to 20 seconds.
- Format: mono PCM WAV, 8 kHz, 8-bit.
- Memory: about 160 KB for the full recording.
- Capture is RAM-first.
- SD is written only after `M5.Mic.end()`.
- OK/GO stops and saves; reaching 20 seconds auto-saves.
- Playback streams WAV/PCM from SD in bounded chunks.

This is required by Cardputer ADV hardware: it has no PSRAM, and live SD writes while microphone capture is active produced `EIO` and SD-state loss. The removed live-streaming path must not be restored without a separate hardware design and stress test.

## Compatibility

Playback accepts mono PCM WAV at 8 or 16 bits and validates sample rates to 8-48 kHz. Existing 16 kHz/16-bit recordings remain playable.

## Failure policy

- RAM allocation failure stops capture without touching SD.
- Save errors are visible and partial output is removed when possible.
- Filename exhaustion fails closed instead of overwriting an existing recording.
- Malformed or unsupported WAV files show an error instead of reaching the speaker with unchecked parameters.

## Later, only if needed

Multi-minute recording requires a separate architecture. It is not part of the current product roadmap.
