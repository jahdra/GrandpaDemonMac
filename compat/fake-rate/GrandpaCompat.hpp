#pragma once
#include <string_view>

// No Geode types: this policy is also compiled by the regression tests.
namespace grandpa::fake_rate {
inline constexpr std::string_view revivedID = "ultrasoda.grandpa_demon_revived";
inline constexpr std::string_view legacyID = "itzkiba.grandpa_demon";

constexpr std::string_view preferredModID(bool revivedLoaded, bool legacyLoaded) {
    return revivedLoaded ? revivedID : legacyLoaded ? legacyID : std::string_view{};
}

// Fake Rate saves 0 = no override and 1..6 = Supreme..Grandpa.
// Grandpa's texture filenames use 0..5. Do not parse a partial integer from an
// arbitrary texture name (backgrounds and the infinity symbol are not faces).
constexpr int overrideFromFrame(std::string_view name) {
    if (auto slash = name.find('/'); slash != std::string_view::npos) {
        auto owner = name.substr(0, slash);
        if (owner != revivedID && owner != legacyID) return 0;
        name.remove_prefix(slash + 1);
    }
    constexpr std::string_view prefix = "GrD_demon";
    if (!name.starts_with(prefix)) return 0;
    name.remove_prefix(prefix.size());
    if (name.empty() || name.front() < '0' || name.front() > '5') return 0;
    auto number = name.front() - '0';
    name.remove_prefix(1);
    if (name != ".png" && name != "_text.png") return 0;
    return number + 1;
}
}
