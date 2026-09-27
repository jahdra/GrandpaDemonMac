#!/usr/bin/env python3
"""Integration guards for the exact upstream source used to build Fake Rate.

These are source-level invariants, not a substitute for running GD's UI hooks.
"""
from pathlib import Path
import subprocess
import sys

source = Path(sys.argv[1])

def text(path):
    return (source / 'src' / path).read_text()

def original(path):
    return subprocess.check_output(['git', '-C', str(source), 'show', 'HEAD:src/' + path], text=True)

# A migrated ID must be used in every UI/renderer path, not only the picker.
for cpp in (source / 'src').rglob('*.cpp'):
    body = cpp.read_text()
    assert '"itzkiba.grandpa_demon/' not in body, f'hardcoded old resource in {cpp}'
    assert 'isModLoaded("itzkiba.grandpa_demon")' not in body, f'old detection in {cpp}'
    assert 'getLoadedMod("itzkiba.grandpa_demon")' not in body, f'old detection in {cpp}'

for path in ('classes/FRGRDPopup.cpp', 'classes/FREditPopup.cpp', 'classes/FRSetFeaturePopup.cpp',
             'hooks/LevelInfoLayer.cpp', 'hooks/LevelCell.cpp', 'classes/FREffects.cpp'):
    assert 'FakeRate::grandpaResource(' in text(path), path
assert 'FakeRate::getGrandpaMod()' in text('classes/FRSetDifficultyPopup.cpp')
assert 'GJDifficulty::DemonExtreme' in text('classes/FRSetDifficultyPopup.cpp')
assert 'm_moreDifficultiesOverride = 0;' in text('classes/FRSetDifficultyPopup.cpp')

info = text('hooks/LevelInfoLayer.cpp')
assert 'FakeRate::getGRDOverride(m_difficultySprite)' in info
assert 'FakeRate::showRevivedEffects(this, m_difficultySprite, remove)' in info
assert 'grandpaDemon && !revived && !gddpOverride && !dibOverride' in info
assert '''if (remove && revived) {
            updateLabelValues();
            FakeRate::showRevivedEffects(this, m_difficultySprite, true);
            defaultFakeRate();''' in info
assert 'FakeRate::showRevivedEffects(nullptr, difficultySprite, false)' in text('hooks/LevelCell.cpp')
assert 'std::clamp(' in text('classes/FREffects.cpp')

# The patch must leave the load/save implementation and struct layout alone.
marker = 'Result<FakeRateSaveData> matjson::Serialize<FakeRateSaveData>::fromJson'
assert text('FakeRate.cpp').split(marker)[1] == original('FakeRate.cpp').split(marker)[1]
assert text('FakeRate.hpp').split('struct FakeRateSaveData {')[1].split('};')[0] == \
    original('FakeRate.hpp').split('struct FakeRateSaveData {')[1].split('};')[0]
for marker in ('$on_mod(Loaded)', '$on_mod(DataSaved)', 'FakeRateSaveData* FakeRate::getFakeRate'):
    assert text('FakeRate.cpp').split(marker)[1].split('\n}')[0] == original('FakeRate.cpp').split(marker)[1].split('\n}')[0]
print('Patched picker, previews, renderers, restoration and unchanged save schema checked.')
