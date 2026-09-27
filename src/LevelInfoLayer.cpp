#include <Geode/modify/LevelInfoLayer.hpp>
#include "ListManager.h"
#include "EffectsManager.h"
#include "ParticleManager.h"

class $modify(GrandpaInfoLayer, LevelInfoLayer) {
    struct Fields { int effectKey = -1; };
    void updateLabelValues() {
        LevelInfoLayer::updateLabelValues();
        int type = ListManager::difficultyFor(m_level);
        if (!m_difficultySprite) return;
        auto face = ListManager::frame(type, true);
        if (face) {
            // Replace only the frame. Preserve featured/epic children and their ownership.
            m_difficultySprite->setDisplayFrame(face);
        } else type = -1;
        bool noBG = Mod::get()->getSettingValue<bool>("infinite-demon-disable");
        bool noParticles = Mod::get()->getSettingValue<bool>("particles-disable");
        int key = (type + 1) * 4 + noBG * 2 + noParticles;
        if (m_fields->effectKey == key) return;
        m_fields->effectKey = key;
        EffectsManager::remove(this, "grandpa-background"_spr);
        EffectsManager::remove(this, "grandpa-particles"_spr);
        EffectsManager::remove(m_difficultySprite, "grandpa-infinity"_spr);
        if (type < 0) return;
        auto faceSize = m_difficultySprite->getContentSize();
        EffectsManager::addInfinitySymbol({faceSize.width / 2.f, faceSize.height / 2.f}, m_difficultySprite, type);
        EffectsManager::addBackground(this, type);
        if (noParticles || type < 2) return;
        auto container = CCNode::create();
        container->setID("grandpa-particles"_spr);
        auto position = convertToNodeSpace(m_difficultySprite->convertToWorldSpace(
            {faceSize.width / 2.f, faceSize.height / 2.f + 5.f}));
        container->setPosition(position);
        addChild(container, 10);
        auto add = [container](CCParticleSystem* particle) { if (particle) container->addChild(particle); };
        if (type >= 4) {
            add(ParticleManager::infiniteParticles1(50, type == 5));
            add(ParticleManager::infiniteParticles2(50));
        } else if (type == 3) add(ParticleManager::mythicalParticles(50));
        else add(ParticleManager::legendaryParticles(50));
    }
};
