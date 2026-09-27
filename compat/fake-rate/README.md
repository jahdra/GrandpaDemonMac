# Fake Rate × Grandpa Demon Revived — macOS compatibility build 1

This is an **unofficial compatibility build of Fake Rate**, not a replacement for
Grandpa Demon and not an official release by hiimjasmine00. It uses the upstream
MIT-licensed source at commit `fac307b118450367a867ade2e2eef345ae98ae93` (v1.4.19).
The upstream license is in `LICENSE.upstream` and included inside the package.

## Install

Keep the working **Grandpa Demon v1.3.0** package from this repository installed.
This build targets **GD 2.2081, Geode 5.10.1 and macOS arm64 + x86_64**. Geode's
Node IDs dependency is still required, as it is for official Fake Rate.

1. Close Geometry Dash. Back up your existing Fake Rate `.geode` and saved data.
2. Download `FakeRate-Grandpa-Compat-macOS` from a successful **Build Fake Rate
   compatibility for macOS** workflow run in this repository.
3. Extract its ZIP and **replace** your current `hiimjustin000.fake_rate.geode`
   in the Geode mods folder with the included file. Do not keep two copies, and
   do not uninstall/delete Fake Rate's saved data.
4. Restart. Geode should list **Fake Rate (Grandpa Compat)**.
5. Open a level → Fake Rate → the difficulty face → the **Grandpa Demon button**
   beside Confirm → choose a face → Confirm in both pickers → **Add**.

All six faces (Supreme, Ultimate, Legendary, Mythical, Silent and Grandpa) are
available on arbitrary levels, including levels outside AREDL. Choose a vanilla
face to clear the custom face. **Remove** restores the level's ordinary/AREDL
appearance. Canceling a picker does not save its pending selection.

The mod ID, v1.4.19 version and saved-data schema are retained so existing Fake
Rate entries continue to load. The changed name and packaged compatibility notice
identify this as a fork. A later official Fake Rate update can replace the fix;
keep this package as a backup and check upstream support before updating.
Reverting to the original Fake Rate file preserves saved entries, but the
original will not display Grandpa Revived overrides until compatibility exists.

## What changed

- Recognizes `ultrasoda.grandpa_demon_revived` as well as the original
  `itzkiba.grandpa_demon`, preferring Revived if both are detected.
- Uses the detected provider's resources in the picker, edit preview, feature
  preview, cells, level pages, infinity symbol and backgrounds.
- Corrects the mapping between texture indices **0–5** and saved overrides **1–6**.
  Supreme is no longer confused with “no override”.
- Reads AREDL's current in-place difficulty frame when initializing the editor,
  so editing only stars/feature status can preserve its face.
- Hides Grandpa's own effects while Fake Rate draws a manual rating, then restores
  them and refreshes the AREDL face on Remove. No private field offsets, fake mod
  registrations or modifications to another mod's saved data are used.
- Clears a conflicting More Difficulties override when selecting a Grandpa face.
- Bounds background alpha and fixes the featured-state restoration check.

Automatic AREDL selection still honors Grandpa's “Disable Grandpa Demon” setting.
A face explicitly selected in Fake Rate takes precedence; the six-option manual
picker remains available. Background and particle settings are read from the
selected Grandpa provider.

## Reproduce the build

The `fake-rate.patch` file is the complete source diff against the pinned upstream
revision. `GrandpaCompat.hpp` is the additional, separately tested policy header.
No third-party checkout or generated binary is stored in this repository.

```sh
git clone https://github.com/hiimjasmine00/FakeRate.git build-fake-rate
# In this separate dependency checkout, use the pinned commit above.
git -C build-fake-rate checkout --detach fac307b118450367a867ade2e2eef345ae98ae93
python3 scripts/prepare_fake_rate.py build-fake-rate
python3 tests/check_fake_rate_source.py build-fake-rate
```

On a Mac, select Xcode 26.3, install the Geode 5.10.1 SDK/CLI with macOS binaries,
set `GEODE_SDK`, and build `build-fake-rate` with the ordinary Geode CMake process.
CI pins the bindings, SDK and CLI and checks the resulting universal package.

## Validation / in-game checklist

Automated checks exercise the actual provider-selection and face-index policy,
patch applicability, all migrated UI paths, preservation of the save schema, and
package resources/architecture. The native build runs on a macOS GitHub runner.
**These checks do not run Geometry Dash.** Before calling this runtime-verified:

- [ ] Existing Fake Rate entries still appear after replacing the package.
- [ ] Select every custom face on an unranked level; verify previews and level cells.
- [ ] Save, restart GD and reopen the level; the chosen face persists.
- [ ] Change stars and featured/epic/legendary/mythic status with a custom face.
- [ ] Apply a vanilla face to a high-ranked AREDL level: no leftover infinity/effects.
- [ ] Remove the fake rate: AREDL face and effects return immediately.
- [ ] Reopen the editor after Remove: its default face matches AREDL.
- [ ] Apply/remove repeatedly; no accumulating overlays or crashes.
- [ ] Like/download/refresh a level with a fake rating; the override remains correct.
- [ ] Both Grandpa background/particle disable settings are honored.
- [ ] Fake Rate remains usable without Grandpa installed.
- [ ] If using other difficulty mods, test switching between their faces and Grandpa.

The previously delivered Grandpa package is intentionally unchanged; the user
confirmed that it works in-game. This compatibility extension needs its own
in-game confirmation.
