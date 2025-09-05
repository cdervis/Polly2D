#include "Polly/Array.hpp"
#include "Polly/SortedMap.hpp"
#include <snitch/snitch.hpp>

using namespace Polly; // NOLINT(*-build-using-namespace)

TEST_CASE("SortedMap basics", "[stl]")
{
    auto map1 = SortedMap<int, float>();
    auto map2 = SortedMap<int, float>();

    assert(map1.isEmpty());
    REQUIRE(map1 == map2);

    map1 = map2;
    REQUIRE(map1 == map2);

    REQUIRE(map1.size() == 0);

    map1 = {
        {1, 2.0f},
        {3, 4.0f},
        {5, 6.0f},
    };

    {
        auto it = map1.begin();
        REQUIRE(*it == Pair(1, 2.0f));
        ++it;
        REQUIRE(*it == Pair(3, 4.0f));
        ++it;
        REQUIRE(*it == Pair(5, 6.0f));
        ++it;
        REQUIRE(it == map1.end());
    }

    REQUIRE_FALSE(map1.isEmpty());
    REQUIRE(map1.size() == 3);
    REQUIRE(map1 != map2);

    map2 = map1;
    REQUIRE(map1.size() == 3);
    REQUIRE(map2.size() == 3);
    REQUIRE(map1 == map2);

    map1 = std::move(map2);
    REQUIRE(map2.isEmpty());
    REQUIRE(map2.size() == 0);
    REQUIRE(map1 != map2);
}

TEST_CASE("SortedMap insertion and removal", "[stl]")
{
    auto map = SortedMap<String, float>();

    map.add(Pair("hello", 2.0f));
    REQUIRE(map.size() == 1);

    map.add("world", 4.0f);
    REQUIRE(map.size() == 2);

    map.remove("hello");
    REQUIRE(map.size() == 1);

    REQUIRE_FALSE(map.add("world", 5.0f));
    REQUIRE(map.size() == 1);
    REQUIRE(map.add("hello", 2.0f));
    REQUIRE(map.size() == 2);

    {
        auto opt = map.add("inserted", 1.0f);
        REQUIRE(opt);
        REQUIRE(opt->first == "inserted");
        REQUIRE(opt->second == 1.0f);

        REQUIRE(map.size() == 3);
    }

    map.addRange(
        Array{
            Pair("first", 10.0f),
            Pair("second", 20.0f),
            Pair("third", 30.0f),
        });

    REQUIRE(map.size() == 6);
    REQUIRE(map.contains("hello"));
    REQUIRE(map.contains("world"));
    REQUIRE(map.contains("inserted"));
    REQUIRE(map.contains("first"));
    REQUIRE(map.contains("second"));
    REQUIRE(map.contains("third"));

    // Removes "inserted", "first" and "third":
    map.removeWhere([](const Pair<String, float>& pair) { return pair.first.contains('i'); });

    REQUIRE(map.size() == 3);

    REQUIRE_FALSE(map.remove("doesnotexist"));
    map.clear();
    REQUIRE(map.isEmpty());
    REQUIRE(map.size() == 0);

    map = {
        Pair("first", 10.0f),
        Pair("second", 20.0f),
        Pair("third", 30.0f),
    };

    REQUIRE(map.size() == 3);

    {
        auto& ref = map["first"];
        REQUIRE(ref == 10.0f);
        ref = 40.0f;
        REQUIRE(map["first"] == 40.0f);

        map["second"] = 50.0f;
    }

    {
        auto opt = map.find("second");
        REQUIRE(opt);
        *opt = 1.0f;
        REQUIRE(opt == 1.0f);
        REQUIRE(map["second"] == 1.0f);
    }
}
