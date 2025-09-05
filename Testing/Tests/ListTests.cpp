#include "Polly/List.hpp"
#include "Polly/String.hpp"
#include <snitch/snitch.hpp>

using namespace Polly; // NOLINT(*-build-using-namespace)

template<typename T, u32 InlineCapacity = 0>
requires(std::is_default_constructible_v<T>)
void testListBasics(const T& dummyValue)
{
    using List = List<T, InlineCapacity>;

    auto l = List();

    REQUIRE(l.isEmpty());
    REQUIRE(l.size() == 0);

    l.add(T());
    REQUIRE_FALSE(l.isEmpty());
    REQUIRE(l.size() == 1);
    REQUIRE(l[0] == T());

    l.add(dummyValue);
    REQUIRE_FALSE(l.isEmpty());
    REQUIRE(l.size() == 2);
    REQUIRE(l[1] == dummyValue);

    l.clear();
    REQUIRE(l.isEmpty());
    REQUIRE(l.size() == 0);

    l.add(dummyValue);
    REQUIRE_FALSE(l.isEmpty());
    REQUIRE(l.size() == 1);
    REQUIRE(l[0] == dummyValue);

    l.add(T());
    REQUIRE_FALSE(l.isEmpty());
    REQUIRE(l.size() == 2);
    REQUIRE(l[1] == T());
}

TEST_CASE("List basics", "[stl]")
{

    testListBasics<int>(4);
    testListBasics<int, 4>(4);
    testListBasics<String>("hello");
    testListBasics<String, 1>("hello");
    testListBasics<String, 8>("hello");
    testListBasics<void*>(reinterpret_cast<void*>(0x48902132109));
    testListBasics<void*, 4>(reinterpret_cast<void*>(0x48902132109));
}

TEST_CASE("List copy ctor small to small", "[stl]")
{
}

TEST_CASE("List move_item_at", "[stl]")
{
    auto list = List{1, 2, 3};

    list.moveItemAt(2, 0);
    auto expected = List{3, 1, 2};
    REQUIRE(list == expected);

    list.moveItemAt(0, 0);
    REQUIRE(list == expected);

    list.moveItemAt(0, 2);
    expected = {1, 2, 3};
    REQUIRE(list == expected);

    list.moveItemAt(0, 1);
    expected = {2, 1, 3};
    REQUIRE(list == expected);

    {
        list.clear();
        list.resize(72);
        for (auto i = 0; auto& val : list)
        {
            val = i++;
        }

        auto insertionIt = list.begin() + 48;
        REQUIRE(*insertionIt == 48);

        insertionIt = list.moveItemAtIterator(list.begin() + 52, insertionIt + 1);
        REQUIRE(list[49] == 52);

        insertionIt = list.moveItemAtIterator(list.begin() + 53, insertionIt + 1);
        REQUIRE(list[50] == 53);

        insertionIt = list.moveItemAtIterator(list.begin() + 55, insertionIt + 1);
        REQUIRE(list[51] == 55);

        insertionIt = list.moveItemAtIterator(list.begin() + 61, insertionIt + 1);
        REQUIRE(list[52] == 61);

        insertionIt = list.moveItemAtIterator(list.begin() + 67, insertionIt + 1);
        REQUIRE(list[53] == 67);

        insertionIt = list.moveItemAtIterator(list.begin() + 68, insertionIt + 1);
        REQUIRE(list[54] == 68);
    }
}

struct BrokenState
{
    List<void*, 4> list;
};

TEST_CASE("List broken state", "[stl]")
{
    auto s = BrokenState();
    s      = {};
    s.list.add(nullptr);
    s.list.add(nullptr);
    s.list.add(nullptr);
    s.list.add(nullptr);
    s.list.add(nullptr);
}
