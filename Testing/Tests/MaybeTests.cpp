#include "Polly/Any.hpp"
#include "Polly/SortedMap.hpp"
#include <snitch/snitch.hpp>

using namespace Polly; // NOLINT(*-build-using-namespace)

struct Trivial
{
    int value;

    String toString() const
    {
        return Polly::toString(value);
    }
};

struct NonTrivial
{
    String value;

    String toString() const
    {
        return value;
    }
};

static_assert(std::is_trivial_v<Trivial>);
static_assert(std::is_trivially_destructible_v<Trivial>);
static_assert(! std::is_trivial_v<NonTrivial>);
static_assert(! std::is_trivially_destructible_v<NonTrivial>);

bool operator==(const Trivial& lhs, const Trivial& rhs)
{
    return lhs.value == rhs.value;
}

bool operator==(const NonTrivial& lhs, const NonTrivial& rhs)
{
    return lhs.value == rhs.value;
}

TEST_CASE("Maybe basics", "[stl]")
{
    REQUIRE(Maybe<int>() == none);
    REQUIRE(Maybe<int>(0) == 0);

    {
        auto opt1 = Maybe<int>();
        REQUIRE_FALSE(opt1);
        REQUIRE(opt1.valueOr(1) == 1);
        REQUIRE(opt1.valueOr(1u) == 1);
        REQUIRE(opt1.valueOr(4) == 4);
        REQUIRE(opt1.valueOr(4u) == 4);
        opt1 = 10;
        REQUIRE(opt1);
        REQUIRE(*opt1 == 10);
        REQUIRE(opt1.valueOr(3) == 10);
        REQUIRE(opt1.valueOr(3u) == 10);

        auto opt2 = opt1;
        REQUIRE(opt1 == opt2);

        opt2 = 5u;
        REQUIRE_FALSE(opt1 == opt2);
    }
}

TEST_CASE("Maybe with SortedMap and Any", "[stl]")
{
    auto map = SortedMap<StringView, Any>();

    auto inserted_pair1 = map.add("Test", 10.0f);
    auto inserted_pair2 = map.add("SomeString", "Hello World!"_s);

    {
        auto value = map.find("Test");
        static_assert(std::is_same_v<decltype(value), Maybe<Any&>>);

        REQUIRE(value);
        REQUIRE(&*value == &inserted_pair1->second);
        REQUIRE(value->get<float>() == 10.0f);
    }

    {
        auto value = map.find("SomeString");
        static_assert(std::is_same_v<decltype(value), Maybe<Any&>>);

        REQUIRE(value);
        REQUIRE(&*value == &inserted_pair2->second);
        REQUIRE(value->get<String>() == "Hello World!");
    }

    {
        auto value = map.find("notExisting");
        static_assert(std::is_same_v<decltype(value), Maybe<Any&>>);

        auto any = value.valueOr("fallback"_sv);
        static_assert(std::is_same_v<decltype(any), Any>);
        REQUIRE(any);
        REQUIRE(any.type() == AnyType::StringView);
        REQUIRE(any.get<StringView>() == "fallback");
    }
}

TEST_CASE("Maybe constexpr", "[stl]")
{
    constexpr auto opt1 = Maybe(0.0f);
    constexpr auto opt2 = Maybe<float>();

    static_assert(opt1 == 0.0f);
    static_assert(opt1 != opt2);

    static_assert(sizeof(Maybe<int&>) == sizeof(int*));
    static_assert(sizeof(Maybe<NonTrivial&>) == sizeof(NonTrivial*));
    static_assert(sizeof(Maybe<Trivial&>) == sizeof(Trivial*));
}

void test_option_ref()
{
    static_assert(std::is_same_v<Maybe<int&>::value_type, int&>, "Option<T&> is ill-formed!");

    {
        auto obj = Trivial{.value = 1};

        auto ref_opt = Maybe<Trivial&>();
        REQUIRE_FALSE(ref_opt);

        ref_opt = obj;
        REQUIRE(ref_opt);
        REQUIRE(ref_opt == obj);
        REQUIRE(ref_opt == Maybe<Trivial&>(obj));
        REQUIRE(&*ref_opt == &obj);

        auto ref_opt2 = ref_opt;
        REQUIRE(ref_opt == ref_opt2);
        REQUIRE(&*ref_opt == &*ref_opt2);

        auto& [value] = *ref_opt2;
        REQUIRE(value == obj.value);
        REQUIRE(ref_opt->value == ref_opt2->value);

        REQUIRE(ref_opt.valueOr(Trivial{.value = 1}) == obj);

        ref_opt = none;
        REQUIRE(ref_opt.valueOr(Trivial{.value = 1}) == Trivial{.value = 1});

        REQUIRE(ref_opt.valueOr(5) == Trivial{.value = 5});
    }

    {
        auto obj = NonTrivial{.value = "two"};

        auto ref_opt = Maybe<NonTrivial&>();
        REQUIRE_FALSE(ref_opt);

        ref_opt = obj;
        REQUIRE(ref_opt);
        REQUIRE(ref_opt == obj);
        REQUIRE(ref_opt == Maybe<NonTrivial&>(obj));
        REQUIRE(&*ref_opt == &obj);

        auto ref_opt2 = ref_opt;
        REQUIRE(ref_opt == ref_opt2);
        REQUIRE(&*ref_opt == &*ref_opt2);

        auto& [value] = *ref_opt2;
        REQUIRE(value == obj.value);
        REQUIRE(ref_opt->value == ref_opt2->value);

        REQUIRE(ref_opt.valueOr(NonTrivial{.value = "two"}) == obj);

        ref_opt = none;
        REQUIRE(ref_opt.valueOr(NonTrivial{.value = "fallback"}) == NonTrivial{.value = "fallback"});

        REQUIRE(ref_opt.valueOr("implicit conversion"_s) == NonTrivial{.value = "implicit conversion"});
    }

    // Test swap
    {
        auto obj1 = Trivial{.value = 1};
        auto obj2 = Trivial{.value = 2};
        auto opt1 = Maybe<Trivial&>(obj1);
        auto opt2 = Maybe<Trivial&>(obj2);

        REQUIRE(opt1 != opt2);
        REQUIRE(&*opt1 == &obj1);
        REQUIRE(&*opt2 == &obj2);
        REQUIRE(opt1 == Trivial{.value = 1});
        REQUIRE(opt2 == Trivial{.value = 2});

        std::swap(opt1, opt2);
        REQUIRE(&*opt1 == &obj2);
        REQUIRE(&*opt2 == &obj1);
        REQUIRE(opt1 == Trivial{.value = 2});
        REQUIRE(opt2 == Trivial{.value = 1});
    }
}

TEST_CASE("Maybe ptr", "[stl]")
{
    static_assert(std::is_same_v<Maybe<int*>::value_type, int*>, "Option<T*> is ill-formed!");
}

TEST_CASE("Maybe valueOr overloads", "[stl]")
{
}

TEST_CASE("Maybe non-trivial destruction", "[stl]")
{
}
