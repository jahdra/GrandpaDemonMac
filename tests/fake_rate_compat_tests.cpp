#include "GrandpaCompat.hpp"
#include <cstdlib>
#include <iostream>
#include <string>

#define CHECK(x) do { if (!(x)) { std::cerr << "Failure at line " << __LINE__ << ": " #x "\n"; std::exit(1); } } while (false)

int main() {
    using namespace grandpa::fake_rate;
    CHECK(preferredModID(false, false).empty());
    CHECK(preferredModID(false, true) == legacyID);
    CHECK(preferredModID(true, false) == revivedID);
    CHECK(preferredModID(true, true) == revivedID);
    for (auto provider : {std::string_view{}, revivedID, legacyID}) {
        auto prefix = provider.empty() ? std::string{} : std::string(provider) + '/';
        for (int type = 0; type < 6; ++type) {
            for (auto suffix : {".png", "_text.png"}) {
                CHECK(overrideFromFrame(prefix + "GrD_demon" + std::to_string(type) + suffix) == type + 1);
            }
        }
        for (auto bad : {"", "GrD_demon", "GrD_demon-1.png", "GrD_demon6.png", "GrD_demon10.png",
                         "GrD_demon0", "GrD_demon4_infinity.png", "GrD_demon4_bg.png", "GrD_demon5_text.png.extra",
                         "GrD_demon0garbage.png", "GrD_demon5_text-hd.png", "difficulty_10_btn2_001.png"}) {
            CHECK(overrideFromFrame(prefix + bad) == 0);
        }
    }
    CHECK(overrideFromFrame("other.mod/GrD_demon5_text.png") == 0);
    CHECK(overrideFromFrame("ultrasoda.grandpa_demon_revived/subdir/GrD_demon0.png") == 0);
    // Prevent recurrence of the upstream off-by-one: Supreme must not mean "none".
    static_assert(overrideFromFrame("GrD_demon0_text.png") == 1);
    static_assert(overrideFromFrame("GrD_demon5_text.png") == 6);
    std::cout << "Fake Rate provider selection and all six face mappings passed.\n";
}
