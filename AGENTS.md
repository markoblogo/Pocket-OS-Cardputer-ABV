# Pocket OS contributor context

## Build and checks

- Target: M5Stack Cardputer ADV / ESP32-S3, ESP-IDF 5.4.x.
- Run `sh tools/check_host.sh` for host behavior.
- For firmware changes, run `idf.py build` from a clean ESP-IDF 5.4.x shell.
- Report host checks, compilation and physical-device evidence separately.

## Safety boundaries

- Preserve user flash, internal Voice data and removable media. Never format or
  erase as part of a routine test.
- Use synthetic fixtures; never commit private recordings, tracks, books or keys.
- Keep storage mutations in the main-loop handoff and retain interrupted-write
  recovery behavior.
- Change firmware in small slices. `main/main.cpp` is hardware-sensitive.

## Source ownership

- Canonical Companion sources live in `tools/`. Regenerate packaged resources
  with `python3 tools/package_companion.py`; do not hand-edit both copies.
- Vendored components retain upstream licenses. Update one dependency at a time
  and record its tag or commit in `docs/DEPENDENCIES.md`.
- Current product truth is in `README.md`, `docs/PROJECT_STATUS.md`,
  `docs/DATA_SAFETY.md`, and `docs/SMOKE_TEST.md`.
