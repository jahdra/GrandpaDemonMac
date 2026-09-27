# macOS release gate — v1.3.1

Do not describe this release candidate as error-free, even after its native build
and in-game tests below have been completed. No automated source test can prove all
Geometry Dash hooks, visual positioning or third-party mod combinations correct.

## Current evidence and recommendation

**Status: release candidate; public beta only after the pre-publication gates below.**

- The maintainer reported that **v1.3.0** loaded and worked correctly in-game.
  This does not establish results on both Mac architectures or all configurations.
- **v1.3.1** built successfully with both architecture slices and passed geometry,
  ranking and package checks: [validated code run](https://github.com/jahdra/GrandpaDemonMac/actions/runs/36334917764).
- **Fake Rate compatibility build 1** built successfully and passed its automated
  checks: [validated build](https://github.com/jahdra/GrandpaDemonMac/actions/runs/36334418160).
- No explicit in-game confirmation of the Fake Rate extension or latest glow fix
  has been recorded yet. Do not describe those as runtime-tested.

## Pre-publication gates (including a public beta)

- [ ] Confirm the redistribution license or obtain permission for original Grandpa
      Demon code and artwork. No explicit license was identified in this checkout
      or the referenced UltraSodaa/GrandpaDemon upstream repository. Do not invent
      a license or assume the separate Fake Rate MIT license covers those assets.
- [ ] On the maintainer's Mac, confirm **v1.3.1** starts cleanly and the affected
      featured/glowing faces align in level pages and full/compact cells.
- [ ] If offering the optional Fake Rate package, confirm all six face selections,
      save/restart persistence and Remove restoring AREDL faces. Repeat with v1.3.1.
- [ ] Publish the exact tested `.geode` files as a **GitHub prerelease** with
      checksums, requirements, known limitations and a bug-report link. Actions
      artifacts expire and are not suitable as permanent Reddit download links.
- [ ] Label the optional Fake Rate build **unofficial**; retain its MIT license,
      author credits, original mod ID and clear replace-not-duplicate instructions.
- [ ] Describe Intel/Apple Silicon as **built for**, not **tested on both**, unless
      both have actually been tested in-game. Request independent beta testers.

Suggested beta report fields: Mac model/chip, macOS version, GD/Geode versions,
mod versions, other UI mods, reproduction steps, screenshot and relevant Geode log.
Remove personal information from logs before posting them publicly.

## Automated checks

- [x] Standalone C++ regression tests run locally with ASan and UBSan.
- [x] Tests cover unordered input, integer validation/overflow, unknown UUIDs,
      missing/null fields, duplicate IDs, conflicting ranks, status filtering,
      all difficulty boundaries, saved-cache round trips and invalid refresh retention.
- [x] Tests cover every category/page, partial final pages, no duplicate/missing IDs,
      out-of-range page requests, and disabling Grandpa.
- [x] GitHub Actions native universal macOS build succeeds (Xcode 26.3).
- [x] Package verification confirms manifest, resources and arm64/x86_64 slices.
- [x] Face-alignment CPU regression tests reproduce the old geometry shift and
      check repeated updates, frame trimming/rotation and return to vanilla.
      These tests do not render Geometry Dash.

## Required in-game checks (not runnable in the Linux development sandbox)

Record OS, chip, GD/Geode versions, installed mods and results for each machine.
Test **Apple Silicon native** and **Intel/x86_64** separately. Rosetta on Apple
Silicon is a useful additional check but is not a substitute for an Intel test.

- [ ] Install only the new package and launch GD 2.2081 with Geode 5.10.1.
- [ ] First launch online: wait for the AREDL success log, then open ranked levels.
- [ ] Confirm ranks 1, 2, 25/26, 75/76, 150/151, 250/251 and 500/501 against AREDL.
- [ ] Verify full and compact level cells, 2.2 lists, search results and level pages.
- [ ] Featured/epic/legendary/mythic decorations remain visible and correctly positioned.
- [ ] Open every Grandpa Search category. Traverse first/last and partial pages;
      navigate backward and enter a level then return. Check normal searches afterward.
- [ ] Rapidly click page arrows and refresh; no doubled requests or stuck controls.
- [ ] Simulate a failed level download, refresh/retry, then navigate away and back.
- [ ] Toggle Grandpa off: rank 1 uses Silent and appears in Silent search.
- [ ] Toggle backgrounds and particles independently; reopen the level page to apply.
- [ ] Reopen/refresh a high-tier level at least 30 times: no duplicated faces,
      accumulating effects, crashes or continually increasing memory.
- [ ] Leave the menu immediately during an AREDL download: no use-after-free/crash.
- [ ] Restart offline with cache: cached ranks load. Restart offline without cache:
      vanilla faces work and Grandpa Search explains unavailability.
- [ ] Disconnect during a download; reconnect after a minute and retry via the menu.
- [ ] Check windowed/full-screen, Retina scaling and supported aspect ratios.
- [ ] Confirm particle animation/performance on battery and a lower-powered Mac.
- [ ] Repeat with the user's usual UI/difficulty mods, then inspect Geode logs.

## Publish

- [ ] Resolve all failures above; attach test evidence and known limitations.
- [ ] Download the artifact from the final successful commit's workflow run.
- [ ] Re-run `python3 scripts/verify_package.py <file.geode>` on a Mac.
- [ ] Publish that exact tested artifact with its commit and SHA-256 checksum.

### Environment limitations

The local sandbox is Linux, without Geometry Dash or a macOS runtime. Direct shell TLS access to AREDL is restricted in this sandbox. The production
endpoint and response schema were confirmed through the web retrieval tool and
AREDL's published backend source. CI also probes the full live response separately
from the deterministic tests (service availability does not block a build). Online/offline integration remains an
explicit in-game test gate rather than an assumed pass.
