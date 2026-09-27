#!/usr/bin/env python3
"""Check the compatibility build, not the original Grandpa mod package."""
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import zipfile


def verify(package, source):
    source = Path(source)
    expected = json.loads((source / 'mod.json').read_text())
    with zipfile.ZipFile(package) as archive:
        names = archive.namelist()
        manifest = json.loads(archive.read('mod.json'))
        for key in ('id', 'name', 'version', 'geode', 'gd', 'dependencies'):
            assert manifest[key] == expected[key], f'manifest mismatch: {key}'
        assert manifest['id'] == 'hiimjustin000.fake_rate'
        assert manifest['gd'] == {'mac': '2.2081'}
        assert manifest['name'] == 'Fake Rate (Grandpa Compat)'
        for png in (source / 'resources').glob('*.png'):
            for scale in ('', '-hd', '-uhd'):
                resource = png.stem + scale + '.png'
                assert any(Path(name).name == resource for name in names), f'missing {resource}'
        for plist in (source / 'resources').glob('*.plist'):
            assert any(Path(name).name == plist.name for name in names), f'missing {plist.name}'
        notice = next(name for name in names if Path(name).name == 'compatibility-notice.txt')
        assert b'MIT License' in archive.read(notice)
        assert b'Not an official Fake Rate release' in archive.read(notice)
        binary = manifest['id'] + '.dylib'
        assert binary in names, 'missing macOS binary'
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / binary
            path.write_bytes(archive.read(binary))
            archs = subprocess.check_output(['lipo', '-archs', str(path)], text=True).split()
            assert set(archs) == {'arm64', 'x86_64'}, f'not universal: {archs}'
    print(f'Validated Fake Rate compatibility package: {package.name}')


if __name__ == '__main__':
    packages = list(Path(sys.argv[1]).rglob('*.geode'))
    assert len(packages) == 1, f'Expected one Fake Rate package, got {len(packages)}'
    verify(packages[0], sys.argv[2])
