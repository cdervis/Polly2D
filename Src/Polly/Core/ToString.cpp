// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Any.hpp"

#include "Polly/ToString.hpp"

#include "Polly/Color.hpp"
#include "Polly/Format.hpp"
#include "Polly/GamePerformanceStats.hpp"
#include "Polly/Image.hpp"
#include "Polly/Input/InputImpl.hpp"
#include "Polly/Rectangle.hpp"
#include "Polly/String.hpp"

namespace Polly
{
auto toString(const Any& value) -> String
{
    if (not value)
    {
        return "<empty>";
    }

    switch (value.type())
    {
        case AnyType::None: return "<empty>";
        case AnyType::Int: return toString(value.get<int>());
        case AnyType::UInt: return toString(value.get<unsigned int>());
        case AnyType::Float: return toString(value.get<float>());
        case AnyType::Double: return toString(value.get<double>());
        case AnyType::Bool: return toString(value.get<bool>());
        case AnyType::Vec2: return toString(value.get<Vec2>());
        case AnyType::Vec3: return toString(value.get<Vec3>());
        case AnyType::Vec4: return toString(value.get<Vec4>());
        case AnyType::Color: return toString(value.get<Color>());
        case AnyType::Matrix: return toString(value.get<Matrix>());
        case AnyType::String: return value.get<String>();
        case AnyType::StringView: return String(value.get<StringView>());
        case AnyType::VoidPointer: return toString(reinterpret_cast<uintptr_t>(value.get<void*>()));
        case AnyType::Char: return toString(value.get<char>());
        case AnyType::UChar: return toString(value.get<unsigned char>());
        case AnyType::Short: return toString(value.get<short>());
        case AnyType::UShort: return toString(value.get<unsigned short>());
    }

    return "<invalid>";
}

String toString(const String& value)
{
    return value;
}

String toString(const StringView& value)
{
    return String(value);
}

String toString(const char* value)
{
    return String(value);
}

String toString(const Vec2& value)
{
    return formatString("[x={}; y={}]", value.x, value.y);
}

String toString(const Vec3& value)
{
    return formatString("[x={}; y={}; z={}]", value.x, value.y, value.z);
}

String toString(const Vec4& value)
{
    return formatString("[x={}; y={}; z={}; w={}]", value.x, value.y, value.z, value.w);
}

String toString(const Color& value)
{
    return formatString("[r={}; g={}; b={}; a={}]", value.r, value.g, value.b, value.a);
}

String toString(const Matrix& value)
{
    return formatString(
        R"([
  {}; {}; {}; {}
  {}; {}; {}; {}
  {}; {}; {}; {}
  {}; {}; {}; {}
])",
        value.row1.x,
        value.row1.y,
        value.row1.z,
        value.row1.w,
        value.row2.x,
        value.row2.y,
        value.row2.z,
        value.row2.w,
        value.row3.x,
        value.row3.y,
        value.row3.z,
        value.row3.w,
        value.row4.x,
        value.row4.y,
        value.row4.z,
        value.row4.w);
}

String toString(Degrees value)
{
    return formatString("{} deg", value.value);
}

String toString(Radians value)
{
    return formatString("{} rad", value.value);
}

String toString(const Rectangle& value)
{
    return formatString("[x={}; y={}; width={}; height={}]", value.x, value.y, value.width, value.height);
}

String toString(MouseButton value)
{
    switch (value)
    {
        case MouseButton::Left: return "Left";
        case MouseButton::Right: return "Right";
        case MouseButton::Middle: return "Middle";
        case MouseButton::Extra1: return "Extra1";
        case MouseButton::Extra2: return "Extra2";
    }

    return "Unknown";
}

String toString(Key value)
{
    return SDL_GetKeyName(InputImpl::toSDLKey(value));
}

