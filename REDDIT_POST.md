# Reddit draft — review before posting

**Do not post this draft as a release announcement until the pre-publication gates
in `RELEASE_CHECKLIST.md` are complete.** Replace the download placeholder with a
published GitHub prerelease URL. Do not use an expiring Actions artifact as the
permanent download. No Reddit post or GitHub release has been published by the agent.

---

## Suggested title

[macOS / Geode] Grandpa Demon Revived — public beta + optional Fake Rate compatibility

## Suggested post

I've been working on a macOS fork of **Grandpa Demon Revived**. It adds Supreme,
Ultimate, Legendary, Mythical, Silent and Grandpa Demon faces based on AREDL rankings.

The original Mac build worked in my game. The latest **v1.3.1** includes a
featured-glow alignment fix, and there is an optional **unofficial Fake Rate
compatibility build** for choosing the extra faces manually. I'm sharing this as
an **early beta** and would appreciate more Mac testers—not claiming that every
setup or mod combination is verified.

**Requirements**
- Geometry Dash **2.2081**
- Geode **5.10.1** or a compatible newer 5.x version
- macOS; binaries include **Apple Silicon and Intel** slices
- macOS deployment target is **11.0+**, subject to GD/Geode requirements; that is
  not a claim that every supported OS version or both architectures were tested

**Download:** [REPLACE WITH THE PUBLISHED GITHUB PRERELEASE URL]

**Source / details:** https://github.com/jahdra/GrandpaDemonMac/tree/arena/01a0e16c-grandpademonmac

**Install:** Close GD, back up your current mod files and saved data, and copy the
Grandpa Demon `.geode` into Geode's mods folder. Replace an older copy rather than
keeping two versions of the same mod. Then restart.

**Fake Rate is optional.** If you want the integration, replace your existing Fake
Rate file with the separately labeled **Fake Rate (Grandpa Compat)** package.
Do not delete its saved data or keep two Fake Rate copies. It is based on Fake
Rate **1.4.19**, preserves its save format and is not an official Fake Rate release.
A later official update may replace this compatibility patch.

Native Mac builds, sanitizer-backed logic tests, geometry tests and package checks
passed. Those do not replace in-game testing. If something is off, please report
it here or at https://github.com/jahdra/GrandpaDemonMac/issues with your Mac chip,
macOS/GD/Geode versions, other installed UI mods, steps to reproduce and a screenshot.

**Credits:** ItzKiba (original mod), UltraSoda (revival), tcoffa (original Grandpa
sprite), hiimjasmine00 (Fake Rate), AREDL and the Geode community. This fork is
maintained by jahdra, with AI-assisted coding, debugging and test work by the
**Arena AI agent**. This is a community fork, not an official Geode or Arena release.
