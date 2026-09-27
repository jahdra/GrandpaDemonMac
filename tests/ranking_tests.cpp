#include "RankingData.h"
#include <cstdlib>
#include <iostream>
#include <set>

#define CHECK(expr) do { if (!(expr)) { std::cerr << "FAIL line " << __LINE__ << ": " #expr "\n"; std::exit(1); } } while (false)

matjson::Value json(std::string const& text) {
    auto parsed = matjson::parse(text);
    CHECK(parsed.isOk());
    return parsed.unwrap();
}
int main() {
    using namespace grandpa;
    for (auto [rank, type] : std::vector<std::pair<int, int>>{
        {-1,-1},{0,-1},{1,5},{2,4},{25,4},{26,3},{75,3},{76,2},
        {150,2},{151,1},{250,1},{251,0},{500,0},{501,-1}})
        CHECK(difficulty(rank) == type);
    CHECK(difficulty(1, true) == 4);
    RankingData data;
    CHECK(data.parse(json(R"([
        {"level_id":5000,"position":500},
        {"id":"a-uuid","level_id":100,"position":1,"status":"MainList"},
        {"level_id":2600,"position":26},
        {"level_id":2600,"position":27},
        {"level_id":999,"position":2,"two_player":true},
        {"level_id":998,"position":3,"legacy":true},
        {"level_id":997,"position":4,"status":"Removed"},
        {"level_id":996,"position":5,"status":"Pending"},
        {"level_id":995,"position":6,"status":"Legacy"},
        {"level_id":994,"position":null},
        {"level_id":0,"position":2},
        {"level_id":-1,"position":2},
        {"level_id":1.5,"position":2},
        {"level_id":2147483648,"position":2},
        {"id":1,"position":2},
        {"level_id":"123","position":2},
        {"level_id":9,"position":0},
        {"level_id":9,"position":-1},
        {"level_id":9,"position":2.5},
        {"level_id":9,"position":2147483648},
        null, 42, "string", {}
    ])")));
    CHECK(data.size() == 3);
    CHECK(data.rankOf(2600) == 26); // no rank compression when entries are omitted
    CHECK(data.rankOf(100) == 1);
    CHECK(data.rankOf(999) == -1);
    CHECK(data.category(5, false) == std::vector<int>{100});
    CHECK(data.category(4, true) == std::vector<int>{100});
    CHECK(data.category(5, true).empty());
    for (auto text : {"{}", "null", "[]", "[{}]", "[{\"id\":42}]",
        "[{\"level_id\":1,\"position\":1},{\"level_id\":2,\"position\":1}]"}) {
        CHECK(!data.parse(json(text)));
        CHECK(data.size() == 3);
        CHECK(data.rankOf(100) == 1);
    }
    RankingData cached;
    CHECK(cached.parse(data.serialize()));
    CHECK(cached.rankOf(2600) == 26);
    auto all = matjson::Value::array();
    for (int rank = 510; rank >= 1; --rank)
        all.push(matjson::makeObject({{"level_id", rank + 1000}, {"position", rank}}));
    CHECK(data.parse(all));
    std::set<int> covered;
    std::array<std::size_t, 6> counts{250,100,75,50,24,1};
    for (int type = 0; type < 6; ++type) {
        auto ids = data.category(type, false);
        CHECK(ids.size() == counts[type]);
        for (auto id : ids) CHECK(covered.insert(id).second);
        std::vector<int> paged;
        for (std::size_t page = 0; page < pageCount(ids.size()); ++page) {
            auto query = pageQuery(ids, page);
            CHECK(!query.empty());
            CHECK(query.find('&') == std::string::npos);
            std::size_t start = 0;
            do {
                auto end = query.find(',', start);
                paged.push_back(std::stoi(query.substr(start, end - start)));
                if (end == std::string::npos) break;
                start = end + 1;
            } while (true);
        }
        CHECK(paged == ids);
        CHECK(pageQuery(ids, pageCount(ids.size())).empty());
    }
    CHECK(covered.size() == 500);
    CHECK(pageCount(0) == 0);
    CHECK(pageCount(1) == 1);
    CHECK(pageCount(10) == 1);
    CHECK(pageCount(11) == 2);
    CHECK(pageQuery({}, 0).empty());
    CHECK(pageQuery({1, 2, 3}, std::numeric_limits<std::size_t>::max()).empty());
    CHECK(data.category(4, true).size() == 25);
    CHECK(data.category(-1, false).size() == 10); // ranks outside this mod's top 500
    std::cout << "Ranking, parsing, cache and pagination regression tests passed.\n";
}
