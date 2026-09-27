#pragma once
#include <Geode/Geode.hpp>
#include "RankingData.h"

using namespace geode::prelude;

class ListManager {
public:
    inline static grandpa::RankingData rankings;
    static void refresh();
    static bool grandpaDisabled() { return Mod::get()->getSettingValue<bool>("grandpa-demon-disable"); }
    static CCSpriteFrame* frame(int type, bool text) {
        if (type < 0 || type > 5) return nullptr;
        auto name = Mod::get()->expandSpriteName(fmt::format("GrD_demon{}{}.png", type, text ? "_text" : ""));
        // Do not replace a valid vanilla face with a missing-texture placeholder.
        return CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name.c_str());
    }
    static CCSprite* sprite(int type, bool text) {
        auto spriteFrame = frame(type, text);
        return spriteFrame ? CCSprite::createWithSpriteFrame(spriteFrame) : nullptr;
    }
    static int difficultyFor(GJGameLevel* level) {
        if (!level || level->m_stars != 10) return -1;
        return grandpa::difficulty(rankings.rankOf(level->m_levelID), grandpaDisabled());
    }
};
