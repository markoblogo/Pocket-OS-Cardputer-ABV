# YTMamp remote control

Pocket OS can control [YTMamp](https://github.com/markoblogo/YTMamp) over the local network. The integration is opt-in and uses YTMamp's stable cast API v1.

## Configure YTMamp

Start YTMamp on the computer with LAN access and a strong shared token:

```sh
INTEGRATION_HOST=0.0.0.0 INTEGRATION_TOKEN=choose-a-long-secret npm start
```

Start **Connections** on Cardputer and join its temporary `ABVX-Cardputer` Wi-Fi from the computer. YTMamp remains loopback-only unless `INTEGRATION_HOST` is explicitly changed, and it refuses tokenless LAN mode. YouTube Music also needs another internet path on the computer, such as Ethernet, while Wi-Fi is attached to Cardputer.

## Configure Pocket OS

Set **CAST HOST** to the computer's address on the Cardputer network (normally `192.168.4.2`) and **CAST PORT** to `18880` in Settings. Put the same token in `/CARDPTR/CONFIG.TXT` on the SD card:

```text
CAST_TOKEN=choose-a-long-secret
```

Pocket OS never shows the token on screen. The configuration file stores it as plain text on the SD card, so keep that card private. Saving Settings preserves the token.

## Controls and current evidence

Pocket OS uses `GET /api/cast/status` and sends `toggle`, `next`, and `prev` to `POST /api/cast/cmd`. The request format, token header, API version, fallback behavior, and firmware compilation are covered by automated checks. Real Cardputer-to-YTMamp LAN control still needs a physical-device entry in [HARDWARE_ACCEPTANCE.md](HARDWARE_ACCEPTANCE.md).
