#!/bin/zsh
set -euo pipefail
SUPPORT_DIR="${ABVX_COMPANION_SUPPORT_DIR:-$HOME/Library/Application Support/ABVx Companion}"
VENV_DIR="$SUPPORT_DIR/.venv"
BOOTSTRAP_PYTHON="${ABVX_COMPANION_BOOTSTRAP_PYTHON:-python3}"
command -v ffmpeg >/dev/null || { print -u2 'Install ffmpeg first: brew install ffmpeg'; exit 1; }
[[ "$(uname -m)" == arm64 ]] || { print -u2 'Parakeet MLX requires Apple Silicon'; exit 1; }
[[ -x "$VENV_DIR/bin/python" ]] || "$BOOTSTRAP_PYTHON" -m venv "$VENV_DIR"
"$VENV_DIR/bin/python" -m pip install 'parakeet-mlx==0.5.2'
print "Runtime installed. Weights download on first TRANSCRIBE VOICE with internet available."
print "Start Companion using: '$VENV_DIR/bin/python' tools/abvx_companion_app.py"