String toString(KeyModifier value)
{
    switch (value)
    {
        case KeyModifier::None: return "None";
        case KeyModifier::LeftShift: return "LeftShift";
        case KeyModifier::RightShift: return "RightShift";
        case KeyModifier::Level5: return "Level5";
        case KeyModifier::LeftControl: return "LeftControl";
        case KeyModifier::RightControl: return "RightControl";
        case KeyModifier::LeftAlt: return "LeftAlt";
        case KeyModifier::RightAlt: return "RightAlt";
        case KeyModifier::LeftGui: return "LeftGui";
        case KeyModifier::RightGui: return "RightGui";
        case KeyModifier::Num: return "Num";
        case KeyModifier::Caps: return "Caps";
        case KeyModifier::Mode: return "Mode";
        case KeyModifier::Scroll: return "Scroll";
        case KeyModifier::AnyControlKey: return "AnyControlKey";
        case KeyModifier::AnyShiftKey: return "AnyShiftKey";
        case KeyModifier::AnyAltKey: return "AnyAltKey";
        case KeyModifier::AnyGuiKey: return "AnyGuiKey";
    }

    return "None";
}

String toString(Scancode value)
{
    switch (value)
    {
        case Scancode::Unknown: return "Unknown";
        case Scancode::A: return "A";
        case Scancode::B: return "B";
        case Scancode::C: return "C";
        case Scancode::D: return "D";
        case Scancode::E: return "E";
        case Scancode::F: return "F";
        case Scancode::G: return "G";
        case Scancode::H: return "H";
        case Scancode::I: return "I";
        case Scancode::J: return "J";
        case Scancode::K: return "K";
        case Scancode::L: return "L";
        case Scancode::M: return "M";
        case Scancode::N: return "N";
        case Scancode::O: return "O";
        case Scancode::P: return "P";
        case Scancode::Q: return "Q";
        case Scancode::R: return "R";
        case Scancode::S: return "S";
        case Scancode::T: return "T";
        case Scancode::U: return "U";
        case Scancode::V: return "V";
        case Scancode::W: return "W";
        case Scancode::X: return "X";
        case Scancode::Y: return "Y";
        case Scancode::Z: return "Z";
        case Scancode::D1: return "D1";
        case Scancode::D2: return "D2";
        case Scancode::D3: return "D3";
        case Scancode::D4: return "D4";
        case Scancode::D5: return "D5";
        case Scancode::D6: return "D6";
        case Scancode::D7: return "D7";
        case Scancode::D8: return "D8";
        case Scancode::D9: return "D9";
        case Scancode::D0: return "D0";
        case Scancode::Return: return "Return";
        case Scancode::Escape: return "Escape";
        case Scancode::Backspace: return "Backspace";
        case Scancode::Tab: return "Tab";
        case Scancode::Space: return "Space";
        case Scancode::Minus: return "Minus";
        case Scancode::Equals: return "Equals";
        case Scancode::LeftBracket: return "LeftBracket";
        case Scancode::RightBracket: return "RightBracket";
        case Scancode::Backslash: return "Backslash";
        case Scancode::NonUSHash: return "NonUSHash";
        case Scancode::Semicolon: return "Semicolon";
        case Scancode::Apostrophe: return "Apostrophe";
        case Scancode::Grave: return "Grave";
        case Scancode::Comma: return "Comma";
        case Scancode::Period: return "Period";
        case Scancode::Slash: return "Slash";
        case Scancode::CapsLock: return "CapsLock";
        case Scancode::F1: return "F1";
        case Scancode::F2: return "F2";
        case Scancode::F3: return "F3";
        case Scancode::F4: return "F4";
        case Scancode::F5: return "F5";
        case Scancode::F6: return "F6";
        case Scancode::F7: return "F7";
        case Scancode::F8: return "F8";
        case Scancode::F9: return "F9";
        case Scancode::F10: return "F10";
        case Scancode::F11: return "F11";
        case Scancode::F12: return "F12";
        case Scancode::PrintScreen: return "PrintScreen";
        case Scancode::ScrollLock: return "ScrollLock";
        case Scancode::Pause: return "Pause";
        case Scancode::Insert: return "Insert";
        case Scancode::Home: return "Home";
        case Scancode::PageUp: return "PageUp";
        case Scancode::Delete: return "Delete";
        case Scancode::End: return "End";
        case Scancode::PageDown: return "PageDown";
        case Scancode::Right: return "Right";
        case Scancode::Left: return "Left";
        case Scancode::Down: return "Down";
        case Scancode::Up: return "Up";
        case Scancode::NumLockClear: return "NumLockClear";
        case Scancode::KeypadDivide: return "KeypadDivide";
        case Scancode::KeypadMultiply: return "KeypadMultiply";
        case Scancode::KeypadMinus: return "KeypadMinus";
        case Scancode::KeypadPlus: return "KeypadPlus";
        case Scancode::KeypadEnter: return "KeypadEnter";
        case Scancode::Keypad1: return "Keypad1";
        case Scancode::Keypad2: return "Keypad2";
        case Scancode::Keypad3: return "Keypad3";
        case Scancode::Keypad4: return "Keypad4";
        case Scancode::Keypad5: return "Keypad5";
        case Scancode::Keypad6: return "Keypad6";
        case Scancode::Keypad7: return "Keypad7";
        case Scancode::Keypad8: return "Keypad8";
        case Scancode::Keypad9: return "Keypad9";
        case Scancode::Keypad0: return "Keypad0";
        case Scancode::KeypadPeriod: return "KeypadPeriod";
        case Scancode::NonUSBackslash: return "NonUSBackslash";
        case Scancode::Application: return "Application";
        case Scancode::Power: return "Power";
        case Scancode::KeypadEquals: return "KeypadEquals";
        case Scancode::F13: return "F13";
        case Scancode::F14: return "F14";
        case Scancode::F15: return "F15";
        case Scancode::F16: return "F16";
        case Scancode::F17: return "F17";
        case Scancode::F18: return "F18";
        case Scancode::F19: return "F19";
        case Scancode::F20: return "F20";
        case Scancode::F21: return "F21";
        case Scancode::F22: return "F22";
        case Scancode::F23: return "F23";
        case Scancode::F24: return "F24";
        case Scancode::Execute: return "Execute";
        case Scancode::Help: return "Help";
        case Scancode::Menu: return "Menu";
        case Scancode::Select: return "Select";
        case Scancode::Stop: return "Stop";
        case Scancode::Again: return "Again";
        case Scancode::Undo: return "Undo";
        case Scancode::Cut: return "Cut";
        case Scancode::Copy: return "Copy";
        case Scancode::Paste: return "Paste";
        case Scancode::Find: return "Find";
        case Scancode::Mute: return "Mute";
        case Scancode::VolumeUp: return "VolumeUp";
        case Scancode::VolumeDown: return "VolumeDown";
        case Scancode::KeypadComma: return "KeypadComma";
        case Scancode::KeypadEqualsAs400: return "KeypadEqualsAs400";
        case Scancode::International1: return "International1";
        case Scancode::International2: return "International2";
        case Scancode::International3: return "International3";
        case Scancode::International4: return "International4";
        case Scancode::International5: return "International5";
        case Scancode::International6: return "International6";
        case Scancode::International7: return "International7";
        case Scancode::International8: return "International8";
        case Scancode::International9: return "International9";
        case Scancode::Lang1: return "Lang1";
        case Scancode::Lang2: return "Lang2";
        case Scancode::Lang3: return "Lang3";
        case Scancode::Lang4: return "Lang4";
        case Scancode::Lang5: return "Lang5";
        case Scancode::Lang6: return "Lang6";
        case Scancode::Lang7: return "Lang7";
        case Scancode::Lang8: return "Lang8";
        case Scancode::Lang9: return "Lang9";
        case Scancode::AltErase: return "AltErase";
        case Scancode::SysReq: return "SysReq";
        case Scancode::Cancel: return "Cancel";
        case Scancode::Clear: return "Clear";
        case Scancode::Prior: return "Prior";
        case Scancode::Return2: return "Return2";
        case Scancode::Separator: return "Separator";
        case Scancode::Out: return "Out";
        case Scancode::Oper: return "Oper";
        case Scancode::ClearAgain: return "ClearAgain";
        case Scancode::CrSel: return "CrSel";
        case Scancode::ExSel: return "ExSel";
        case Scancode::Keypad00: return "Keypad00";
        case Scancode::Keypad000: return "Keypad000";
        case Scancode::ThousandsSeparator: return "ThousandsSeparator";
        case Scancode::DecimalSeparator: return "DecimalSeparator";
        case Scancode::CurrencyUnit: return "CurrencyUnit";
        case Scancode::CurrencySubUnit: return "CurrencySubUnit";
        case Scancode::KeypadLeftParent: return "KeypadLeftParent";
        case Scancode::KeypadRightParen: return "KeypadRightParen";
        case Scancode::KeypadLeftBrace: return "KeypadLeftBrace";
        case Scancode::KeypadRightBrace: return "KeypadRightBrace";
        case Scancode::KeypadTab: return "KeypadTab";
        case Scancode::KeypadBackspace: return "KeypadBackspace";
        case Scancode::KeypadA: return "KeypadA";
        case Scancode::KeypadB: return "KeypadB";
        case Scancode::KeypadC: return "KeypadC";
        case Scancode::KeypadD: return "KeypadD";
        case Scancode::KeypadE: return "KeypadE";
        case Scancode::KeypadF: return "KeypadF";
        case Scancode::KeypadXor: return "KeypadXor";
        case Scancode::KeypadPower: return "KeypadPower";
        case Scancode::KeypadPercent: return "KeypadPercent";
        case Scancode::KeypadLess: return "KeypadLess";
        case Scancode::KeypadGreater: return "KeypadGreater";
        case Scancode::KeypadAmpersand: return "KeypadAmpersand";
        case Scancode::KeypadDoubleAmpersand: return "KeypadDoubleAmpersand";
        case Scancode::KeypadVerticalBar: return "KeypadVerticalBar";
        case Scancode::KeypadDoubleVerticalBar: return "KeypadDoubleVerticalBar";
        case Scancode::KeypadColon: return "KeypadColon";
        case Scancode::KeypadHash: return "KeypadHash";
        case Scancode::KeypadSpace: return "KeypadSpace";
        case Scancode::KeypadAt: return "KeypadAt";
        case Scancode::KeypadExclam: return "KeypadExclam";
        case Scancode::KeypadMemStore: return "KeypadMemStore";
        case Scancode::KeypadMemRecall: return "KeypadMemRecall";
        case Scancode::KeypadMemClear: return "KeypadMemClear";
        case Scancode::KeypadMemAdd: return "KeypadMemAdd";
        case Scancode::KeypadMemSubtract: return "KeypadMemSubtract";
        case Scancode::KeypadMemMultiply: return "KeypadMemMultiply";
        case Scancode::KeypadMemDivide: return "KeypadMemDivide";
        case Scancode::KeypadPlusMinus: return "KeypadPlusMinus";
        case Scancode::KeypadClear: return "KeypadClear";
        case Scancode::KeypadClearEntry: return "KeypadClearEntry";
        case Scancode::KeypadBinary: return "KeypadBinary";
        case Scancode::KeypadOctal: return "KeypadOctal";
        case Scancode::KeypadDecimal: return "KeypadDecimal";
        case Scancode::KeypadHexadecimal: return "KeypadHexadecimal";
        case Scancode::LeftControl: return "LeftControl";
        case Scancode::LeftShift: return "LeftShift";
        case Scancode::LeftAlt: return "LeftAlt";
        case Scancode::LeftGui: return "LeftGui";
        case Scancode::RightControl: return "RightControl";
        case Scancode::RightShift: return "RightShift";
        case Scancode::RightAlt: return "RightAlt";
        case Scancode::RightGui: return "RightGui";
        case Scancode::Mode: return "Mode";
        case Scancode::Sleep: return "Sleep";
        case Scancode::Wake: return "Wake";
        case Scancode::ChannelIncrement: return "ChannelIncrement";
        case Scancode::ChannelDecrement: return "ChannelDecrement";
        case Scancode::MediaPlay: return "MediaPlay";
        case Scancode::MediaPause: return "MediaPause";
        case Scancode::MediaRecord: return "MediaRecord";
        case Scancode::MediaFastForward: return "MediaFastForward";
        case Scancode::MediaRewind: return "MediaRewind";
        case Scancode::MediaNextTrack: return "MediaNextTrack";
        case Scancode::MediaPreviousTrack: return "MediaPreviousTrack";
        case Scancode::MediaStop: return "MediaStop";
        case Scancode::MediaEject: return "MediaEject";
        case Scancode::MediaPlayPause: return "MediaPlayPause";
        case Scancode::MediaSelect: return "MediaSelect";
        case Scancode::AcNew: return "AcNew";
        case Scancode::AcOpen: return "AcOpen";
        case Scancode::AcClose: return "AcClose";
        case Scancode::AcExit: return "AcExit";
        case Scancode::AcSave: return "AcSave";
        case Scancode::AcPrint: return "AcPrint";
        case Scancode::AcProperties: return "AcProperties";
        case Scancode::AcSearch: return "AcSearch";
        case Scancode::AcHome: return "AcHome";
        case Scancode::AcBack: return "AcBack";
        case Scancode::AcForward: return "AcForward";
        case Scancode::AcStop: return "AcStop";
        case Scancode::AcRefresh: return "AcRefresh";
        case Scancode::AcBookmarks: return "AcBookmarks";
        case Scancode::SoftLeft: return "SoftLeft";
        case Scancode::SoftRight: return "SoftRight";
        case Scancode::Call: return "Call";
        case Scancode::EndCall: return "EndCall";
    }

    return "Unknown";
}

