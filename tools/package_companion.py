#!/usr/bin/env python3
"""Generate app resources from the canonical tools directory."""
from pathlib import Path
import shutil
root=Path(__file__).resolve().parent
resources=root/'ABVx Companion.app/Contents/Resources'
runtime_sources = (
    'abvx_companion.py',
    'abvx_companion_app.py',
    'cardputer_local_model.py',
    'cardputer_local_pipeline.py',
    'cardputer_time_sync.py',
    'cardputer_upload.py',
    'needle_intent_adapter.py',
    'prepare_music.py',
    'voice_companion.py',
)
for name in runtime_sources:
    shutil.copy2(root/name, resources/name)
shutil.copy2(root/'companion_ui/index.html',resources/'companion_ui/index.html')
print('Companion resources generated')
