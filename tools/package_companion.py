#!/usr/bin/env python3
"""Generate app resources from the canonical tools directory."""
from pathlib import Path
import shutil
root=Path(__file__).resolve().parent
resources=root/'ABVx Companion.app/Contents/Resources'
runtime_sources = (root/'companion_runtime_files.txt').read_text(encoding='utf-8').splitlines()
for name in runtime_sources:
    if not name or name.startswith('#'):
        continue
    shutil.copy2(root/name, resources/name)
shutil.copy2(root/'companion_ui/index.html',resources/'companion_ui/index.html')
print('Companion resources generated')
