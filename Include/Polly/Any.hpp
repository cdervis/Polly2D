// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Color.hpp"
#include "Polly/Linalg.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/String.hpp"
#include "Polly/StringView.hpp"

namespace Polly
{
enum class AnyType
{
    None        = 0,
    Char        = 1,
    UChar       = 2,
    Short       = 3,
    UShort      = 4,
    Int         = 5,
    UInt        = 6,
    Float       = 7,
    Double      = 8,
    Bool        = 9,
    Vec2        = 10,
    Vec3        = 11,
    Vec4        = 12,
    Color       = 13,
    Matrix      = 14,
    String      = 15,
    StringView  = 16,
    VoidPointer = 17,
};

namespace Concepts
{
template<typename T>
concept SupportedByAny = std::is_same_v<T, char>
                         or std::is_same_v<T, unsigned char>
                         or std::is_same_v<T, short>
                         or std::is_same_v<T, unsigned short>
                         or std::is_same_v<T, int>
                         or std::is_same_v<T, unsigned int>
                         or std::is_same_v<T, float>
                         or std::is_same_v<T, double>
                         or std::is_same_v<T, bool>
                         or std::is_same_v<T, Vec2>
                         or std::is_same_v<T, Vec3>
                         or std::is_same_v<T, Vec4>
                         or std::is_same_v<T, Color>
                         or std::is_same_v<T, Matrix>
                         or std::is_same_v<T, String>
                         or std::is_same_v<T, StringView>
                         or std::is_same_v<T, void*>;

template<typename T>
concept AssignableToAny = SupportedByAny<std::remove_cvref_t<T>>;
} // namespace concepts

class Any final
{
  public:
    // ReSharper disable once CppPossiblyUninitializedMember
    Any() // NOLINT(*-pro-type-member-init)
        : _type(AnyType::None)
    {
    }

    // ReSharper disable once CppPossiblyUninitializedMember
    // ReSharper disable once CppNonExplicitConvertingConstructor
    Any(Details::NoObjectTag)
        : Any()
    {
    }

    // ReSharper disable once CppNonExplicitConvertingConstructor
    template<Concepts::AssignableToAny T>
    Any(T&& value); // NOLINT(*-explicit-constructor)

    Any(const Any& copyFrom);

    Any& operator=(const Any& copyFrom);

    Any(Any&& moveFrom) noexcept;

    Any& operator=(Any&& moveFrom) noexcept;

    ~Any() noexcept;

    AnyType type() const;

    template<Concepts::SupportedByAny T>
    [[nodiscard]]
    T& get() &;

    template<Concepts::SupportedByAny T>
    [[nodiscard]]
    const T& get() const&;

    template<Concepts::SupportedByAny T>
    [[nodiscard]]
    T&& get() &&;

    template<Concepts::SupportedByAny T>
    [[nodiscard]]
    Maybe<T&> tryGet();

    template<Concepts::SupportedByAny T>
    [[nodiscard]]
    Maybe<const T&> tryGet() const;

    template<Concepts::SupportedByAny T>
    [[nodiscard]]
    T getOr(const T& fallback_value) const;

    explicit operator bool() const
    {
        return _type != AnyType::None;
    }

  private:
    using LargestType = Matrix;

    template<typename Ptr>
    void constructFromBuffer(Ptr ptr);

    void destroy();

    AnyType _type;
    alignas(alignof(LargestType)) u8 _buffer[sizeof(LargestType)];
};

namespace Details
{
template<class T>
inline constexpr bool false_constant = false;

template<typename T>
struct AnyTypeMapper
{
    static_assert(false_constant<T>, "Attempting to use the Any type with an unsupported type.");
};

template<>
struct AnyTypeMapper<char>
{
    static constexpr auto type = AnyType::Char;
};

template<>
struct AnyTypeMapper<unsigned char>
{
    static constexpr auto type = AnyType::UChar;
};

template<>
struct AnyTypeMapper<short>
{
    static constexpr auto type = AnyType::Short;
};

template<>
struct AnyTypeMapper<unsigned short>
{
    static constexpr auto type = AnyType::UShort;
};

template<>
struct AnyTypeMapper<int>
{
    static constexpr auto type = AnyType::Int;
};

template<>
struct AnyTypeMapper<unsigned int>
{
    static constexpr auto type = AnyType::UInt;
};

template<>
struct AnyTypeMapper<float>
{
    static constexpr auto type = AnyType::Float;
};

template<>
struct AnyTypeMapper<double>
{
    static constexpr auto type = AnyType::Double;
};

template<>
struct AnyTypeMapper<bool>
{
    static constexpr auto type = AnyType::Bool;
};

template<>
struct AnyTypeMapper<Vec2>
{
    static constexpr auto type = AnyType::Vec2;
};

template<>
struct AnyTypeMapper<Vec3>
{
    static constexpr auto type = AnyType::Vec3;
};

template<>
struct AnyTypeMapper<Vec4>
{
    static constexpr auto type = AnyType::Vec4;
};

template<>
struct AnyTypeMapper<Color>
{
    static constexpr auto type = AnyType::Color;
};

template<>
struct AnyTypeMapper<Matrix>
{
    static constexpr auto type = AnyType::Matrix;
};

template<>
struct AnyTypeMapper<String>
{
    static constexpr auto type = AnyType::String;
};

template<>
struct AnyTypeMapper<StringView>
{
    static constexpr auto type = AnyType::StringView;
};

template<>
struct AnyTypeMapper<void*>
{
    static constexpr auto type = AnyType::VoidPointer;
};

struct AnyTypeInfo
{
    AnyTypeInfo(bool is_trivial, size_t size, StringView name)
        : is_trivial(is_trivial)
        , size(size)
        , name(name)
    {
    }

