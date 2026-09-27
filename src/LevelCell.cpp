#include <Geode/modify/LevelCell.hpp>
#include "ListManager.h"
#include "EffectsManager.h"

namespace {
GJDifficultySprite* findDifficulty(CCNode* node, int depth = 0) {
    if (!node || depth > 4) return nullptr;
    if (auto face = typeinfo_cast<GJDifficultySprite*>(node)) return face;
    for (auto child : CCArrayExt<CCNode*>(node->getChildren()))
        if (auto face = findDifficulty(child, depth + 1)) return face;
    return nullptr;
}
}
class $modify(GrandpaLevelCell, LevelCell) {
    void loadCustomLevelCell() {
        LevelCell::loadCustomLevelCell();
        auto original = findDifficulty(m_mainLayer);
        if (!original) return; // A different UI mod may intentionally remove the face.
        EffectsManager::remove(original, "grandpa-infinity"_spr);
        int type = ListManager::difficultyFor(m_level);
        auto replacement = ListManager::sprite(type, false);
        if (!replacement) return;
        original->setDisplayFrame(replacement->displayFrame());
        auto size = original->getContentSize();
        EffectsManager::addInfinitySymbol({size.width / 2.f, size.height / 2.f}, original, type);
    }
};
