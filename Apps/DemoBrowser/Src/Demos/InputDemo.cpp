#include "InputDemo.hpp"

#include "DemoBrowser.hpp"

InputDemo::InputDemo(DemoBrowser* browser)
    : Demo("Input", browser)
{
    // Show the Polly log on-screen.
    browser->window().setIsLogVisible(true);
}

InputDemo::~InputDemo() noexcept
{
    browser().window().setIsLogVisible(false);
}

void InputDemo::update([[maybe_unused]] GameTime time)
{
    _mousePos       = currentMousePosition();
    _mouseMoveDelta = currentMousePositionDelta();

    // Check the state of a key directly:
    if (isKeyDown(Scancode::Space))
    {
        logInfo("'Space' key is being pressed (polled)");
    }
}

void InputDemo::draw(Painter painter)
{
    // Draw mouse information.
    {
        // Draw an ellipse where the mouse currently is.
        // The mouse movement influences the ellipse's radius and color.
        const auto pixelRatio    = browser().window().pixelRatio();
        const auto mouseMovement = length(_mouseMoveDelta);
        const auto radius        = clamp(mouseMovement * 2.0f, 15.0f, 50.0f);
        const auto color         = lerp(lime, red, clamp(mouseMovement * 0.02f, 0.0f, 1.0f));

        painter.drawEllipse(_mousePos * pixelRatio, Vec2(radius), color, 5.0f);
    }

    // If any gamepads are connected, draw information about the first one.
    if (const auto gamepads = browser().gamepads(); !gamepads.isEmpty())
    {
        const auto& gamepad = gamepads.first();

        constexpr auto axisCircleRadius   = Vec2(100.0f, 100.0f);
        constexpr auto stickRadius        = Vec2(50, 50);
        constexpr auto actionButtonRadius = Vec2(30, 30);
        constexpr auto triggerRectHeight  = 200.0f;

        // Left stick
        {
            const auto xValue       = gamepad.axisValue(GamepadAxis::LeftX);
            const auto yValue       = gamepad.axisValue(GamepadAxis::LeftY);
            const auto triggerValue = gamepad.axisValue(GamepadAxis::LeftTrigger);

            constexpr auto stickPos = Vec2(800, 700);
            const auto     xyOffset = Vec2(xValue, yValue) * axisCircleRadius;

            if (gamepad.isButtonDown(GamepadButton::LeftStick))
            {
                painter.fillEllipse(stickPos, axisCircleRadius, darkGray);
            }

            painter.drawEllipse(stickPos, axisCircleRadius, gray, 3.0f);

            painter.fillEllipse(stickPos + xyOffset, stickRadius, cornflowerblue);

            auto triggerRect = Rectangle(
                stickPos.x - axisCircleRadius.x - 100.0f,
                stickPos.y - (triggerRectHeight / 2),
                50.0f,
                triggerRectHeight);

            painter.drawRoundedRectangle(triggerRect, 8.0f, darkGray, 3.0f);

            if (triggerValue > 0.05f)
            {
                triggerRect.height *= triggerValue;
                painter.fillRoundedRectangle(triggerRect, 8.0f, cornflowerblue);
            }
        }

        // Right stick
        {
            const float xValue       = gamepad.axisValue(GamepadAxis::RightX);
            const float yValue       = gamepad.axisValue(GamepadAxis::RightY);
            const float triggerValue = gamepad.axisValue(GamepadAxis::RightTrigger);

            constexpr auto stickPos = Vec2(1400, 1000);
            const auto     xyOffset = Vec2(xValue, yValue) * axisCircleRadius;

            if (gamepad.isButtonDown(GamepadButton::RightStick))
            {
                painter.fillEllipse(stickPos, axisCircleRadius, darkGray);
            }

            painter.drawEllipse(stickPos, axisCircleRadius, gray, 3.0f);
            painter.fillEllipse(stickPos + xyOffset, stickRadius, cornflowerblue);

            auto triggerRect = Rectangle(
                stickPos.x + axisCircleRadius.x + 50.0f,
                stickPos.y - (triggerRectHeight / 2),
                50.0f,
                triggerRectHeight);

            painter.drawRoundedRectangle(triggerRect, 8.0f, darkGray, 3.0f);

            if (triggerValue > 0.05f)
            {
                triggerRect.height *= triggerValue;
                painter.fillRoundedRectangle(triggerRect, 8.0f, cornflowerblue);
            }
        }

        // Action buttons
        {
            constexpr auto westPos  = Vec2(1380, 700);
            constexpr auto southPos = Vec2(1450, 770);
            constexpr auto eastPos  = Vec2(1520, 700);
            constexpr auto northPos = Vec2(1450, 630);

            painter.drawEllipse(northPos, actionButtonRadius, yellow, 3.0f);
            painter.drawEllipse(eastPos, actionButtonRadius, red, 3.0f);
            painter.drawEllipse(southPos, actionButtonRadius, lime, 3.0f);
            painter.drawEllipse(westPos, actionButtonRadius, cornflowerblue, 3.0f);

            if (gamepad.isButtonDown(GamepadButton::ActionWest))
            {
                painter.fillEllipse(westPos, actionButtonRadius, cornflowerblue);
            }

            if (gamepad.isButtonDown(GamepadButton::ActionSouth))
            {
                painter.fillEllipse(southPos, actionButtonRadius, lime);
            }

            if (gamepad.isButtonDown(GamepadButton::ActionEast))
            {
                painter.fillEllipse(eastPos, actionButtonRadius, red);
            }

            if (gamepad.isButtonDown(GamepadButton::ActionNorth))
            {
                painter.fillEllipse(northPos, actionButtonRadius, yellow);
            }
        }

        // D-pad
        {
            constexpr auto leftPos  = Vec2(700, 1000);
            constexpr auto upPos    = Vec2(800, 900);
            constexpr auto rightPos = Vec2(900, 1000);
            constexpr auto downPos  = Vec2(800, 1100);
            constexpr auto radius   = 30.0f;

            painter.drawDirectedTriangle(leftPos, radius, Direction::Left, gray, 3.0f);
            painter.drawDirectedTriangle(upPos, radius, Direction::Up, gray, 3.0f);
            painter.drawDirectedTriangle(rightPos, radius, Direction::Right, gray, 3.0f);
            painter.drawDirectedTriangle(downPos, radius, Direction::Down, gray, 3.0f);

            if (gamepad.isButtonDown(GamepadButton::DPadLeft))
            {
                painter.fillDirectedTriangle(leftPos, radius, Direction::Left, lightGray);
            }

            if (gamepad.isButtonDown(GamepadButton::DPadUp))
            {
                painter.fillDirectedTriangle(upPos, radius, Direction::Up, lightGray);
            }

            if (gamepad.isButtonDown(GamepadButton::DPadRight))
            {
                painter.fillDirectedTriangle(rightPos, radius, Direction::Right, lightGray);
            }

            if (gamepad.isButtonDown(GamepadButton::DPadDown))
            {
                painter.fillDirectedTriangle(downPos, radius, Direction::Down, lightGray);
            }
        }
    }
}