    bool       is_trivial;
    size_t     size;
    StringView name;
};

static Maybe<AnyTypeInfo> any_type_info(AnyType type)
{
    switch (type)
    {
        case AnyType::None: return AnyTypeInfo(false, 0, "None"_sv);
        case AnyType::Int: return AnyTypeInfo(true, sizeof(int), "Int"_sv);
        case AnyType::UInt: return AnyTypeInfo(true, sizeof(unsigned int), "UInt"_sv);
        case AnyType::Float: return AnyTypeInfo(true, sizeof(float), "Float"_sv);
        case AnyType::Double: return AnyTypeInfo(true, sizeof(double), "Double"_sv);
        case AnyType::Bool: return AnyTypeInfo(true, sizeof(Vec2), "Bool"_sv);
        case AnyType::Vec2: return AnyTypeInfo(true, sizeof(Vec3), "Vec2"_sv);
        case AnyType::Vec3: return AnyTypeInfo(true, sizeof(Vec4), "Vec3"_sv);
        case AnyType::Vec4: return AnyTypeInfo(true, sizeof(Vec4), "Vec4"_sv);
        case AnyType::Color: return AnyTypeInfo(true, sizeof(Color), "Color"_sv);
        case AnyType::Matrix: return AnyTypeInfo(true, sizeof(Matrix), "Matrix"_sv);
        case AnyType::String: return AnyTypeInfo(false, sizeof(String), "String"_sv);
        case AnyType::StringView: return AnyTypeInfo(false, sizeof(StringView), "StringView"_sv);
        case AnyType::VoidPointer: return AnyTypeInfo(true, sizeof(void*), "VoidPointer"_sv);
        case AnyType::Char: return AnyTypeInfo(true, sizeof(char), "Char"_sv);
        case AnyType::UChar: return AnyTypeInfo(true, sizeof(unsigned char), "UChar"_sv);
        case AnyType::Short: return AnyTypeInfo(true, sizeof(short), "Short"_sv);
        case AnyType::UShort: return AnyTypeInfo(true, sizeof(unsigned short), "UShort"_sv);
    }

    return none;
}

inline void raiseEmptyAnyError()
{
    throw Error("Attempting to access an empty Any object.");
}

inline void raiseInvalidAnyConversionError(AnyType from, AnyType to)
{
    auto msg = "Attempting to obtain the value of an Any object of type '"_s;
    msg += any_type_info(from)->name;
    msg += "' as a '";
    msg += any_type_info(to)->name;
    msg += "'.";

    throw Error(std::move(msg));
}
} // namespace Details

template<Concepts::AssignableToAny T>
Any::Any(T&& value) // NOLINT(*-pro-type-member-init)
    : _type(Details::AnyTypeMapper<std::remove_cvref_t<T>>::type)
{
    std::construct_at(reinterpret_cast<std::remove_cvref_t<T>*>(_buffer), std::forward<T>(value));
}

inline Any::Any(const Any& copyFrom) // NOLINT(*-pro-type-member-init)
    : _type(copyFrom._type)
{
    constructFromBuffer(copyFrom._buffer);
}

inline Any& Any::operator=(const Any& copyFrom)
{
    if (&copyFrom != this)
    {
        destroy();
        _type = copyFrom._type;
        constructFromBuffer(copyFrom._buffer);
    }

    return *this;
}

inline Any::Any(Any&& moveFrom) noexcept // NOLINT(*-pro-type-member-init)
    : _type(moveFrom._type)
{
    constructFromBuffer(moveFrom._buffer);
}

inline Any& Any::operator=(Any&& moveFrom) noexcept
{
    if (&moveFrom != this)
    {
        destroy();
        _type = moveFrom._type;
        constructFromBuffer(moveFrom._buffer);
    }

    return *this;
}

inline Any::~Any() noexcept
{
    destroy();
}

inline AnyType Any::type() const
{
    return _type;
}

template<Concepts::SupportedByAny T>
T& Any::get() &
{
    [[maybe_unused]]
    constexpr auto result_type = Details::AnyTypeMapper<T>::type;

#ifndef polly_no_hardening
    if (_type == AnyType::None)
    {
        Details::raiseEmptyAnyError();
    }

    if (_type != result_type)
    {
        Details::raiseInvalidAnyConversionError(_type, result_type);
    }
#endif

    return *reinterpret_cast<T*>(_buffer);
}

template<Concepts::SupportedByAny T>
const T& Any::get() const&
{
    [[maybe_unused]]
    constexpr auto result_type = Details::AnyTypeMapper<T>::type;

#ifndef polly_no_hardening
    if (_type == AnyType::None)
    {
        Details::raiseEmptyAnyError();
    }

    if (_type != result_type)
    {
        Details::raiseInvalidAnyConversionError(_type, result_type);
    }
#endif

    return *reinterpret_cast<const T*>(_buffer);
}

template<Concepts::SupportedByAny T>
T&& Any::get() &&
{
    [[maybe_unused]]
    constexpr auto result_type = Details::AnyTypeMapper<T>::type;

#ifndef polly_no_hardening
    if (_type == AnyType::None)
    {
        Details::raiseEmptyAnyError();
    }

    if (_type != result_type)
    {
        Details::raiseInvalidAnyConversionError(_type, result_type);
    }
#endif

    return std::move(*reinterpret_cast<T*>(_buffer));
}

template<Concepts::SupportedByAny T>
Maybe<T&> Any::tryGet()
{
#ifndef polly_no_hardening
    if (_type == AnyType::None)
    {
        Details::raiseEmptyAnyError();
    }
#endif

    constexpr auto result_type = Details::AnyTypeMapper<T>::type;

    return _type == result_type ? *reinterpret_cast<T*>(_buffer) : Maybe<T&>();
}

template<Concepts::SupportedByAny T>
Maybe<const T&> Any::tryGet() const
{
#ifndef polly_no_hardening
    if (_type == AnyType::None)
    {
        Details::raiseEmptyAnyError();
    }
#endif

    constexpr auto result_type = Details::AnyTypeMapper<T>::type;

    return _type == result_type ? *reinterpret_cast<const T*>(_buffer) : Maybe<const T&>();
}

template<Concepts::SupportedByAny T>
T Any::getOr(const T& fallback_value) const
{
    if (_type == AnyType::None)
    {
        return fallback_value;
    }

    constexpr auto result_type = Details::AnyTypeMapper<T>::type;

    return _type == result_type ? *reinterpret_cast<const T*>(_buffer) : fallback_value;
}

template<typename Ptr>
void Any::constructFromBuffer(Ptr ptr)
{
    const auto construct_non_trivial_type = [this, ptr]<typename T>()
    {
        auto* dst = reinterpret_cast<T*>(_buffer);
        if constexpr (std::is_const_v<std::remove_pointer_t<Ptr>>)
        {
            std::construct_at(dst, *reinterpret_cast<const T*>(ptr));
        }
        else
        {
            std::construct_at(dst, std::move(*reinterpret_cast<T*>(ptr)));
        }
    };

    if (const auto info = *Details::any_type_info(_type); info.is_trivial)
    {
        // Can copy bitwise.
        std::memcpy(_buffer, ptr, info.size);
    }
    // Must construct type based on tag:
    else if (_type == AnyType::String)
    {
        construct_non_trivial_type.template operator()<String>();
    }
    else if (_type == AnyType::StringView)
    {
        construct_non_trivial_type.template operator()<StringView>();
    }
}

inline void Any::destroy()
{
    const auto destroy_non_trivial_type = [this]<typename T, bool Clear = true>()
    {
        std::destroy_at(reinterpret_cast<T*>(_buffer));

        if constexpr (Clear)
        {
            std::memset(_buffer, 0, sizeof(_buffer));
        }
    };

    if (_type == AnyType::String)
    {
        destroy_non_trivial_type.operator()<String>();
    }
    else if (_type == AnyType::StringView)
    {
        destroy_non_trivial_type.operator()<StringView>();
    }
}
} // namespace Polly
