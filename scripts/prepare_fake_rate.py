#!/usr/bin/env python3
"""Apply the reviewed compatibility patch to an exact upstream checkout.

The generated source tree belongs in build-fake-rate/, not in version control.
Never modifies the working Grandpa Demon mod or either mod's saved data.
"""
import json
from pathlib import Path
import shutil
import subprocess
import sys

UPSTREAM_COMMIT = 'fac307b118450367a867ade2e2eef345ae98ae93'
ROOT = Path(__file__).resolve().parents[1]
COMPAT = ROOT / 'compat' / 'fake-rate'


def prepare(source):
    source = Path(source).resolve()
    if source == ROOT or not (source / 'src' / 'FakeRate.cpp').is_file():
        raise ValueError('Expected a separate FakeRate source checkout')
    commit = subprocess.check_output(['git', '-C', str(source), 'rev-parse', 'HEAD'], text=True).strip()
    if commit != UPSTREAM_COMMIT:
        raise ValueError(f'Wrong upstream revision: expected {UPSTREAM_COMMIT}, got {commit}')
    manifest = json.loads((source / 'mod.json').read_text())
    if manifest['id'] != 'hiimjustin000.fake_rate' or manifest['version'] != 'v1.4.19':
        raise ValueError('Unexpected Fake Rate manifest')
    patch = str(COMPAT / 'fake-rate.patch')
    subprocess.run(['git', '-C', str(source), 'apply', '--check', patch], check=True)
    subprocess.run(['git', '-C', str(source), 'apply', patch], check=True)
    shutil.copyfile(COMPAT / 'GrandpaCompat.hpp', source / 'src' / 'GrandpaCompat.hpp')
    manifest['geode'] = '5.10.1'
    manifest['gd'] = {'mac': '2.2081'}
    manifest['name'] = 'Fake Rate (Grandpa Compat)'
    manifest['description'] = 'Unofficial macOS compatibility build for Grandpa Demon Revived.'
    # Same ID and save schema deliberately preserve installed Fake Rate data.
    manifest['resources']['files'].append('compatibility-notice.txt')
    (source / 'mod.json').write_text(json.dumps(manifest, indent=4) + '\n')
    notice = (
        'Unofficial Fake Rate / Grandpa Demon Revived compatibility build 1.\n'
        f'Based on hiimjasmine00/FakeRate v1.4.19, commit {UPSTREAM_COMMIT}.\n'
        'Not an official Fake Rate release. Original author: hiimjasmine00.\n'
        'Compatibility source and instructions:\n'
        'https://github.com/jahdra/GrandpaDemonMac/tree/arena/01a0e16c-grandpademonmac/compat/fake-rate\n\n'
    )
    (source / 'compatibility-notice.txt').write_text(notice + (COMPAT / 'LICENSE.upstream').read_text())
    print(f'Prepared Fake Rate compatibility source at {source}')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit('Usage: python3 scripts/prepare_fake_rate.py path/to/pinned/FakeRate')
    prepare(sys.argv[1])
