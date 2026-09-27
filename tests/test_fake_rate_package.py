import importlib.util
import json
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
import zipfile

spec = importlib.util.spec_from_file_location('verify_fake_rate', 'scripts/verify_fake_rate_package.py')
verifier = importlib.util.module_from_spec(spec)
spec.loader.exec_module(verifier)


class FakeRatePackageTests(unittest.TestCase):
    def package(self, root, omit=None, wrong_id=False, wrong_name=False):
        root = Path(root)
        (root / 'resources').mkdir()
        (root / 'resources' / 'FR_grdBtn_001.png').write_bytes(b'png')
        (root / 'resources' / 'grandpaEffect.plist').write_bytes(b'plist')
        data = {'id': 'hiimjustin000.fake_rate', 'name': 'Fake Rate (Grandpa Compat)',
                'version': 'v1.4.19', 'geode': '5.10.1', 'gd': {'mac': '2.2081'},
                'dependencies': {'geode.node-ids': '>=v1.12.0'}}
        (root / 'mod.json').write_text(json.dumps(data))
        if wrong_id:
            data['id'] = 'another.mod'
        if wrong_name:
            data['name'] = 'Fake Rate'
        files = {'mod.json': json.dumps(data), 'hiimjustin000.fake_rate.dylib': b'binary',
                 'compatibility-notice.txt': 'MIT License\nNot an official Fake Rate release',
                 'grandpaEffect.plist': b'plist'}
        for scale in ('', '-hd', '-uhd'):
            files[f'FR_grdBtn_001{scale}.png'] = b'png'
        if omit:
            del files[omit]
        package = root / 'test.geode'
        with zipfile.ZipFile(package, 'w') as archive:
            for name, content in files.items():
                archive.writestr(name, content)
        return package

    @patch.object(verifier.subprocess, 'check_output', return_value='arm64 x86_64\n')
    def test_valid(self, _):
        with tempfile.TemporaryDirectory() as root:
            verifier.verify(self.package(root), root)

    @patch.object(verifier.subprocess, 'check_output', return_value='x86_64\n')
    def test_intel_only_rejected(self, _):
        with tempfile.TemporaryDirectory() as root:
            with self.assertRaisesRegex(AssertionError, 'not universal'):
                verifier.verify(self.package(root), root)

    def test_no_save_id_change(self):
        with tempfile.TemporaryDirectory() as root:
            with self.assertRaisesRegex(AssertionError, 'manifest mismatch: id'):
                verifier.verify(self.package(root, wrong_id=True), root)

    def test_unofficial_name_required(self):
        with tempfile.TemporaryDirectory() as root:
            with self.assertRaisesRegex(AssertionError, 'manifest mismatch: name'):
                verifier.verify(self.package(root, wrong_name=True), root)

    def test_missing_retina_icon(self):
        with tempfile.TemporaryDirectory() as root:
            with self.assertRaisesRegex(AssertionError, 'missing FR_grdBtn_001-uhd.png'):
                verifier.verify(self.package(root, omit='FR_grdBtn_001-uhd.png'), root)

    def test_missing_particle_plist(self):
        with tempfile.TemporaryDirectory() as root:
            with self.assertRaisesRegex(AssertionError, 'missing grandpaEffect.plist'):
                verifier.verify(self.package(root, omit='grandpaEffect.plist'), root)
