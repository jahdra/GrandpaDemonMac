# Grandpa Demon for macOS

Adds six difficulty faces above Extreme Demon using the [All Rated Extreme Demon List](https://aredl.net/).
Originally created by **ItzKiba**, revived by **UltraSoda**; this fork focuses on macOS compatibility and reliability.

![Difficulties](resources/readme/difficulties.png)

## Compatibility

| Component | Target |
| --- | --- |
| Geometry Dash | **2.2081** |
| Geode | **5.10.1 or a compatible newer 5.x release** |
| macOS | **11.0+**, subject to the game's and Geode's requirements |
| Architecture | Universal **arm64 + x86_64** (Apple Silicon and Intel) |

Version **1.3.1** is a macOS release candidate. Do not install it on GD 2.2074 or Geode 4.
Windows, Android and iOS are not advertised by this fork's package.
A successful build does not replace the in-game checks in [RELEASE_CHECKLIST.md](RELEASE_CHECKLIST.md).

## Install

1. Update Geometry Dash and install the matching [Geode loader](https://geode-sdk.org/).
2. Download `GrandpaDemon-macOS` from a successful run of this repository's
   **Validate and build macOS mod** GitHub Actions workflow.
3. Extract the artifact ZIP. Copy the **`.geode` file**, not the ZIP or source folder,
   into the mods folder opened from Geode's settings. Restart Geometry Dash.
4. Keep only one copy of `ultrasoda.grandpa_demon_revived` installed. This fork retains
   the original mod ID to preserve settings and must not be installed alongside another version of that mod.

## Features

| AREDL rank | Difficulty |
| --- | --- |
| 1 | Grandpa Demon |
| 2–25 | Silent Demon |
| 26–75 | Mythical Demon |
| 76–150 | Legendary Demon |
| 151–250 | Ultimate Demon |
| 251–500 | Supreme Demon |

- Official `position` values determine rank, not API response order. Unranked,
  legacy, removed, pending and two-player variants are excluded; rank gaps are not compressed.
- With **Disable Grandpa Demon** enabled, rank 1 becomes Silent Demon and is included in Silent search.
- Open the regular demon filter, then **Grandpa Search** to pick a category.
  All categories use bounded ten-ID pages; normal searches are unchanged.
- Legendary and higher faces have animated backgrounds and particles.
  **Disable BG Effects** and **Disable Particles** independently reduce visual load.
- The last valid rankings are cached in Geode's saved data. Offline startup uses
  that cache; without a cache, vanilla faces remain usable. Network failures are
  logged without interrupting gameplay with an alert.
- A request times out after 20 seconds. A failed request may retry after one minute
  when returning to the main menu or opening Grandpa Search. A successful list is
  kept for the session; restart to refresh it. Already-open lists/pages may need
  reopening after the first download completes. Cached rankings may be out of date.

## Build on a Mac

Install Xcode 26.3 (or a compatible Apple Clang 17+ toolchain), CMake 3.25+, Ninja, the Geode CLI, and the **Geode
5.10.1 SDK** with macOS binaries. Set `GEODE_SDK` to that SDK checkout. The CI workflow
pins the SDK, bindings revision and build action for a reproducible target.

```sh
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DGEODE_TARGET_PLATFORM=MacOS \
  '-DCMAKE_OSX_ARCHITECTURES=arm64;x86_64' \
  -DGEODE_DONT_INSTALL_MODS=ON
cmake --build build --parallel 2
python3 scripts/verify_package.py build
```

The verifier checks the packaged manifest, both Mach-O architecture slices and
required difficulty sprite frames. Do not rename another platform's binary or
just edit `mod.json` inside an old `.geode`; that does not port a mod to macOS.

## Standalone regression tests

These compile the **same parser, ranking and paging implementation used by the mod**
against Geode's actual matjson 3.3.1 library; they do not need Geometry Dash or Geode.
CMake downloads the pinned test dependencies on first use.

```sh
cmake -S . -B build-tests \
  -DGRANDPA_BUILD_MOD=OFF -DGRANDPA_BUILD_TESTS=ON \
  -DCMAKE_BUILD_TYPE=Debug \
  '-DCMAKE_CXX_FLAGS=-fsanitize=address,undefined -fno-omit-frame-pointer'
cmake --build build-tests --parallel 2
ctest --test-dir build-tests --output-on-failure
```

## Credits

- **ItzKiba** — original mod
- **UltraSoda** — Grandpa Demon revival
- **tcoffa** — original Grandpa Demon sprite
- **AREDL** — rankings and API
- **Geode contributors and community** — mod loader, SDK and bindings
- **AeonAir** — inspiration for the original mod
