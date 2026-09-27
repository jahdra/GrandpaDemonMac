#include <Geode/modify/DemonFilterSelectLayer.hpp>
#include <Geode/ui/Popup.hpp>
#include "SearchContext.h"

class GrandpaSearchPopup : public Popup {
    bool setup() {
        if (!Popup::init(350.f, 210.f)) return false;
        setTitle("Grandpa Demon Search");
        auto disabled = ListManager::grandpaDisabled();
        for (int type = 0; type < (disabled ? 5 : 6); ++type) {
            auto sprite = ListManager::sprite(type, true);
            if (!sprite) continue;
            auto button = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(GrandpaSearchPopup::onSearch));
            button->setTag(type);
            button->setPosition({75.f + (type % 3) * 100.f, 130.f - (type / 3) * 75.f});
            m_buttonMenu->addChild(button);
        }
        return true;
    }
    void onSearch(CCObject* sender) {
        auto node = typeinfo_cast<CCNode*>(sender);
        if (!node) return;
        auto ids = ListManager::rankings.category(node->getTag(), ListManager::grandpaDisabled());
        auto search = GrandpaSearchContext::createSearch(ids);
        if (!search) {
            FLAlertLayer::create("No rankings", "No ranked levels are available in this category yet. Please try again after AREDL finishes loading.", "OK")->show();
            ListManager::refresh();
            return;
        }
        if (auto browser = LevelBrowserLayer::create(search)) geode::cocos::switchToScene(browser);
    }
public:
    static GrandpaSearchPopup* create() {
        auto popup = new GrandpaSearchPopup;
        if (popup->setup()) { popup->autorelease(); return popup; }
        delete popup;
        return nullptr;
    }
};

class $modify(GrandpaDemonFilter, DemonFilterSelectLayer) {
    bool init() {
        if (!DemonFilterSelectLayer::init()) return false;
        // Keep the vanilla filter and its delegate untouched. A separate picker avoids
        // relying on undocumented child ordering or resizing another mod's popup.
        auto menu = CCMenu::create();
        menu->setID("grandpa-search-menu"_spr);
        auto size = CCDirector::sharedDirector()->getWinSize();
        menu->setPosition({size.width / 2.f, size.height / 2.f - 125.f});
        auto label = ButtonSprite::create("Grandpa Search", "goldFont.fnt", "GJ_button_01.png", 0.6f);
        if (!label) return true;
        label->setScale(0.65f);
        menu->addChild(CCMenuItemSpriteExtra::create(label, this, menu_selector(GrandpaDemonFilter::onGrandpaSearch)));
        addChild(menu, 10);
        return true;
    }
    void onGrandpaSearch(CCObject*) {
        ListManager::refresh();
        if (ListManager::rankings.empty()) {
            FLAlertLayer::create("AREDL unavailable", "Rankings are still loading or AREDL is unavailable. Vanilla difficulties remain active. Please try again shortly; failed requests can retry after one minute.", "OK")->show();
            return;
        }
        if (auto popup = GrandpaSearchPopup::create()) popup->show();
    }
};
