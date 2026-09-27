#pragma once
#include "ListManager.h"

class EffectsManager {
public:
    static void remove(CCNode* parent, char const* id) {
        if (parent) if (auto child = parent->getChildByID(id)) child->removeFromParentAndCleanup(true);
    }
    static void addInfinitySymbol(CCPoint position, CCNode* parent, int type) {
        if (!parent || type != 4 || parent->getChildByID("grandpa-infinity"_spr)) return;
        auto sprite = CCSprite::createWithSpriteFrameName("GrD_demon4_infinity.png"_spr);
        if (!sprite) return;
        sprite->setID("grandpa-infinity"_spr);
        sprite->setPosition(position + CCPoint{-0.4f, 14.f});
        sprite->setScale(0.4f);
        sprite->setColor({255, 233, 136});
        sprite->setOpacity(100);
        sprite->setBlendFunc({GL_ONE, GL_ONE});
        sprite->runAction(CCRepeatForever::create(CCSequence::create(
            CCEaseSineInOut::create(CCFadeTo::create(1.5f, 200)),
            CCEaseSineInOut::create(CCFadeTo::create(1.5f, 60)), nullptr)));
        parent->addChild(sprite, 30);
    }
    static void addBackground(CCNode* parent, int type) {
        if (!parent || type < 2 || Mod::get()->getSettingValue<bool>("infinite-demon-disable")) return;
        auto size = CCDirector::sharedDirector()->getWinSize();
        // All effects belong to this mod. Never recolor, reorder or hide vanilla UI nodes.
        auto container = CCNode::create();
        container->setID("grandpa-background"_spr);
        parent->addChild(container, -1);
        ccColor3B color = type == 5 ? ccColor3B{121, 80, 255} :
            type == 4 ? ccColor3B{249, 249, 165} :
            type == 3 ? ccColor3B{76, 63, 118} : ccColor3B{55, 48, 78};
        for (int i = 0; i < (type >= 4 ? 2 : 1); ++i) {
            auto sprite = CCSprite::create("GrD_demon4_bg.png"_spr);
            if (!sprite || sprite->getContentSize().width <= 0) continue;
            sprite->setScale(size.width / sprite->getContentSize().width);
            sprite->setAnchorPoint({0.5f, 0.f});
            sprite->setPosition({size.width / 2.f, 0.f});
            sprite->setColor(color);
            sprite->setOpacity(0);
            sprite->setBlendFunc({GL_ONE, GL_ONE});
            container->addChild(sprite);
            float duration = i ? 30.f : 20.f;
            sprite->runAction(CCRepeatForever::create(CCSequence::create(
                CCMoveTo::create(duration, {size.width / 2.f, -sprite->getScaledContentSize().height * 0.25f}),
                CCMoveTo::create(0.f, {size.width / 2.f, 0.f}), nullptr)));
            // Bounded integer alpha; the old random expression could underflow GLubyte.
            sprite->runAction(CCRepeatForever::create(CCSequence::create(
                CCFadeTo::create(duration * 0.25f, i ? 70 : 150),
                CCFadeTo::create(duration * 0.25f, 40),
                CCFadeTo::create(duration * 0.25f, i ? 100 : 180),
                CCFadeTo::create(duration * 0.25f, 0), nullptr)));
        }
    }
};
