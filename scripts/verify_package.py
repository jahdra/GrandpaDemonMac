#!/usr/bin/env python3
"""Fail CI if the macOS artifact is incomplete or not a universal Mach-O."""
import json
from pathlib import Path
import plistlib
import subprocess
import sys
import tempfile
import zipfile


def verify(package):
    expected = json.loads(Path('mod.json').read_text())
    with zipfile.ZipFile(package) as archive:
        names = archive.namelist()
        manifest = json.loads(archive.read('mod.json'))
        for key in ('id', 'version', 'geode', 'gd'):
            assert manifest[key] == expected[key], f'manifest mismatch: {key}'
        binary = expected['id'] + '.dylib'
        assert binary in names, 'missing macOS binary'
        for scale in ('', '-hd', '-uhd'):
            resources = [f'GrD_demon4_bg{scale}.png']
            for sheet_name in expected['resources']['spritesheets']:
                resources += [f'{sheet_name}{scale}.png', f'{sheet_name}{scale}.plist']
            for resource in resources:
                assert any(Path(name).name == resource for name in names), f'missing {resource}'
            sheet = next(name for name in names if Path(name).name == f'GrD_IconSheet{scale}.plist')
            frames = plistlib.loads(archive.read(sheet))['frames']
            for index in range(6):
                for suffix in ('', '_text'):
                    name = f'GrD_demon{index}{suffix}.png'
                    assert any(frame.endswith(name) for frame in frames), f'missing sprite {name}'
            assert any(frame.endswith('GrD_demon4_infinity.png') for frame in frames)
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / binary
            path.write_bytes(archive.read(binary))
            archs = subprocess.check_output(['lipo', '-archs', str(path)], text=True).split()
            assert set(archs) == {'arm64', 'x86_64'}, f'not universal: {archs}'
    print(f'Validated universal macOS package: {package.name}')


if __name__ == '__main__':
    path = Path(sys.argv[1])
    packages = [path] if path.is_file() else list(path.rglob('*.geode'))
    assert packages, f'No .geode package found in {path}'
    for package in packages:
        verify(package)
