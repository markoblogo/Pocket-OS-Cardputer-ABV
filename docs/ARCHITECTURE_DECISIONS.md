# Architecture Decisions

This document records product-level decisions for ABVx Cardputer firmware and future companion tools.

## Core principle

Cardputer should remain a lightweight offline-first runtime shell.

Heavy work belongs outside the device:

- file conversion;
- metadata extraction;
- web parsing;
- large transfer orchestration;
- AI requests where possible;
- rich analytics.

Cardputer should execute simple prepared formats reliably.

## AI

AI is online-only.

Offline behavior:

```text
AI OFFLINE
Connect Wi-Fi or Mac Companion
```

No offline nano-LLM is planned. A tiny local model would not provide useful answers on Cardputer ADV resources.

Preferred online route:

1. Cardputer sends prompt to Mac Companion.
2. Mac Companion calls OpenAI.
3. Mac Companion returns text answer.
4. Cardputer can display and save answer to Notes.

Reasons:

- API key stays off Cardputer.
- TLS/auth complexity stays on Mac.
- Companion can log, cache, retry, and save outputs.

Fallback/later route:

- Cardputer calls OpenAI API directly over Wi-Fi.
- This is secondary because it requires secure config, networking, and API handling on-device.

Input modes:

- MVP: text input in English, French, or translit.
- Later: voice input by recording audio and sending it to Companion/OpenAI.

Output modes:

- MVP: text answer.
- Later: optional voice/TTS.

## Agent

No standalone Agent app in the main launcher for now.

The old quick-actions Agent duplicated the launcher and added no value. Agent should only return if it becomes a real command/AI surface.

Future Agent, if implemented, should be part of the AI/command screen:

- local deterministic commands;
- OpenAI-backed Q&A when online;
- save answer to Notes;
- app actions only where they are faster than launcher navigation.

Do not build fake offline AI.

## Mac Companion

Mac Companion is the preparation and sync layer.

Responsibilities:

- convert books to Cardputer-ready TXT;
- convert music to Cardputer-friendly MP3;
- prepare browser favorite packages;
- pull notes and recordings;
- sync time/config;
- proxy AI requests;
- firmware update later.

Initial transport can be mounted SD card. Wi-Fi transfer is useful later, but not required for the first companion MVP.

## Browser

Browser is prepared-first and device-light.

- Mac Companion fetches and cleans favorite sites.
- Cardputer reads cached text packages from `/sdcard/browser`.
- Offline mode shows saved snapshots.
- Online mode may refresh known favorites later.
- Search is a text-only adapter, not a full Google browser.
- General questions belong to AI Chat, not Browser.

See `docs/BROWSER_ARCHITECTURE.md`.

## Connections / Transfer

Current scope:

- Wi-Fi AP diagnostics;
- ping/status/list/download/write-test;
- small upload only.

Large upload is disabled because MP3-sized Wi-Fi writes caused SD instability.

Future transfer should be redesigned as a staged/chunked state machine and tested separately. Bluetooth transfer is postponed/R&D.

## Recorder

Current safe design:

- RAM/PSRAM-first recording;
- save to SD only after recording stops;
- continuous playback from RAM buffer.

Reason:

- Live SD writes during microphone capture caused I/O errors and SD state loss.

If longer recording remains unstable, limit firmware to the proven duration and move long recording to a dedicated R&D task.

## Reader

Reader firmware is considered functionally ready for MVP.

Remaining validation:

- test one real English book;
- test one real Russian book;
- verify normal reading, speed mode, and bookmarks.

Book conversion belongs to Mac Companion, not firmware.

## Notes

Notes editor is LAT/plain text only.

- User can write English/French/translit.
- External Cyrillic notes can be displayed.
- Cyrillic notes are view-only in firmware.

Do not spend firmware complexity on full Unicode editing for now.

## Habits

Current Habits app is sufficient for firmware MVP.

- Daily checklist.
- Manual next day.
- 7D/30D stats.

Richer weekly/monthly/yearly processing should be done in Mac Companion after pulling logs.

## Time

Cardputer cannot preserve real time across full power-off without reliable RTC persistence/time source.

Current firmware keeps:

- manual clock set;
- stopwatch;
- timer;
- alarm.

Future sync sources:

- Mac Companion;
- phone/web companion;
- NTP over Wi-Fi later.

## Files

Files app should stay simple:

- browse SD;
- open known file types;
- show info for unsupported files;
- delete with confirmation;
- expose `/cardputer` as transfer folder.

Do not build a full Finder clone.
