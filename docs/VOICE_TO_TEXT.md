# Internal Voice to local Markdown

## Setup (Apple Silicon Mac)

Install ffmpeg (`brew install ffmpeg`), then:

```sh
zsh tools/setup_abvx_voice.zsh
"$HOME/Library/Application Support/ABVx Companion/.venv/bin/python" tools/abvx_companion_app.py
```

The optional runtime pins parakeet-mlx 0.5.2. The first transcription downloads
`mlx-community/parakeet-tdt-0.6b-v3`; internet access is needed for that download.
Audio stays on the host. Subsequent cached runs can be offline. Runtime/model
installation and real-speech timing are not part of the host regression tests.

## Use

1. Record a short note with R. Stop and save.
2. Open Transfer and start its AP. Join ABVX-Cardputer on Mac using the on-screen password.
3. In Companion choose SYNC VOICE. Wait for the background job to finish successfully.
4. Choose TRANSCRIBE VOICE. If weights are not cached, return the Mac to an internet-connected network first.
5. Read Markdown in `~/ABVxCompanionBackup/Voice/<device-id>/`.

CLI equivalents:

```sh
python3 tools/voice_companion.py offload
"$HOME/Library/Application Support/ABVx Companion/.venv/bin/python" tools/voice_companion.py transcribe
```

The inventory maps device REC names to full SHA-256 filenames. Repeating offload
verifies existing files and does not create another audio copy. Changed content
under a reused REC name is preserved separately. ASR runs serially in its own
process and uses ffmpeg to produce 16 kHz PCM16 input; original WAVs remain intact.
Failures have `.json` state and no successful Markdown marker; repeat transcription
to retry. Finished Markdown is retained, so edit it freely.

## Device API v1

- `GET /api/voice/list`: version, device_id, files (name, size, sha256).
- `GET /api/voice/download?name=REC00001.WAV`: bounded internal WAV download.
- No remote deletion endpoint. Original deletion is a separate on-device action.
- 503 when Transfer is not the active screen or Voice storage is unavailable.
- AP membership is the authorization boundary; use the on-screen password.

Current capture is 4 kHz/8-bit, up to 20 seconds (~80 KB). Resampling does not
restore bandwidth. Parakeet supports Russian/Ukrainian/English, but actual short
note quality must be measured. The donor's always-streaming Arduino firmware is
not merged; long recording remains gated on SD/audio/power tests.

Sources: [Parakeet MLX](https://github.com/senstella/parakeet-mlx),
[NVIDIA model](https://huggingface.co/nvidia/parakeet-tdt-0.6b-v3).
