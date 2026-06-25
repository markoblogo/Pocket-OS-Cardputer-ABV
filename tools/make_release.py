#!/usr/bin/env python3
import datetime as dt
import shutil
import subprocess
import zipfile
from pathlib import Path

ENV = "m5stack-cardputer-adv"

FEATURE_STATUS = """\
Feature status:
- Working: input contract, wake/sleep handling, launcher, power, storage, settings, notes, reader, randomizer, diagnostics.
- Partially working: music, recorder, network, web file manager, browser, AI text, clock.
- Disabled: AI voice.
- Informational: payments.
"""

KNOWN_LIMITS = """\
Known limitations:
- Music and recorder need real Cardputer-Adv audio validation.
- Screen-off uses backlight-only; screen wake is key-first and does not forward first key.
- Web File Manager is local-network only and may be unauthenticated.
- Browser is text-only and ignores JavaScript/CSS.
- AI provider schemas may need endpoint tuning.
"""

SD_LAYOUT = """\
SD card layout:
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
"""

TEST_CHECKLIST = """\
Test checklist:
1. Boot to main menu.
2. Open Input Test and verify punctuation/Fn arrow mapping, BtnGO short/long.
3. Verify wake suppression on first key after screen-off.
4. Verify launcher number shortcuts 1-0, Tab, and Left/Right/Up/Down navigation.
5. Play MP3 and check music UI, state, and volume control.
6. Confirm no phantom track list state errors ("No MP3" when files exist).
7. Open Notes and save text.
8. Open Reader and scroll normal/speed modes.
9. Scan/connect Wi-Fi and open Browser/AI.
10. Open System Info, Randomizer, Web File Manager, and Recorder smoke test.
"""

def main() -> int:
    root = Path(__file__).resolve().parents[1]
    release = root / "release"
    release.mkdir(exist_ok=True)

    subprocess.run(["platformio", "run", "-e", ENV], cwd=root, check=True)

    firmware = root / ".pio" / "build" / ENV / "firmware.bin"
    dest_fw = release / "firmware.bin"
    shutil.copy2(firmware, dest_fw)

    zip_path = release / "sdcard_template.zip"
    if zip_path.exists():
        zip_path.unlink()
    with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as zf:
        for path in (root / "sdcard_template").rglob("*"):
            if path.is_file():
                zf.write(path, path.relative_to(root))

    shutil.copy2(root / "README-FIRST-RUN.md", release / "README-FIRST-RUN.md")

    notes = release / "RELEASE_NOTES.txt"
    notes.write_text(
        f"Cardputer ADV Unified Shell Release Candidate\n"
        f"Build date: {dt.datetime.now().isoformat(timespec='seconds')}\n"
        f"Firmware: firmware.bin\n"
        f"Firmware size: {dest_fw.stat().st_size} bytes\n\n"
        f"{FEATURE_STATUS}\n{KNOWN_LIMITS}\n{SD_LAYOUT}\n{TEST_CHECKLIST}\n",
        encoding="utf-8",
    )

    print(f"Release written to: {release}")
    print(f"Firmware: {dest_fw} ({dest_fw.stat().st_size} bytes)")
    print(f"SD template zip: {zip_path}")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