String toString(ImageFormat value)
{
    switch (value)
    {
        case ImageFormat::R8Unorm: return "R8_UNorm";
        case ImageFormat::R8G8B8A8UNorm: return "R8G8B8A8_UNorm";
        case ImageFormat::R8G8B8A8Srgb: return "R8G8B8A8_Srgb";
        case ImageFormat::R32G32B32A32Float: return "R32G32B32A32_Float";
    }

    return "Unknown";
}

String toString(const Image& value)
{
    if (value)
    {
        const auto name = value.debuggingLabel();

        return formatString(
            "[name='{}'; size={}x{}; format={}]",
            name.isEmpty() ? "<unnamed>" : name,
            value.width(),
            value.height(),
            value.format());
    }

    return "<none>";
}

String toString(const GamePerformanceStats& value)
{
    return formatString(
        R"(FPS: {}
Sprites (incl. text): {}
Polygons: {}
Meshes: {}
Draw Calls: {}
Texture Changes: {}
Vertices: {})",
        value.framesPerSecond,
        value.spriteCount,
        value.polygonCount,
        value.meshCount,
        value.drawCallCount,
        value.textureChangeCount,
        value.vertexCount);
}

String toString(const void* value)
{
    constexpr auto digits = StringView("0123456789ABCDEF");

    auto buffer = String();

    auto num = reinterpret_cast<u64>(value);
    do
    {
        constexpr size_t base = 16;
        buffer += digits[num % base];
        num = num / base;
    }
    while (num > 0);

    std::ranges::reverse(buffer);

    return formatString("0x{}", buffer);
}
} // namespace Polly