# Text Input

There are many ways a user can input text in your game, for example using a keyboard, an on-screen keyboard or a virtual keyboard such as on mobile devices.

## Text Event

The `TextInputEvent` is raised when the user has pressed any physical or virtual key that produced a Unicode character.

To observe it, handle the event using `Game::onTextInput()`:

```cpp
struct MyGame : Game
{
    String text;

    void printText()
    {
        logInfo("Text: {}", text);
    }

    void onTextInput(const TextInputEvent& event) override
    {
        text += event.text;
        printText();
    }

    void onKeyPressed(const KeyEvent& event) override
    {
        if ( event.key == Key::Backspace && !text.empty() )
        {
            text.removeLast();
            printText();
        }
    }
};
```

## On-Screen Keyboards

To show or hide the system's on-screen keyboard, use the `Window::startAcceptingTextInput()` and `Window::stopAcceptingTextInput()` methods, respectively.

Typing on an on-screen keyboard will produce `TextInputEvent` events.

```cpp
void MyGame::playerInputRequired()
{
    window().startAcceptingTextInput();
}

void MyGame::playerInputFinished()
{
    window().stopAcceptingTextInput();
}
```
