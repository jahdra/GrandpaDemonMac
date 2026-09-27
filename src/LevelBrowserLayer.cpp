#include <Geode/modify/LevelBrowserLayer.hpp>
#include "SearchContext.h"

class $modify(GrandpaBrowser, LevelBrowserLayer) {
    struct Fields {
        std::vector<int> ids;
        std::size_t page = 0;
        bool loading = false;
    };
    bool init(GJSearchObject* search) {
        if (!search) return false;
        if (auto context = dynamic_cast<GrandpaSearchContext*>(search->getUserObject())) {
            m_fields->ids = context->ids;
            m_fields->page = context->page;
            m_fields->loading = true;
        }
        if (!LevelBrowserLayer::init(search)) return false;
        updateGrandpaControls();
        return true;
    }
    void updateGrandpaControls() {
        if (m_fields->ids.empty()) return;
        auto page = m_fields->page;
        auto count = m_fields->ids.size();
        if (m_pageBtn) m_pageBtn->setVisible(false);
        if (m_lastBtn) m_lastBtn->setVisible(false);
        if (m_pageText) m_pageText->setVisible(false);
        if (m_leftArrow) m_leftArrow->setVisible(!m_fields->loading && page > 0);
        if (m_rightArrow) m_rightArrow->setVisible(!m_fields->loading && page + 1 < grandpa::pageCount(count));
        if (m_countText) m_countText->setString(fmt::format("{} to {} of {}", page * 10 + 1,
            std::min(count, (page + 1) * 10), count).c_str());
    }
    void loadLevelsFinished(CCArray* levels, char const* key, int type) {
        LevelBrowserLayer::loadLevelsFinished(levels, key, type);
        m_fields->loading = false;
        updateGrandpaControls();
    }
    void loadLevelsFailed(char const* key, int type) {
        LevelBrowserLayer::loadLevelsFailed(key, type);
        m_fields->loading = false;
        updateGrandpaControls();
    }
    void loadGrandpaPage(std::size_t page) {
        if (m_fields->loading) return;
        auto search = GrandpaSearchContext::createSearch(m_fields->ids, page);
        if (!search) return;
        m_fields->page = page;
        m_fields->loading = true;
        updateGrandpaControls();
        // Exactly one request, not the vanilla next-page request plus a custom request.
        LevelBrowserLayer::loadPage(search);
    }
    void onNextPage(CCObject* sender) {
        if (m_fields->ids.empty()) return LevelBrowserLayer::onNextPage(sender);
        loadGrandpaPage(m_fields->page + 1);
    }
    void onPrevPage(CCObject* sender) {
        if (m_fields->ids.empty()) return LevelBrowserLayer::onPrevPage(sender);
        if (m_fields->page > 0) loadGrandpaPage(m_fields->page - 1);
    }
    void onRefresh(CCObject* sender) {
        if (!m_fields->ids.empty()) {
            if (m_fields->loading) return;
            m_fields->loading = true;
            updateGrandpaControls();
        }
        LevelBrowserLayer::onRefresh(sender);
    }
};
