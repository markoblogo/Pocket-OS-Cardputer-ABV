#!/usr/bin/env python3
"""Generate app resources from the canonical tools directory."""
from pathlib import Path
import shutil
root=Path(__file__).resolve().parent
resources=root/'ABVx Companion.app/Contents/Resources'
for source in root.glob('*.py'):
    if source.name != 'package_companion.py': shutil.copy2(source, resources/source.name)
shutil.copy2(root/'companion_ui/index.html',resources/'companion_ui/index.html')
print('Companion resources generated')
