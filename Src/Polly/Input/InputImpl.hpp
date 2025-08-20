// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Array.hpp"
#include "Polly/Input.hpp"
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

    static Key fromSDLKey(SDL_Keycode sdl_key);

    static int toSDLMouseButton(MouseButton button);

    static MouseButton fromSDLMouseButton(int sdl_button);

    static Pair<Key, KeyModifier> fromSDLKeysym(SDL_Keycode sdl_key, SDL_Keymod sdl_mod);

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
    using key_state_array          = Array<u8, static_cast<size_t>(Scancode::EndCall)>;
    using mouse_button_state_array = Array<u8, static_cast<size_t>(MouseButton::Extra2)>;

    key_state_array          _previousKeyStates;
    key_state_array          _keyStates;
    Vec2                     _mousePositionDelta;
    Vec2                     _mouseWheelDelta;
    mouse_button_state_array _previousMouseButtonStates;
    mouse_button_state_array _mouseButtonStates;
};
} // namespace Polly