#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>
#include "ListManager.h"
#include <chrono>

using namespace geode::prelude;

void ListManager::refresh() {
    // Owned by the mod, not a MenuLayer that may be destroyed during the request.
    static async::TaskHolder<web::WebResponse> request;
    static bool cacheLoaded = false;
    static bool refreshed = false;
    static auto nextAttempt = std::chrono::steady_clock::time_point::min();
    if (!cacheLoaded) {
        cacheLoaded = true;
        auto cache = Mod::get()->getSavedValue<matjson::Value>("aredl-cache-v2");
        if (rankings.parse(cache)) log::info("Loaded {} cached AREDL rankings", rankings.size());
    }
    auto now = std::chrono::steady_clock::now();
    if (refreshed || request.isPending() || now < nextAttempt) return;
    nextAttempt = now + std::chrono::seconds(60);
    auto req = web::WebRequest();
    req.userAgent("GrandpaDemon/1.3.0").timeout(std::chrono::seconds(20));
    request.spawn(req.get("https://api.aredl.net/v2/api/aredl/levels?exclude_legacy=true&exclude_pending=true&exclude_removed=true"),
        [](web::WebResponse response) {
            if (!response.ok()) {
                log::warn("AREDL unavailable ({}): {}. Keeping cached rankings; vanilla faces if no cache.",
                    response.code(), response.errorMessage());
                return;
            }
            auto data = response.json();
            if (!data || !rankings.parse(data.unwrap())) {
                log::warn("Rejected invalid AREDL response; keeping cached rankings");
                return;
            }
            refreshed = true;
            Mod::get()->setSavedValue("aredl-cache-v2", rankings.serialize());
            log::info("Loaded {} AREDL rankings", rankings.size());
        });
}

class $modify(GrandpaMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        ListManager::refresh();
        return true;
    }
};
