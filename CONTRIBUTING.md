# Contributing

Use small pull requests with a reproducible problem and expected behavior.
Run `sh tools/check_host.sh`; firmware changes also require ESP-IDF 5.4.2
`idf.py build`. Report hardware smoke separately from host tests.

Canonical Companion code lives in `tools/`; regenerate packaged resources with
`python3 tools/package_companion.py` after changes. Do not hand-edit both copies.

Never use private voice notes, books, music, or credentials as test fixtures.
Data-loss and interrupted-write regressions need failure-path coverage.
GNSS examples must be synthetic or explicitly shared for testing.

Third-party components retain their existing licenses. Project-wide licensing
is awaiting owner selection; do not infer a license from dependency notices.
