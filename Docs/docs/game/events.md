# Events

The `Game` class provides a way to observe the various events that occur during the game's lifecycle.

The most important events are `update()` and `draw()`:

- `update()` is responsible for updating the game's logic.
- `draw()` is responsible for drawing the game's visuals.

```cpp
struct MyGame : Game
{
    void update(GameTime time) override
    {
        // Update the game's logic here using 'time'.
    }
    
    void draw(Painter painter) override
    {
        // Draw the game here using 'painter'.
    }
};
```

There are many more types of events however, such as when a keyboard key is pressed or when a touch display receives a touch.

Every method in the game class that starts with `on...()` represents a way to observe an event and can be overridden with `override`. Example:

```cpp
void onKeyPressed(KeyEvent event) override
{
    logInfo("Key {} was pressed!", event.key);
}

void onMouseButtonPressed(MouseButtonEvent event) override
{
    logInfo("Some mouse button was pressed!");
}
```

## List of Events

The following table lists all events that are provided by the `Game` class:

| Name | Description |
|------|-------------|
| `update(GameTime)` | Called once per frame to update the game's logic. |
| `draw(Painter)` | Called when the game should be drawn. |
| `onStartedRunning()` | Called just after the game has started running, before the first call to `update()`. |
| `onImGui(ImGui)` | Always called at the appropriate time, once per frame. Allows the game to perform ImGui handling. ImGui elements are drawn after `draw()`. |
| `onWindowShown(WindowEvent)` | Called when the game's window becomes visible. |
| `onWindowHidden(WindowEvent)` | Called when the game's window becomes hidden. |
| `onWindowMoved(WindowEvent)` | Called when the game's window is moved. |
| `onWindowResized(WindowResizedEvent)` | Called when the game's window is resized **non-programmatically**. |
| `onWindowMinimized(WindowEvent)` | Called when the game's window is minimized. |
| `onWindowMaximized(WindowEvent)` | Called when the game's window is maximized. |
| `onGotMouseFocus(WindowEvent)` | Called when the game's window receives mouse focus. |
| `onLostMouseFocus(WindowEvent)` | Called when the game's window loses mouse focus. |
| `onGotKeyboardFocus(WindowEvent)` | Called when the game's window receives keyboard focus, for example by alt-tabbing. |
| `onLostKeyboardFocus(WindowEvent)` | Called when the game's window loses keyboard focus. |
| `onWindowClosed(WindowEvent)` | Called when the game's window is closed. |
| `onKeyPressed(KeyEvent)` | Called when a keyboard key is pressed. |
| `onKeyReleased(KeyEvent)` | Called when a keyboard key is released. |
| `onMouseMoved(MouseMoveEvent)` | Called when the mouse has moved. |
| `onMouseButtonPressed(MouseButtonEvent)` | Called when a mouse button is pressed. |
| `onMouseButtonReleased(MouseButtonEvent)` | Called when a mouse button is released. |
| `onMouseWheelScrolled(MouseWheelEvent)` | Called when the mouse wheel is scrolled. |
| `onTouch(TouchFingerEvent)` | Called when a touch display has received a touch. |
| `onGamepadConnected(GamepadEvent)` | Called when a gamepad is connected. |
| `onGamepadDisconnected(GamepadEvent)` | Called when a gamepad is connected. |
| `onTextInput(TextInputEvent)` | Called when the game's window receives text input. |
| `onDisplayOrientationChanged(DisplayOrientationChangedEvent)` | Called when the system's display orientation changed. |
| `onDisplayAdded(DisplayEvent)` | Called when a display is added to the system. |
| `onDisplayRemoved(DisplayEvent)` | Called when a display is removed from the system. |
| `onDisplayMoved(DisplayEvent)` | Called when a display is moved virtually. |
| `onDisplayDesktopModeChanged(DisplayEvent)` | Called when the system's desktop mode changes. |
| `onDisplayCurrentModeChanged(DisplayEvent)` | Called when the system's current display mode changes. |
| `onDisplayContentScaleChanged(DisplayEvent)` | Called when the system's content scaling factor changes. |
