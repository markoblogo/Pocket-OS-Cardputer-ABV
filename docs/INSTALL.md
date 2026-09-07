# Install 0.3.0-rc1 (Cardputer ADV only)

This is a release candidate: host checks and ESP-IDF build are not a hardware
acceptance certificate. Do not flash an original Cardputer with this ADV build.

## Preserve existing data first

For firmware with `/api/voice/list`, use Companion SYNC VOICE and check the job
succeeded. Companion Flash repeats this verification before flashing.

For older firmware without Voice export, preserve a raw flash backup before
migration. The current Voice partition is 2 MiB at 0x410000. A full 8 MiB image
also preserves bootloader, partition table and settings. With the correct serial
port and esptool installed in the ESP-IDF environment:

```sh
python -m esptool --chip esp32s3 --port YOUR_PORT read_flash 0 0x800000 cardputer-before-upgrade.bin
shasum -a 256 cardputer-before-upgrade.bin
```

Keep this private backup outside the repository. Readback may reset the device.
If readback fails, stop before flashing. Do not use erase_flash. Restore only a
backup from this same device, with its matching original partition layout.

## Flash the candidate

Download the release files and verify `SHA256SUMS` in their directory:

```sh
shasum -a 256 -c SHA256SUMS
```

Install ESP-IDF 5.4.2 for a development build, or use its esptool environment for
the downloaded binaries. List ports safely under zsh:

```sh
find /dev -maxdepth 1 -name 'cu.usbmodem*'
```

After backup, replace YOUR_PORT with the identified device and run from the
release directory:

```sh
python -m esptool --chip esp32s3 --port YOUR_PORT --baud 460800 write_flash --flash_mode dio --flash_freq 80m --flash_size 8MB 0x0 bootloader.bin 0x8000 partition-table.bin 0x10000 cardputer-abvx-minimal.bin
```

No Voice partition image is included. Never pad/erase the whole flash when
installing over existing user data. A merged `firmware.bin` is also supplied for
M5Burner preparation at offset 0; hardware acceptance is pending before catalog
publication. The partition table matches the existing 4 MiB app + 2 MiB Voice layout.

Reset, confirm About shows 0.3.0-rc1, and follow SMOKE_TEST.md. A truly blank Voice
partition initializes automatically. A damaged non-empty one stays unformatted.

## First use

Use FAT32 SD for media. Start with one MP3 and one TXT via Companion. For Voice
export open Transfer, start AP, then use its displayed password on the Mac.
The old hard-coded password is no longer the default for this candidate.

[ESP-IDF setup](https://docs.espressif.com/projects/esp-idf/en/v5.4.2/esp32s3/get-started/).
