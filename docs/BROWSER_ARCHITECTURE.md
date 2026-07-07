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
