import importlib.util
import json
from pathlib import Path
import plistlib
import tempfile
import unittest
from unittest.mock import patch
import zipfile

spec = importlib.util.spec_from_file_location('verify_package', 'scripts/verify_package.py')
verifier = importlib.util.module_from_spec(spec)
spec.loader.exec_module(verifier)


class PackageTests(unittest.TestCase):
    def package(self, root, omit=None, bad_manifest=False, bad_frame=False):
        manifest = json.loads(Path('mod.json').read_text())
        if bad_manifest:
            manifest['gd']['mac'] = '2.2074'
        files = {'mod.json': json.dumps(manifest), manifest['id'] + '.dylib': b'fake'}
        frames = {f'GrD_demon{i}{suffix}.png': {} for i in range(6) for suffix in ('', '_text')}
        frames['GrD_demon4_infinity.png'] = {}
        if bad_frame:
            del frames['GrD_demon5.png']
        for scale in ('', '-hd', '-uhd'):
            files[f'resources/GrD_demon4_bg{scale}.png'] = b'fake'
            for name in ('GrD_IconSheet', 'GrD_ReadmeSheet'):
                files[f'resources/{name}{scale}.png'] = b'fake'
                files[f'resources/{name}{scale}.plist'] = plistlib.dumps({'frames': frames})
        if omit:
            del files[omit]
        path = Path(root) / 'test.geode'
        with zipfile.ZipFile(path, 'w') as archive:
            for name, content in files.items():
                archive.writestr(name, content)
        return path

    @patch.object(verifier.subprocess, 'check_output', return_value='x86_64 arm64\n')
    def test_universal(self, lipo):
        with tempfile.TemporaryDirectory() as root:
            verifier.verify(self.package(root))
            self.assertEqual(lipo.call_args.args[0][0:2], ['lipo', '-archs'])

    @patch.object(verifier.subprocess, 'check_output', return_value='arm64\n')
    def test_reject_single_arch(self, _):
        with tempfile.TemporaryDirectory() as root:
            with self.assertRaisesRegex(AssertionError, 'not universal'):
                verifier.verify(self.package(root))

    def test_missing_retina_asset(self):
        with tempfile.TemporaryDirectory() as root:
            with self.assertRaisesRegex(AssertionError, 'missing GrD_IconSheet-uhd.png'):
                verifier.verify(self.package(root, omit='resources/GrD_IconSheet-uhd.png'))

    def test_wrong_manifest(self):
        with tempfile.TemporaryDirectory() as root:
            with self.assertRaisesRegex(AssertionError, 'manifest mismatch'):
                verifier.verify(self.package(root, bad_manifest=True))

    def test_missing_frame(self):
        with tempfile.TemporaryDirectory() as root:
            with self.assertRaisesRegex(AssertionError, 'missing sprite'):
                verifier.verify(self.package(root, bad_frame=True))

    def test_missing_binary(self):
        with tempfile.TemporaryDirectory() as root:
            mod_id = json.loads(Path('mod.json').read_text())['id']
            with self.assertRaisesRegex(AssertionError, 'missing macOS binary'):
                verifier.verify(self.package(root, omit=mod_id + '.dylib'))
