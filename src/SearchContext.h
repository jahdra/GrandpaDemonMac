#pragma once
#include "ListManager.h"

// Metadata travels with this search only. No global flags can leak into other browsers.
class GrandpaSearchContext : public cocos2d::CCObject {
public:
    std::vector<int> ids;
    std::size_t page = 0;
    static GJSearchObject* createSearch(std::vector<int> const& ids, std::size_t page = 0) {
        auto query = grandpa::pageQuery(ids, page);
        if (query.empty()) return nullptr;
        auto search = GJSearchObject::create(SearchType::Type19, query);
        if (!search) return nullptr;
        auto context = new GrandpaSearchContext;
        context->ids = ids;
        context->page = page;
        search->setUserObject("grandpa-search-context"_spr, context);
        context->release(); // retained by search
        return search;
    }
};
