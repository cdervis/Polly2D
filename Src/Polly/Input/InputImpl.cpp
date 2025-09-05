// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "InputImpl.hpp"

#include "Polly/Assume.hpp"
#include "Polly/KeyModifier.hpp"
#include "Polly/MouseButton.hpp"
#include "Polly/Span.hpp"

namespace Polly
{
static InputImpl* sInputImplInstance;

int InputImpl::toSDLScancode(Scancode scancode)
{
    return static_cast<int>(scancode);
}

int InputImpl::toSDLKey(const Key key)
{
    switch (key)
    {
        case Key::Unknown: return SDLK_UNKNOWN;
        case Key::Return: return SDLK_RETURN;
        case Key::Escape: return SDLK_ESCAPE;
        case Key::Backspace: return SDLK_BACKSPACE;
        case Key::Tab: return SDLK_TAB;
        case Key::Space: return SDLK_SPACE;
        case Key::Exclaim: return SDLK_EXCLAIM;
        case Key::DoubleApostrophe: return SDLK_DBLAPOSTROPHE;
        case Key::Hash: return SDLK_HASH;
        case Key::Dollar: return SDLK_DOLLAR;
        case Key::Percent: return SDLK_PERCENT;
        case Key::Ampersand: return SDLK_AMPERSAND;
        case Key::Apostrophe: return SDLK_APOSTROPHE;
        case Key::LeftParen: return SDLK_LEFTPAREN;
        case Key::RightParen: return SDLK_RIGHTPAREN;
        case Key::Asterisk: return SDLK_ASTERISK;
        case Key::Plus: return SDLK_PLUS;
        case Key::Comma: return SDLK_COMMA;
        case Key::Minus: return SDLK_MINUS;
        case Key::Period: return SDLK_PERIOD;
        case Key::Slash: return SDLK_SLASH;
        case Key::D0: return SDLK_0;
        case Key::D1: return SDLK_1;
        case Key::D2: return SDLK_2;
        case Key::D3: return SDLK_3;
        case Key::D4: return SDLK_4;
        case Key::D5: return SDLK_5;
        case Key::D6: return SDLK_6;
        case Key::D7: return SDLK_7;
        case Key::D8: return SDLK_8;
        case Key::D9: return SDLK_9;
        case Key::Colon: return SDLK_COLON;
        case Key::Semicolon: return SDLK_SEMICOLON;
        case Key::Less: return SDLK_LESS;
        case Key::Equals: return SDLK_EQUALS;
        case Key::Greater: return SDLK_GREATER;
        case Key::Question: return SDLK_QUESTION;
        case Key::At: return SDLK_AT;
        case Key::LeftBracket: return SDLK_LEFTBRACKET;
        case Key::Backslash: return SDLK_BACKSLASH;
        case Key::RightBracket: return SDLK_RIGHTBRACKET;
        case Key::Caret: return SDLK_CARET;
        case Key::Underscore: return SDLK_UNDERSCORE;
        case Key::Grave: return SDLK_GRAVE;
        case Key::A: return SDLK_A;
        case Key::B: return SDLK_B;
        case Key::C: return SDLK_C;
        case Key::D: return SDLK_D;
        case Key::E: return SDLK_E;
        case Key::F: return SDLK_F;
        case Key::G: return SDLK_G;
        case Key::H: return SDLK_H;
        case Key::I: return SDLK_I;
        case Key::J: return SDLK_J;
        case Key::K: return SDLK_K;
        case Key::L: return SDLK_L;
        case Key::M: return SDLK_M;
        case Key::N: return SDLK_N;
        case Key::O: return SDLK_O;
        case Key::P: return SDLK_P;
        case Key::Q: return SDLK_Q;
        case Key::R: return SDLK_R;
        case Key::S: return SDLK_S;
        case Key::T: return SDLK_T;
        case Key::U: return SDLK_U;
        case Key::V: return SDLK_V;
        case Key::W: return SDLK_W;
        case Key::X: return SDLK_X;
        case Key::Y: return SDLK_Y;
        case Key::Z: return SDLK_Z;
        case Key::LeftBrace: return SDLK_LEFTBRACE;
        case Key::Pipe: return SDLK_PIPE;
        case Key::RightBrace: return SDLK_RIGHTBRACE;
        case Key::Tilde: return SDLK_TILDE;
        case Key::Delete: return SDLK_DELETE;
        case Key::PlusMinus: return SDLK_PLUSMINUS;
        case Key::CapsLock: return SDLK_CAPSLOCK;
        case Key::F1: return SDLK_F1;
        case Key::F2: return SDLK_F2;
        case Key::F3: return SDLK_F3;
        case Key::F4: return SDLK_F4;
        case Key::F5: return SDLK_F5;
        case Key::F6: return SDLK_F6;
        case Key::F7: return SDLK_F7;
        case Key::F8: return SDLK_F8;
        case Key::F9: return SDLK_F9;
        case Key::F10: return SDLK_F10;
        case Key::F11: return SDLK_F11;
        case Key::F12: return SDLK_F12;
        case Key::PrintScreen: return SDLK_PRINTSCREEN;
        case Key::ScrollLock: return SDLK_SCROLLLOCK;
        case Key::Pause: return SDLK_PAUSE;
        case Key::Insert: return SDLK_INSERT;
        case Key::Home: return SDLK_HOME;
        case Key::PageUp: return SDLK_PAGEUP;
        case Key::End: return SDLK_END;
        case Key::PageDown: return SDLK_PAGEDOWN;
        case Key::Right: return SDLK_RIGHT;
        case Key::Left: return SDLK_LEFT;
        case Key::Down: return SDLK_DOWN;
        case Key::Up: return SDLK_UP;
        case Key::NumLockClear: return SDLK_NUMLOCKCLEAR;
        case Key::KeypadDivide: return SDLK_KP_DIVIDE;
        case Key::KeypadMultiply: return SDLK_KP_MULTIPLY;
        case Key::KeypadMinus: return SDLK_KP_MINUS;
        case Key::KeypadPlus: return SDLK_KP_PLUS;
        case Key::KeypadEnter: return SDLK_KP_ENTER;
        case Key::Keypad1: return SDLK_KP_1;
        case Key::Keypad2: return SDLK_KP_2;
        case Key::Keypad3: return SDLK_KP_3;
        case Key::Keypad4: return SDLK_KP_4;
        case Key::Keypad5: return SDLK_KP_5;
        case Key::Keypad6: return SDLK_KP_6;
        case Key::Keypad7: return SDLK_KP_7;
        case Key::Keypad8: return SDLK_KP_8;
        case Key::Keypad9: return SDLK_KP_9;
        case Key::Keypad0: return SDLK_KP_0;
        case Key::KeypadPeriod: return SDLK_KP_PERIOD;
        case Key::Application: return SDLK_APPLICATION;
        case Key::Power: return SDLK_POWER;
        case Key::KeypadEquals: return SDLK_KP_EQUALS;
        case Key::F13: return SDLK_F13;
        case Key::F14: return SDLK_F14;
        case Key::F15: return SDLK_F15;
        case Key::F16: return SDLK_F16;
        case Key::F17: return SDLK_F17;
        case Key::F18: return SDLK_F18;
        case Key::F19: return SDLK_F19;
        case Key::F20: return SDLK_F20;
        case Key::F21: return SDLK_F21;
        case Key::F22: return SDLK_F22;
        case Key::F23: return SDLK_F23;
        case Key::F24: return SDLK_F24;
        case Key::Execute: return SDLK_EXECUTE;
        case Key::Help: return SDLK_HELP;
        case Key::Menu: return SDLK_MENU;
        case Key::Select: return SDLK_SELECT;
        case Key::Stop: return SDLK_STOP;
        case Key::Again: return SDLK_AGAIN;
        case Key::Undo: return SDLK_UNDO;
        case Key::Cut: return SDLK_CUT;
        case Key::Copy: return SDLK_COPY;
        case Key::Paste: return SDLK_PASTE;
        case Key::Find: return SDLK_FIND;
        case Key::Mute: return SDLK_MUTE;
        case Key::VolumeUp: return SDLK_VOLUMEUP;
        case Key::VolumeDown: return SDLK_VOLUMEDOWN;
        case Key::KeypadComma: return SDLK_KP_COMMA;
        case Key::KeypadEqualsAs400: return SDLK_KP_EQUALSAS400;
        case Key::AltErase: return SDLK_ALTERASE;
        case Key::SysReq: return SDLK_SYSREQ;
        case Key::Cancel: return SDLK_CANCEL;
        case Key::Clear: return SDLK_CLEAR;
        case Key::Prior: return SDLK_PRIOR;
        case Key::Return2: return SDLK_RETURN2;
        case Key::Separator: return SDLK_SEPARATOR;
        case Key::Out: return SDLK_OUT;
        case Key::Oper: return SDLK_OPER;
        case Key::ClearAgain: return SDLK_CLEARAGAIN;
        case Key::CrSel: return SDLK_CRSEL;
        case Key::ExSel: return SDLK_EXSEL;
        case Key::Keypad00: return SDLK_KP_00;
        case Key::Keypad000: return SDLK_KP_000;
        case Key::ThousandsSeparator: return SDLK_THOUSANDSSEPARATOR;
        case Key::DecimalSeparator: return SDLK_DECIMALSEPARATOR;
        case Key::CurrencyUnit: return SDLK_CURRENCYUNIT;
        case Key::CurrencySubUnit: return SDLK_CURRENCYSUBUNIT;
        case Key::KeypadLeftParen: return SDLK_KP_LEFTPAREN;
        case Key::KeypadRightParen: return SDLK_KP_RIGHTPAREN;
        case Key::KeypadLeftBrace: return SDLK_KP_LEFTBRACE;
        case Key::KeypadRightBrace: return SDLK_KP_RIGHTBRACE;
        case Key::KeypadTab: return SDLK_KP_TAB;
        case Key::KeypadBackspace: return SDLK_KP_BACKSPACE;
        case Key::KeypadA: return SDLK_KP_A;
        case Key::KeypadB: return SDLK_KP_B;
        case Key::KeypadC: return SDLK_KP_C;
        case Key::KeypadD: return SDLK_KP_D;
        case Key::KeypadE: return SDLK_KP_E;
        case Key::KeypadF: return SDLK_KP_F;
        case Key::KeypadXor: return SDLK_KP_XOR;
        case Key::KeypadPower: return SDLK_KP_POWER;
        case Key::KeypadPercent: return SDLK_KP_PERCENT;
        case Key::KeypadLess: return SDLK_KP_LESS;
        case Key::KeypadGreater: return SDLK_KP_GREATER;
        case Key::KeypadAmpersand: return SDLK_KP_AMPERSAND;
        case Key::KeypadDoubleAmpersand: return SDLK_KP_DBLAMPERSAND;
        case Key::KeypadVerticalBar: return SDLK_KP_VERTICALBAR;
        case Key::KeypadDoubleVerticalBar: return SDLK_KP_DBLVERTICALBAR;
        case Key::KeypadColon: return SDLK_KP_COLON;
        case Key::KeypadHash: return SDLK_KP_HASH;
        case Key::KeypadSpace: return SDLK_KP_SPACE;
        case Key::KeypadAt: return SDLK_KP_AT;
        case Key::KeypadExclam: return SDLK_KP_EXCLAM;
        case Key::KeypadMemStore: return SDLK_KP_MEMSTORE;
        case Key::KeypadMemRecall: return SDLK_KP_MEMRECALL;
        case Key::KeypadMemClear: return SDLK_KP_MEMCLEAR;
        case Key::KeypadMemAdd: return SDLK_KP_MEMADD;
        case Key::KeypadMemSubtract: return SDLK_KP_MEMSUBTRACT;
        case Key::KeypadMemMultiply: return SDLK_KP_MEMMULTIPLY;
        case Key::KeypadMemDivide: return SDLK_KP_MEMDIVIDE;
        case Key::KeypadPlusMinus: return SDLK_KP_PLUSMINUS;
        case Key::KeypadClear: return SDLK_KP_CLEAR;
        case Key::KeypadClearEntry: return SDLK_KP_CLEARENTRY;
        case Key::KeypadBinary: return SDLK_KP_BINARY;
        case Key::KeypadOctal: return SDLK_KP_OCTAL;
        case Key::KeypadDecimal: return SDLK_KP_DECIMAL;
        case Key::KeypadHexadecimal: return SDLK_KP_HEXADECIMAL;
        case Key::LeftControl: return SDLK_LCTRL;
        case Key::LeftShift: return SDLK_LSHIFT;
        case Key::LeftAlt: return SDLK_LALT;
        case Key::LeftGui: return SDLK_LGUI;
        case Key::RightControl: return SDLK_RCTRL;
        case Key::RightShift: return SDLK_RSHIFT;
        case Key::RightAlt: return SDLK_RALT;
        case Key::RightGui: return SDLK_RGUI;
        case Key::Mode: return SDLK_MODE;
        case Key::Sleep: return SDLK_SLEEP;
        case Key::Wake: return SDLK_WAKE;
        case Key::ChannelIncrement: return SDLK_CHANNEL_INCREMENT;
        case Key::ChannelDecrement: return SDLK_CHANNEL_DECREMENT;
        case Key::MediaPlay: return SDLK_MEDIA_PLAY;
        case Key::MediaPause: return SDLK_MEDIA_PAUSE;
        case Key::MediaRecord: return SDLK_MEDIA_RECORD;
        case Key::MediaFastForward: return SDLK_MEDIA_FAST_FORWARD;
        case Key::MediaRewind: return SDLK_MEDIA_REWIND;
        case Key::MediaNextTrack: return SDLK_MEDIA_NEXT_TRACK;
        case Key::MediaPreviousTrack: return SDLK_MEDIA_PREVIOUS_TRACK;
        case Key::MediaStop: return SDLK_MEDIA_STOP;
        case Key::MediaEject: return SDLK_MEDIA_EJECT;
        case Key::MediaPlayPause: return SDLK_MEDIA_PLAY_PAUSE;
        case Key::MediaSelect: return SDLK_MEDIA_SELECT;
        case Key::AcNew: return SDLK_AC_NEW;
        case Key::AcOpen: return SDLK_AC_OPEN;
        case Key::AcClose: return SDLK_AC_CLOSE;
        case Key::AcExit: return SDLK_AC_EXIT;
        case Key::AcSave: return SDLK_AC_SAVE;
        case Key::AcPrint: return SDLK_AC_PRINT;
        case Key::AcProperties: return SDLK_AC_PROPERTIES;
        case Key::AcSearch: return SDLK_AC_SEARCH;
        case Key::AcHome: return SDLK_AC_HOME;
        case Key::AcBack: return SDLK_AC_BACK;
        case Key::AcForward: return SDLK_AC_FORWARD;
        case Key::AcStop: return SDLK_AC_STOP;
        case Key::AcRefresh: return SDLK_AC_REFRESH;
        case Key::AcBookmarks: return SDLK_AC_BOOKMARKS;
        case Key::SoftLeft: return SDLK_SOFTLEFT;
        case Key::SoftRight: return SDLK_SOFTRIGHT;
        case Key::Call: return SDLK_CALL;
        case Key::EndCall: return SDLK_ENDCALL;
        case Key::LeftTab: return SDLK_LEFT_TAB;
        case Key::Level5Shift: return SDLK_LEVEL5_SHIFT;
        case Key::MultiKeyCompose: return SDLK_MULTI_KEY_COMPOSE;
        case Key::LeftMeta: return SDLK_LMETA;
        case Key::RightMeta: return SDLK_RMETA;
        case Key::LeftHyper: return SDLK_LHYPER;
        case Key::RightHyper: return SDLK_RHYPER;
    }

    return SDLK_UNKNOWN;
}

Key InputImpl::fromSDLKey(const SDL_Keycode sdlKey)
{
    switch (sdlKey)
    {
        case SDLK_RETURN: return Key::Return;
        case SDLK_ESCAPE: return Key::Escape;
        case SDLK_BACKSPACE: return Key::Backspace;
        case SDLK_TAB: return Key::Tab;
        case SDLK_SPACE: return Key::Space;
        case SDLK_EXCLAIM: return Key::Exclaim;
        case SDLK_DBLAPOSTROPHE: return Key::DoubleApostrophe;
        case SDLK_HASH: return Key::Hash;
        case SDLK_DOLLAR: return Key::Dollar;
        case SDLK_PERCENT: return Key::Percent;
        case SDLK_AMPERSAND: return Key::Ampersand;
        case SDLK_APOSTROPHE: return Key::Apostrophe;
        case SDLK_LEFTPAREN: return Key::LeftParen;
        case SDLK_RIGHTPAREN: return Key::RightParen;
        case SDLK_ASTERISK: return Key::Asterisk;
        case SDLK_PLUS: return Key::Plus;
        case SDLK_COMMA: return Key::Comma;
        case SDLK_MINUS: return Key::Minus;
        case SDLK_PERIOD: return Key::Period;
        case SDLK_SLASH: return Key::Slash;
        case SDLK_0: return Key::D0;
        case SDLK_1: return Key::D1;
        case SDLK_2: return Key::D2;
        case SDLK_3: return Key::D3;
        case SDLK_4: return Key::D4;
        case SDLK_5: return Key::D5;
        case SDLK_6: return Key::D6;
        case SDLK_7: return Key::D7;
        case SDLK_8: return Key::D8;
        case SDLK_9: return Key::D9;
        case SDLK_COLON: return Key::Colon;
        case SDLK_SEMICOLON: return Key::Semicolon;
        case SDLK_LESS: return Key::Less;
        case SDLK_EQUALS: return Key::Equals;
        case SDLK_GREATER: return Key::Greater;
        case SDLK_QUESTION: return Key::Question;
        case SDLK_AT: return Key::At;
        case SDLK_LEFTBRACKET: return Key::LeftBracket;
        case SDLK_BACKSLASH: return Key::Backslash;
        case SDLK_RIGHTBRACKET: return Key::RightBracket;
        case SDLK_CARET: return Key::Caret;
        case SDLK_UNDERSCORE: return Key::Underscore;
        case SDLK_GRAVE: return Key::Grave;
        case SDLK_A: return Key::A;
        case SDLK_B: return Key::B;
        case SDLK_C: return Key::C;
        case SDLK_D: return Key::D;
        case SDLK_E: return Key::E;
        case SDLK_F: return Key::F;
        case SDLK_G: return Key::G;
        case SDLK_H: return Key::H;
        case SDLK_I: return Key::I;
        case SDLK_J: return Key::J;
        case SDLK_K: return Key::K;
        case SDLK_L: return Key::L;
        case SDLK_M: return Key::M;
        case SDLK_N: return Key::N;
        case SDLK_O: return Key::O;
        case SDLK_P: return Key::P;
        case SDLK_Q: return Key::Q;
        case SDLK_R: return Key::R;
        case SDLK_S: return Key::S;
        case SDLK_T: return Key::T;
        case SDLK_U: return Key::U;
        case SDLK_V: return Key::V;
        case SDLK_W: return Key::W;
        case SDLK_X: return Key::X;
        case SDLK_Y: return Key::Y;
        case SDLK_Z: return Key::Z;
        case SDLK_LEFTBRACE: return Key::LeftBrace;
        case SDLK_PIPE: return Key::Pipe;
        case SDLK_RIGHTBRACE: return Key::RightBrace;
        case SDLK_TILDE: return Key::Tilde;
        case SDLK_DELETE: return Key::Delete;
        case SDLK_PLUSMINUS: return Key::PlusMinus;
        case SDLK_CAPSLOCK: return Key::CapsLock;
        case SDLK_F1: return Key::F1;
        case SDLK_F2: return Key::F2;
        case SDLK_F3: return Key::F3;
        case SDLK_F4: return Key::F4;
        case SDLK_F5: return Key::F5;
        case SDLK_F6: return Key::F6;
        case SDLK_F7: return Key::F7;
        case SDLK_F8: return Key::F8;
        case SDLK_F9: return Key::F9;
        case SDLK_F10: return Key::F10;
        case SDLK_F11: return Key::F11;
        case SDLK_F12: return Key::F12;
        case SDLK_PRINTSCREEN: return Key::PrintScreen;
        case SDLK_SCROLLLOCK: return Key::ScrollLock;
        case SDLK_PAUSE: return Key::Pause;
        case SDLK_INSERT: return Key::Insert;
        case SDLK_HOME: return Key::Home;
        case SDLK_PAGEUP: return Key::PageUp;
        case SDLK_END: return Key::End;
        case SDLK_PAGEDOWN: return Key::PageDown;
        case SDLK_RIGHT: return Key::Right;
        case SDLK_LEFT: return Key::Left;
        case SDLK_DOWN: return Key::Down;
        case SDLK_UP: return Key::Up;
        case SDLK_NUMLOCKCLEAR: return Key::NumLockClear;
        case SDLK_KP_DIVIDE: return Key::KeypadDivide;
        case SDLK_KP_MULTIPLY: return Key::KeypadMultiply;
        case SDLK_KP_MINUS: return Key::KeypadMinus;
        case SDLK_KP_PLUS: return Key::KeypadPlus;
        case SDLK_KP_ENTER: return Key::KeypadEnter;
        case SDLK_KP_1: return Key::Keypad1;
        case SDLK_KP_2: return Key::Keypad2;
        case SDLK_KP_3: return Key::Keypad3;
        case SDLK_KP_4: return Key::Keypad4;
        case SDLK_KP_5: return Key::Keypad5;
        case SDLK_KP_6: return Key::Keypad6;
        case SDLK_KP_7: return Key::Keypad7;
        case SDLK_KP_8: return Key::Keypad8;
        case SDLK_KP_9: return Key::Keypad9;
        case SDLK_KP_0: return Key::Keypad0;
        case SDLK_KP_PERIOD: return Key::KeypadPeriod;
        case SDLK_APPLICATION: return Key::Application;
        case SDLK_POWER: return Key::Power;
        case SDLK_KP_EQUALS: return Key::KeypadEquals;
        case SDLK_F13: return Key::F13;
        case SDLK_F14: return Key::F14;
        case SDLK_F15: return Key::F15;
        case SDLK_F16: return Key::F16;
        case SDLK_F17: return Key::F17;
        case SDLK_F18: return Key::F18;
        case SDLK_F19: return Key::F19;
        case SDLK_F20: return Key::F20;
        case SDLK_F21: return Key::F21;
        case SDLK_F22: return Key::F22;
        case SDLK_F23: return Key::F23;
        case SDLK_F24: return Key::F24;
        case SDLK_EXECUTE: return Key::Execute;
        case SDLK_HELP: return Key::Help;
        case SDLK_MENU: return Key::Menu;
        case SDLK_SELECT: return Key::Select;
        case SDLK_STOP: return Key::Stop;
        case SDLK_AGAIN: return Key::Again;
        case SDLK_UNDO: return Key::Undo;
        case SDLK_CUT: return Key::Cut;
        case SDLK_COPY: return Key::Copy;
        case SDLK_PASTE: return Key::Paste;
        case SDLK_FIND: return Key::Find;
        case SDLK_MUTE: return Key::Mute;
        case SDLK_VOLUMEUP: return Key::VolumeUp;
        case SDLK_VOLUMEDOWN: return Key::VolumeDown;
        case SDLK_KP_COMMA: return Key::KeypadComma;
        case SDLK_KP_EQUALSAS400: return Key::KeypadEqualsAs400;
        case SDLK_ALTERASE: return Key::AltErase;
        case SDLK_SYSREQ: return Key::SysReq;
        case SDLK_CANCEL: return Key::Cancel;
        case SDLK_CLEAR: return Key::Clear;
        case SDLK_PRIOR: return Key::Prior;
        case SDLK_RETURN2: return Key::Return2;
        case SDLK_SEPARATOR: return Key::Separator;
        case SDLK_OUT: return Key::Out;
        case SDLK_OPER: return Key::Oper;
        case SDLK_CLEARAGAIN: return Key::ClearAgain;
        case SDLK_CRSEL: return Key::CrSel;
        case SDLK_EXSEL: return Key::ExSel;
        case SDLK_KP_00: return Key::Keypad00;
        case SDLK_KP_000: return Key::Keypad000;
        case SDLK_THOUSANDSSEPARATOR: return Key::ThousandsSeparator;
        case SDLK_DECIMALSEPARATOR: return Key::DecimalSeparator;
        case SDLK_CURRENCYUNIT: return Key::CurrencyUnit;
        case SDLK_CURRENCYSUBUNIT: return Key::CurrencySubUnit;
        case SDLK_KP_LEFTPAREN: return Key::KeypadLeftParen;
        case SDLK_KP_RIGHTPAREN: return Key::KeypadRightParen;
        case SDLK_KP_LEFTBRACE: return Key::KeypadLeftBrace;
        case SDLK_KP_RIGHTBRACE: return Key::KeypadRightBrace;
        case SDLK_KP_TAB: return Key::KeypadTab;
        case SDLK_KP_BACKSPACE: return Key::KeypadBackspace;
        case SDLK_KP_A: return Key::KeypadA;
        case SDLK_KP_B: return Key::KeypadB;
        case SDLK_KP_C: return Key::KeypadC;
        case SDLK_KP_D: return Key::KeypadD;
        case SDLK_KP_E: return Key::KeypadE;
        case SDLK_KP_F: return Key::KeypadF;
        case SDLK_KP_XOR: return Key::KeypadXor;
        case SDLK_KP_POWER: return Key::KeypadPower;
        case SDLK_KP_PERCENT: return Key::KeypadPercent;
        case SDLK_KP_LESS: return Key::KeypadLess;
        case SDLK_KP_GREATER: return Key::KeypadGreater;
        case SDLK_KP_AMPERSAND: return Key::KeypadAmpersand;
        case SDLK_KP_DBLAMPERSAND: return Key::KeypadDoubleAmpersand;
        case SDLK_KP_VERTICALBAR: return Key::KeypadVerticalBar;
        case SDLK_KP_DBLVERTICALBAR: return Key::KeypadDoubleVerticalBar;
        case SDLK_KP_COLON: return Key::KeypadColon;
        case SDLK_KP_HASH: return Key::KeypadHash;
        case SDLK_KP_SPACE: return Key::KeypadSpace;
        case SDLK_KP_AT: return Key::KeypadAt;
        case SDLK_KP_EXCLAM: return Key::KeypadExclam;
        case SDLK_KP_MEMSTORE: return Key::KeypadMemStore;
        case SDLK_KP_MEMRECALL: return Key::KeypadMemRecall;
        case SDLK_KP_MEMCLEAR: return Key::KeypadMemClear;
        case SDLK_KP_MEMADD: return Key::KeypadMemAdd;
        case SDLK_KP_MEMSUBTRACT: return Key::KeypadMemSubtract;
        case SDLK_KP_MEMMULTIPLY: return Key::KeypadMemMultiply;
        case SDLK_KP_MEMDIVIDE: return Key::KeypadMemDivide;
        case SDLK_KP_PLUSMINUS: return Key::KeypadPlusMinus;
        case SDLK_KP_CLEAR: return Key::KeypadClear;
        case SDLK_KP_CLEARENTRY: return Key::KeypadClearEntry;
        case SDLK_KP_BINARY: return Key::KeypadBinary;
        case SDLK_KP_OCTAL: return Key::KeypadOctal;
        case SDLK_KP_DECIMAL: return Key::KeypadDecimal;
        case SDLK_KP_HEXADECIMAL: return Key::KeypadHexadecimal;
        case SDLK_LCTRL: return Key::LeftControl;
        case SDLK_LSHIFT: return Key::LeftShift;
        case SDLK_LALT: return Key::LeftAlt;
        case SDLK_LGUI: return Key::LeftGui;
        case SDLK_RCTRL: return Key::RightControl;
        case SDLK_RSHIFT: return Key::RightShift;
        case SDLK_RALT: return Key::RightAlt;
        case SDLK_RGUI: return Key::RightGui;
        case SDLK_MODE: return Key::Mode;
        case SDLK_SLEEP: return Key::Sleep;
        case SDLK_WAKE: return Key::Wake;
        case SDLK_CHANNEL_INCREMENT: return Key::ChannelIncrement;
        case SDLK_CHANNEL_DECREMENT: return Key::ChannelDecrement;
        case SDLK_MEDIA_PLAY: return Key::MediaPlay;
        case SDLK_MEDIA_PAUSE: return Key::MediaPause;
        case SDLK_MEDIA_RECORD: return Key::MediaRecord;
        case SDLK_MEDIA_FAST_FORWARD: return Key::MediaFastForward;
        case SDLK_MEDIA_REWIND: return Key::MediaRewind;
        case SDLK_MEDIA_NEXT_TRACK: return Key::MediaNextTrack;
        case SDLK_MEDIA_PREVIOUS_TRACK: return Key::MediaPreviousTrack;
        case SDLK_MEDIA_STOP: return Key::MediaStop;
        case SDLK_MEDIA_EJECT: return Key::MediaEject;
        case SDLK_MEDIA_PLAY_PAUSE: return Key::MediaPlayPause;
        case SDLK_MEDIA_SELECT: return Key::MediaSelect;
        case SDLK_AC_NEW: return Key::AcNew;
        case SDLK_AC_OPEN: return Key::AcOpen;
        case SDLK_AC_CLOSE: return Key::AcClose;
        case SDLK_AC_EXIT: return Key::AcExit;
        case SDLK_AC_SAVE: return Key::AcSave;
        case SDLK_AC_PRINT: return Key::AcPrint;
        case SDLK_AC_PROPERTIES: return Key::AcProperties;
        case SDLK_AC_SEARCH: return Key::AcSearch;
        case SDLK_AC_HOME: return Key::AcHome;
        case SDLK_AC_BACK: return Key::AcBack;
        case SDLK_AC_FORWARD: return Key::AcForward;
        case SDLK_AC_STOP: return Key::AcStop;
        case SDLK_AC_REFRESH: return Key::AcRefresh;
        case SDLK_AC_BOOKMARKS: return Key::AcBookmarks;
        case SDLK_SOFTLEFT: return Key::SoftLeft;
        case SDLK_SOFTRIGHT: return Key::SoftRight;
        case SDLK_CALL: return Key::Call;
        case SDLK_ENDCALL: return Key::EndCall;
        case SDLK_LEFT_TAB: return Key::LeftTab;
        case SDLK_LEVEL5_SHIFT: return Key::Level5Shift;
        case SDLK_MULTI_KEY_COMPOSE: return Key::MultiKeyCompose;
        case SDLK_LMETA: return Key::LeftMeta;
        case SDLK_RMETA: return Key::RightMeta;
        case SDLK_LHYPER: return Key::LeftHyper;
        case SDLK_RHYPER: return Key::RightHyper;
        default: break;
    }

    return Key::Unknown;
}

int InputImpl::toSDLMouseButton(const MouseButton button)
{
    switch (button)
    {
        case MouseButton::Left: return SDL_BUTTON_LEFT;
        case MouseButton::Right: return SDL_BUTTON_RIGHT;
        case MouseButton::Middle: return SDL_BUTTON_MIDDLE;
        case MouseButton::Extra1: return SDL_BUTTON_X1;
        case MouseButton::Extra2: return SDL_BUTTON_X2;
    }

    return 0;
}

MouseButton InputImpl::fromSDLMouseButton(const int sdlButton)
{
    switch (sdlButton)
    {
        case SDL_BUTTON_LEFT: return MouseButton::Left;
        case SDL_BUTTON_RIGHT: return MouseButton::Right;
        case SDL_BUTTON_MIDDLE: return MouseButton::Middle;
        default: return static_cast<MouseButton>(0);
    }
}

static KeyModifier fromSDLKeymods(Uint16 mods)
{
    return static_cast<KeyModifier>(mods);
}

Pair<Key, KeyModifier> InputImpl::fromSDLKeysym(const SDL_Keycode sdlKey, const SDL_Keymod sdlMod)
{
    return Pair(fromSDLKey(sdlKey), fromSDLKeymods(sdlMod));
}

InputImpl::InputImpl()
    : _previousKeyStates()
    , _keyStates()
    , _previousMouseButtonStates()
    , _mouseButtonStates()
{
}

void InputImpl::createInstance()
{
    assume(not sInputImplInstance);
    sInputImplInstance = new InputImpl();
}

void InputImpl::destroyInstance()
{
    assume(sInputImplInstance);
    delete sInputImplInstance;
}

InputImpl& InputImpl::instance()
{
    assume(sInputImplInstance);
    return *sInputImplInstance;
}

static u32 toArrayIndex(Scancode scancode)
{
    return static_cast<u32>(scancode);
}

static u32 toArrayIndex(MouseButton button)
{
    return static_cast<u32>(static_cast<int>(button) - 1);
}

bool InputImpl::isKeyDown(const Scancode scancode) const
{
    return _keyStates[toArrayIndex(scancode)] == 1;
}

bool InputImpl::wasKeyJustPressed(const Scancode scancode) const
{
    const auto idx = toArrayIndex(scancode);

    return _previousKeyStates[idx] == 0 and _keyStates[idx] == 1;
}

bool InputImpl::wasKeyJustReleased(const Scancode scancode) const
{
    const auto idx = toArrayIndex(scancode);

    return _previousKeyStates[idx] == 1 and _keyStates[idx] == 0;
}

bool InputImpl::isMouseButtonDown(const MouseButton button) const
{
    return _mouseButtonStates[toArrayIndex(button)] == 1;
}

bool InputImpl::wasMouseButtonJustPressed(const MouseButton button) const
{
    const auto idx = toArrayIndex(button);

    return _previousMouseButtonStates[idx] == 0 and _mouseButtonStates[idx] == 1;
}

bool InputImpl::wasMouseButtonJustReleased(const MouseButton button) const
{
    const auto idx = toArrayIndex(button);

    return _previousMouseButtonStates[idx] == 1 and _mouseButtonStates[idx] == 0;
}

void InputImpl::update()
{
    // Key states
    {
        auto        numKeys          = 0;
        const auto* sdlKeyStates     = SDL_GetKeyboardState(&numKeys);
        const auto  sdlKeyStatesSpan = Span(sdlKeyStates, numKeys);

        _previousKeyStates = _keyStates;

        for (u32 i = 0; i < _keyStates.size(); ++i)
        {
            const auto scancode    = static_cast<Scancode>(i);
            const auto sdlScancode = toSDLScancode(scancode);

            _keyStates[i] = sdlKeyStatesSpan[sdlScancode] ? 1 : 0;
        }
    }

    // Mouse buttons
    {
        _previousMouseButtonStates = _mouseButtonStates;

        const auto bits = SDL_GetMouseState(nullptr, nullptr);

        auto setButtonState = [this, bits](MouseButton button, const int sdlButton)
        {
            const auto idx = static_cast<int>(button) - 1;
            assume(idx >= 0);
            const auto isDown       = (bits bitand SDL_BUTTON_MASK(sdlButton)) != 0u;
            _mouseButtonStates[idx] = isDown ? 1 : 0;
        };

        setButtonState(MouseButton::Left, SDL_BUTTON_LEFT);
        setButtonState(MouseButton::Right, SDL_BUTTON_RIGHT);
        setButtonState(MouseButton::Middle, SDL_BUTTON_MIDDLE);
        setButtonState(MouseButton::Extra1, SDL_BUTTON_X1);
        setButtonState(MouseButton::Extra2, SDL_BUTTON_X2);
    }
}

Vec2 InputImpl::mousePositionDelta() const
{
    return _mousePositionDelta;
}

void InputImpl::setMousePositionDelta(const Vec2 value)
{
    _mousePositionDelta = value;
}

Vec2 InputImpl::mouseWheelDelta() const
{
    return _mouseWheelDelta;
}

void InputImpl::setMouseWheelDelta(const Vec2 value)
{
    _mouseWheelDelta = value;
}
} // namespace Polly