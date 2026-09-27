# v1.3.1 — featured glow alignment
* Preserve the vanilla difficulty sprite canvas when applying a custom face, so featured/epic/legendary/mythic decorations retain their original alignment.
* Recompute the drawable texture quad with the custom frame's trim/rotation data; avoid moving child nodes or changing shared sprite frames.
* Apply the same correction to level pages and full/compact level cells.
* Keep ratings, settings and Fake Rate save data unchanged. The existing Fake Rate compatibility build remains supported.
* Add regression tests for all six face sizes, trimmed/rotated frames, repeated updates, feature-state changes and return to vanilla.

# v1.3.0 — macOS release candidate
* Target GD 2.2081 and Geode 5.10.1 with a universal Apple Silicon/Intel build.
* Use the AREDL v2 endpoint and official positions; validate IDs, ranks and status.
* Cache valid rankings, bound request time, retry failures and preserve vanilla fallback.
* Replace global search flags with per-search snapshots and bounded pagination.
* Preserve difficulty sprite children instead of unsafely re-parenting them.
* Prevent duplicated effects, null dereferences and particle dictionary leaks.
* Avoid temporary sprite allocations during face updates and stop loading unused README textures.
* Add independent regression tests, macOS CI and package verification.
* Native in-game validation is required before publication; see RELEASE_CHECKLIST.md.

# v1.2.1
* Made compatible with 2.2074
* Replaced Infinite Demon with Silent Demon difficulty

# v1.2.0
* Made compatible with 2.206
* Fixed a crash that occured when trying to search for Grandpa Demons with Grandpa Demon disabled

# v1.1.0

* Added Instant Search feature for custom demon difficulty types.
* Added particles to Legendary, Mythical, and Infinite Demon level info pages (can be disabled in Mod settings)

# v1.0.1

* Initial Release