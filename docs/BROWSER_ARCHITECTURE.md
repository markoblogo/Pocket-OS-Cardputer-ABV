# Browser Architecture

ABVx Browser is not a general graphical browser. It is a prepared-first, device-light text browser for a small set of favorite sites.

## Product model

The user has a small fixed set of important sites. These should be prepared on Mac first, copied to Cardputer, and then optionally refreshed online from the device.

Offline behavior:

- Cardputer opens the cached prepared version.
- No network required.
- No HTML/CSS/JS parsing on device.

Online behavior:

- Cardputer may refresh known favorites later.
- Refresh should update cached text snapshots, not behave like a full browser.

## Mac-side preprocessing

Mac Companion should handle heavy work:

1. Fetch URL.
2. Remove scripts, styles, images, video, ads, tracking, layout noise.
3. Extract readable text and useful hyperlinks.
4. Normalize encoding and line wrapping.
5. Save a Cardputer-ready package.
6. Copy package to `/sdcard/browser`.

## Cardputer-side browser

Cardputer should stay simple:

- Favorites list.
- Page text viewer, similar to Reader.
- Links list.
- Open cached pages offline.
- Optional online refresh for known favorites later.
- Download supported files later: TXT, MP3, WAV, etc.

## Proposed SD format

```text
/sdcard/browser/
  INDEX.TXT
  SITE001/
    META.TXT
    PAGE.TXT
    LINKS.TXT
  SITE002/
    META.TXT
    PAGE.TXT
    LINKS.TXT
```

`INDEX.TXT`:

```text
SITE001|Spike Spot Index
SITE002|ABVx Monitor
```

`META.TXT`:

```text
TITLE=Spike Spot Index
URL=https://example.com
UPDATED=2026-07-07T12:00:00
```

`PAGE.TXT`:

```text
Clean readable text prepared for Cardputer.
```

`LINKS.TXT`:

```text
1|Market offers|SITE002
2|Original URL|https://example.com/offers
```

## Design rule

Browser is prepared-first, device-light.

Do not build a full browser engine on Cardputer. Treat Browser as a cached text reader with links and optional refresh.

## Browser / Search / AI separation

ABVx should keep three online concepts separate:

```text
Browser = curated web reader
Search  = text-only search adapter
AI Chat = ask/explain/summarize online
```

### Browser

Browser is favorites-first. It opens a small curated set of user-approved resources, usually prepared by Mac Companion and stored on SD.

Example favorite list:

```text
1|ABVx Monitor|SITE001|REFRESH
2|Spike Spot Index|SITE002|REFRESH
3|Project dashboard|SITE003|STATIC
4|Google Text Search|SEARCH001|SEARCH
5|Docs / reference|SITE004|STATIC
```

Favorite modes:

- `STATIC`: use saved version only.
- `REFRESH`: online refresh updates the saved text snapshot.
- `SEARCH`: accepts a query and returns text-only results.
- `DOWNLOAD`: page/resource intended for supported file downloads.

Do not hardcode personal sites directly into the firmware binary. Firmware should provide the Browser app and default schema. Mac Companion should generate/update `/sdcard/browser/INDEX.TXT` and site packages.

### Text Search

Search should not be a full Google browser. It should be an adapter that returns clean text results:

```text
Results for: esp32 s3 mp3 decoder

1. Minimp3 ESP32 example
   https://...
   Short snippet...

2. ESP-IDF I2S audio output
   https://...
   Short snippet...
```

Cardputer UI:

- Up/Down: select result.
- OK: open result as prepared text page if available or fetch simplified page later.
- GO: back.

The adapter may live in Mac Companion or a future backend/proxy. Direct heavy search parsing on Cardputer is not a goal.

### AI Chat

If the user wants to ask a general question, explain something, summarize, or reason, that belongs to AI Chat, not Browser.

AI Chat should later support:

- online OpenAI API when connected;
- text question to text answer;
- save answer to Notes;
- voice input/output later.

Offline shell must continue working if AI is unavailable.

## Firmware vs SD content

Recommended split:

- Firmware: Browser app, parser for prepared packages, simple UI, optional refresh hooks.
- SD: favorites list, cached pages, links, metadata.
- Mac Companion: fetch, clean, normalize, package, sync.

This keeps favorite sites editable without reflashing firmware.
