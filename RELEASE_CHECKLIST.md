# macOS release gate — v1.3.0

Do not describe this release candidate as error-free until its native build and
in-game tests below have been completed. No automated source test can prove all
Geometry Dash hooks, visual positioning or third-party mod combinations correct.

## Automated checks

- [x] Standalone C++ regression tests run locally with ASan and UBSan.
- [x] Tests cover unordered input, integer validation/overflow, unknown UUIDs,
      missing/null fields, duplicate IDs, conflicting ranks, status filtering,
      all difficulty boundaries, saved-cache round trips and invalid refresh retention.
- [x] Tests cover every category/page, partial final pages, no duplicate/missing IDs,
      out-of-range page requests, and disabling Grandpa.
- [ ] GitHub Actions native universal macOS build succeeds.
- [ ] Package verification confirms manifest, resources and arm64/x86_64 slices.

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

The local sandbox is Linux, without Geometry Dash or a macOS runtime. A direct
request to the production AREDL endpoint failed during TLS connection in this
sandbox; endpoint/schema handling was checked against AREDL's published backend
source, not a successful live response. Online/offline integration remains an
explicit in-game test gate rather than an assumed pass.