void InputDemo::onImGui(ImGui imgui)
{
    // Show some dynamic information in the sidebar.
    imgui.text("Mouse Position: %.1f; %.1f", _mousePos.x, _mousePos.y);
    imgui.text("Mouse Delta: %.1f; %.1f", _mouseMoveDelta.x, _mouseMoveDelta.y);

    const auto gamepads = browser().gamepads();

    if (!gamepads.isEmpty())
    {
        const auto& gamepad = gamepads.first();

        imgui.newLine();
        imgui.separatorWithText("Gamepad");

        imgui.text("Left X-axis: %.3f", gamepad.axisValue(GamepadAxis::LeftX));
        imgui.text("Left Y-axis: %.3f", gamepad.axisValue(GamepadAxis::LeftY));
        imgui.text("Left Trigger: %.3f", gamepad.axisValue(GamepadAxis::LeftTrigger));

        imgui.text("Right X-axis: %.3f", gamepad.axisValue(GamepadAxis::RightX));
        imgui.text("Right Y-axis: %.3f", gamepad.axisValue(GamepadAxis::RightY));
        imgui.text("Right Trigger: %.3f", gamepad.axisValue(GamepadAxis::RightTrigger));
    }
}

void InputDemo::onKeyPressed(KeyEvent e)
{
    logInfo("Key pressed: {}; modifiers = {}; isRepeat = {}", e.key, e.modifiers, e.isRepeat);
}

void InputDemo::onKeyReleased(KeyEvent e)
{
    logInfo("Key released: {}; modifiers = {}; isRepeat = {}", e.key, e.modifiers, e.isRepeat);
}

void InputDemo::onMouseButtonPressed(MouseButtonEvent event)
{
    logInfo("Mouse button '{}' pressed at {}", event.button, event.position);
}

void InputDemo::onMouseButtonReleased(const MouseButtonEvent& event)
{
    logInfo("Mouse button '{}' released at {}", event.button, event.position);
}

void InputDemo::onMouseWheelScrolled(const MouseWheelEvent& event)
{
    logInfo("Mouse wheel scrolled with delta {}", event.delta);
}

void InputDemo::onGamepadConnected(GamepadEvent event)
{
    logInfo("Gamepad connected: {}", event.gamepad.name());
}

void InputDemo::onGamepadDisconnected(GamepadEvent event)
{
    logInfo("Gamepad disconnected: {}", event.gamepad.name());
}
