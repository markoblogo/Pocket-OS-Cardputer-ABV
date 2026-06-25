# First Real-Device Run

## Required Hardware

- M5Stack Cardputer-Adv.
- USB-C data cable.
- microSD card, 4 GB or larger recommended.
- Computer with PlatformIO installed.
- Optional: headphones/speaker checks depending on your audio setup.

## Format The microSD Card

Format the card as FAT32. Use a simple volume name such as `CARDPUTER`.

On macOS, Disk Utility can erase the card as `MS-DOS (FAT)`.

## Prepare The SD Card

Recommended:

```sh
python3 tools/prepare_sdcard.py /Volumes/CARDPUTER
python3 tools/validate_sdcard.py /Volumes/CARDPUTER
```

Manual method: copy the contents of `sdcard_template/` to the SD card root.

Required folders:

```text
/music
/recordings
/notes
/books
/browser
/browser/bookmarks
/browser/saved_pages
/ai
/config
/logs
/tmp
```

## Where To Put Files

- Music: copy `.mp3` files to `/music`.
- Books: copy `.txt` files to `/books`.
- Notes: copy `.txt` files to `/notes`.
- Randomizer lists: edit `/config/randomizer_lists.txt`, one item per line. Lines starting with `#` are ignored.

## Configure Wi-Fi

Use the Network app on device to scan, enter a password, and save networks to `/config/wifi.json`.

You may also edit `/config/wifi.json` manually, but do not commit real passwords.

## Configure AI

Edit `/config/ai.json` on the SD card. Do not edit the committed template with real secrets.

Set:

```json
{
  "enabled": true,
  "endpoint": "https://your-endpoint.example/v1/chat/completions",
  "apiKey": "your-key",
  "model": "your-model",
  "provider": "openai_compatible",
  "systemPrompt": "You are a concise assistant.",
  "insecureTlsForTesting": true
}
```

Never commit real API keys.

## Build

```sh
platformio run -e m5stack-cardputer-adv
```

## Upload

Connect the Cardputer-Adv over USB-C, then run:

```sh
platformio run -e m5stack-cardputer-adv -t upload
```

If upload fails, hold the boot/reset sequence required by your board and retry.

## Serial Monitor

```sh
platformio device monitor -e m5stack-cardputer-adv -b 115200
```

Exit with `Ctrl+]`.

## First Boot Checklist

1. Insert the prepared SD card.
2. Power on the Cardputer-Adv.
3. Confirm the main menu appears.
4. Open System Info and confirm SD is mounted.
5. Open Input Test and verify arrows, text keys, GO short press, and GO long press.
6. Open Notes and save a short note.
7. Open Reader with a `.txt` book.
8. Open Network and scan Wi-Fi.
9. Open Music with an `.mp3` file and test volume at low level first.
10. Open Recorder and save a short WAV.

## Expected Failure Messages

- `SD missing`: SD card is absent, unformatted, or not mounted.
- `No MP3 files in /music`: add `.mp3` files to `/music`.
- `No .txt books in /books`: add `.txt` files to `/books`.
- `Wi-Fi not connected`: connect from Network before Browser, AI, or Web File Manager.
- `API key missing`: configure `/config/ai.json` on the SD card.
- `HTTP error ...`: endpoint rejected the browser or AI request.
- `error: MP3 open/output failed`: audio path or file format needs device validation.

## Known Limitations

- Browser is text-only. No JavaScript or CSS rendering.
- Web File Manager is local-network only and may be unauthenticated.
- AI depends on the endpoint schema and TLS behavior.
- Screen-off mode turns off backlight; deep sleep is not used.
- Audio hardware needs real-device tuning.

## Audio Safety Notes

M5Unified microphone and speaker paths may need exclusive hardware use.

- Stop Music before starting Recorder.
- Timer beep during recording may be suppressed or delayed.
- MusicApp and RecorderApp are the highest priority hardware validation targets.

