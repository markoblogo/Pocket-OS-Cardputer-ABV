# CardputerABVx Minimal Firmware

Minimal ABVx firmware for M5Stack Cardputer ADV.

Current focus: MP3-first offline shell with a simple recorder.

## Current working version

Confirmed on real Cardputer ADV:

- ABVx boot splash.
- Large monochrome launcher UI.
- Music app:
  - scans `/sdcard/music` for `.MP3` / `.mp3` files;
  - plays MP3 files from SD;
  - waveform visualization;
  - `OK` / `GO` stop playback;
  - Up / Down volume control: `MUTE`, `MID`, `LOUD`;
  - Left / Right track navigation;
  - `1` shuffle toggle;
  - EOF auto-next when multiple tracks exist.
- Record app:
  - stores recordings in `/sdcard/rec`;
  - first list item `NEW REC` starts recording;
  - `OK` / `GO` stops and saves;
  - saves 16-bit mono 16 kHz WAV files as `REC0001.WAV`, `REC0002.WAV`, etc.;
  - plays saved recordings;
  - waveform visualization during record and playback.

## SD card layout

Use FATFS 8.3-safe names for now.

```text
/sdcard/music/A.MP3
/sdcard/music/T01.MP3
/sdcard/rec/REC0001.WAV
```

Long filenames and long folder names are not reliable yet.

## Build

```sh
cd /Volumes/Work/Work/cardputer-abvx-minimal
. ~/esp/esp-idf-v5.4.2/export.sh
idf.py build
```

Firmware output:

```text
build/cardputer-abvx-minimal.bin
```

## Flash

Find the current USB modem port if needed:

```sh
ls /dev/cu.usbmodem*
```

Flash:

```sh
idf.py -p /dev/cu.usbmodem101 flash
```

Replace `/dev/cu.usbmodem101` with the actual port.

## Hardware test checklist

### Music

1. Open Music.
2. Select `A.MP3` or another MP3.
3. Press `OK`.
4. Confirm sound, waveform, and `PLAYING` screen.
5. Test `OK` / `GO` stop.
6. Test Up / Down volume.
7. Test Left / Right track change.
8. Test `1` shuffle toggle.

### Record

1. Open Record.
2. Select `NEW REC`.
3. Press `OK`.
4. Speak for a few seconds.
5. Press `OK` or `GO` to save.
6. Confirm `REC0001.WAV` appears.
7. Press `OK` on the WAV file.
8. Confirm playback and waveform.

## Known limitations

- No Reader/Notes MVP yet.
- Recordings use `/sdcard/rec` instead of `/sdcard/recordings` because FATFS long filenames are not enabled yet.
- Music playback is chunk/blocking-based; controls may have small latency.
- Screen-off playback/power optimization is not finalized yet.
- Dev diagnostics were removed from the normal build after proving MP3 decode/speaker path.
