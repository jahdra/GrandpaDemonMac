#pragma once

#include <matjson.hpp>
#include <algorithm>
#include <array>
#include <limits>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace grandpa {
// Positions are one-based in the API. Never infer rank from response order.
struct RankedLevel { int id; int rank; };

inline std::optional<int> positiveInt(matjson::Value const& value) {
    if (!value.isExactlyInt() && !value.isExactlyUInt()) return std::nullopt;
    auto number = value.asInt();
    if (!number || number.unwrap() <= 0 || number.unwrap() > std::numeric_limits<int>::max())
        return std::nullopt;
    return static_cast<int>(number.unwrap());
}

inline int difficulty(int rank, bool disableGrandpa = false) {
    if (rank < 1 || rank > 500) return -1;
    if (rank == 1 && !disableGrandpa) return 5;
    if (rank <= 25) return 4;
    if (rank <= 75) return 3;
    if (rank <= 150) return 2;
    if (rank <= 250) return 1;
    return 0;
}

class RankingData {
    std::vector<RankedLevel> m_levels;
    std::unordered_map<int, int> m_ranks;
public:
    // Transactional: invalid responses must not destroy the last usable list.
    bool parse(matjson::Value const& data) {
        if (!data.isArray()) return false;
        std::unordered_map<int, int> ranks;
        for (auto const& item : data.asArray().unwrap()) {
            if (!item.isObject()) continue;
            if (item.contains("status") && item["status"].asString().unwrapOr("") != "MainList") continue;
            if (item.contains("legacy") && item["legacy"].asBool().unwrapOr(true)) continue;
            if (item.contains("two_player") && item["two_player"].asBool().unwrapOr(true)) continue;
            auto id = positiveInt(item["level_id"]);
            auto rank = positiveInt(item["position"]);
            if (!id || !rank) continue;
            auto [it, inserted] = ranks.emplace(*id, *rank);
            if (!inserted) it->second = std::min(it->second, *rank);
        }
        if (ranks.empty()) return false;
        std::vector<RankedLevel> levels;
        for (auto [id, rank] : ranks) levels.push_back({id, rank});
        std::sort(levels.begin(), levels.end(), [](auto a, auto b) {
            return a.rank != b.rank ? a.rank < b.rank : a.id < b.id;
        });
        // Conflicting official positions indicate a corrupt/incompatible response.
        for (std::size_t i = 1; i < levels.size(); ++i)
            if (levels[i - 1].rank == levels[i].rank) return false;
        m_levels = std::move(levels);
        m_ranks = std::move(ranks);
        return true;
    }
    int rankOf(int id) const {
        auto it = m_ranks.find(id);
        return it == m_ranks.end() ? -1 : it->second;
    }
    bool empty() const { return m_levels.empty(); }
    std::size_t size() const { return m_levels.size(); }
    std::vector<int> category(int type, bool disableGrandpa) const {
        std::vector<int> ids;
        for (auto level : m_levels)
            if (difficulty(level.rank, disableGrandpa) == type) ids.push_back(level.id);
        return ids;
    }
    matjson::Value serialize() const {
        auto array = matjson::Value::array();
        for (auto level : m_levels)
            array.push(matjson::makeObject({{"level_id", level.id}, {"position", level.rank}}));
        return array;
    }
};

inline constexpr std::size_t pageSize = 10;
inline std::size_t pageCount(std::size_t count) { return (count + pageSize - 1) / pageSize; }
inline std::string pageQuery(std::vector<int> const& ids, std::size_t page) {
    if (page >= pageCount(ids.size())) return {};
    std::string query;
    for (auto i = page * pageSize; i < std::min(ids.size(), (page + 1) * pageSize); ++i) {
        if (!query.empty()) query += ',';
        query += std::to_string(ids[i]);
    }
    return query;
}
}
