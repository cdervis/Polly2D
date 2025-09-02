// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Array.hpp"
#include "Polly/Key.hpp"
#include "Polly/KeyModifier.hpp"
#include "Polly/Linalg.hpp"
#include "Polly/MouseButton.hpp"
#include "Polly/Pair.hpp"
#include "Polly/Scancode.hpp"

#include <SDL3/SDL.h>

namespace Polly
{
class InputImpl final
{
  public:
    InputImpl();

    static void createInstance();

    static void destroyInstance();

    static InputImpl& instance();

    static int toSDLScancode(Scancode scancode);

    static int toSDLKey(Key key);

    static Key fromSDLKey(SDL_Keycode sdlKey);

    static int toSDLMouseButton(MouseButton button);

    static MouseButton fromSDLMouseButton(int sdlButton);

    static Pair<Key, KeyModifier> fromSDLKeysym(SDL_Keycode sdlKey, SDL_Keymod sdlMod);

    bool isKeyDown(Scancode scancode) const;

    bool wasKeyJustPressed(Scancode scancode) const;

    bool wasKeyJustReleased(Scancode scancode) const;

    bool isMouseButtonDown(MouseButton button) const;

    bool wasMouseButtonJustPressed(MouseButton button) const;

    bool wasMouseButtonJustReleased(MouseButton button) const;

    void update();

    Vec2 mousePositionDelta() const;

    void setMousePositionDelta(Vec2 value);

    Vec2 mouseWheelDelta() const;

    void setMouseWheelDelta(Vec2 value);

  private:
    using KeyStateArray         = Array<u8, static_cast<size_t>(Scancode::EndCall)>;
    using MouseButtonStateArray = Array<u8, static_cast<size_t>(MouseButton::Extra2)>;

    KeyStateArray         _previousKeyStates;
    KeyStateArray         _keyStates;
    Vec2                  _mousePositionDelta;
    Vec2                  _mouseWheelDelta;
    MouseButtonStateArray _previousMouseButtonStates;
    MouseButtonStateArray _mouseButtonStates;
};
} // namespace Polly