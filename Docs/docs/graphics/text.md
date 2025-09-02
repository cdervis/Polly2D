# Text

## Drawing Text

Drawing text is not that different from drawing sprites.
However, since text can be understood as a group of sprites, where each character is a sprite, Polly's API provides you with some extra functionality. In this page, we'll go through all functions one by one.

First, you need a font.

Polly comes with an embedded font that is directly available via the static `Font::builtin()` method. Alternatively, you can drop a font file (.ttf or .otf) into your game's `assets` and load it directly.

```cpp
auto font = Font("SomeFont.ttf");
```

Because it wouldn't make much of a difference in the following examples, we'll stick to the built-in font on this page.

Text is drawn using `Painter::drawString()` which takes a `Font`, a size and the string to draw. The string is expected to be UTF-8-encoded.

In your game's `draw()` method, simply call `drawString()` using the font object and give it some text:

```cpp
void draw(Painter painter) override
{
    const auto font = Font::builtin();
    
    // Draw some text at position {100, 100}.
    painter.drawString("Hello World!", font, /*fontSize: */ 48, Vec2(100, 100));
    
    // Draw another text at position {100, 200}, and with color red.
    painter.drawString("Hello World 2!", font, /*fontSize: */ 32, Vec2(100, 200), yellow);
}
```

This will draw some text as expected:
![img](/assets/images/drawing_text1.webp){.align-center}

**Note**, you can also pass an empty `Font` object, which is the same as using `Font::builtin()`.
Example:

```cpp
// Passing 'none' as a font is the same as using Font::builtin().
painter.drawString("Hello World!", none, 32, Vec2(100, 100));
```

### Text Decorations

For cases when text should be highlighted or otherwise hint at certain information, `drawString()` provides a way to decorate text, namely using the `TextDecoration` type.

We can for example draw strikethrough or underlined text using the respective `Strikethrough` and `Underline` types.

#### Text Strikethrough

```cpp
// Draw text with a strikethrough line.
painter.drawString(text, font, 48, Vec2(100, 100), white, Strikethrough{});

// Draw text with a strikethrough line,
// but specify a custom color and thickness.
painter.drawString(text, font, 48, Vector2( 400, 100 ), white, Strikethrough {
    .color = red,
    .thickness = 10.0f,
});
```

#### Text Underline

```cpp
// Same for an underline.
painter.drawString(text, font, 48, Vec2(100, 200), yellow, Underline{});

painter.drawString(text, font, 48, {400, 200}, yellow, Underline {
    .color = red,
    .thickness = 10.0f,
});
```

Which produces the following:
![img](/assets/images/drawing_text2.webp){.align-center}

### Text Samplers

::: callout info
This assumes that you've read [Samplers](/graphics/samplers).
:::

It's important to note that the active sampler (set by `Painter::setSampler()`) also affects how text is drawn. This is by design, since it allows you to draw pixelated fonts easily. Just set a nearest-neighbor sampler (i.e. `pointClamp`) and then draw your text as usual.

```cpp title="Setting a sampler for text drawing"
// Disable interpolation.
painter.setSampler(pointClamp);
painter.drawString(...);
painter.drawString(...);
// ...

// Reenable interpolation.
painter.setSampler(linearRepeat);
// ...
```

### Text Shaping

The current text layouting algorithm in Polly is rudimentary and doesn't cover many corner cases.
For example, it doesn't perform full Unicode-conforming text shaping, which for example makes rendering of arabic text currently impossible. Full text shaping support using ICU and HarfBuzz is currently in development and will be available in a future release.

# Handling Text 

While drawing text is enough for many cases, there are some cases where you want to gain some information about text you're about to draw. Think of user interfaces where text elements are neighbors of each other. This requires some kind of layouting algorithm, which in turn relies on the size of text elements so that it can properly figure out where to put each element.

For such cases, `Font` provides methods such as `measure()`, `lineHeight()` and `forEachGlyph()`.

::: callout info
This assumes that you've read [Drawing Text](/graphics/text#drawing-text).
:::

## Measuring

We'll first go over the `measure()` method. It allows us to get the exact size of a text, in pixels, given a font and a size.

```cpp
auto font     = Font::builtin();
auto text     = "Hello World!\nThis is line two."_sv;
auto fontSize = 48.0f;

// Measure the text we're going to draw.
auto textSize     = font.measure(text, fontSize);
auto textPosition = Vec2(100, 100);

// Draw a rectangle first to show that we know the text size correctly.
painter.fillRectangle(Rectangle(textPosition, textSize), yellow);

painter.drawString(text, font, fontSize, textPosition, black);
```

This should give us a yellow rectangle with black text on top:
![img](/assets/images/handling_text1.webp){.align-center}

Imagine we had to draw multiple independent text objects next to one another, including a margin between each of them. With this information we can now implement such a case, just like this:

```cpp
// Use the same font for all texts.
auto font = Font::builtin();

// These are the texts we have to draw.
auto texts = Array {
    std::tuple("Hello World!\nThis is line two."_sv, 32, white),
    std::tuple("This is text 2"_sv, 48, yellow),
    std::tuple("... and this is text 3!"_sv, 64, black),
};

// The spacing between each text element.
constexpr auto spacing = 30.0f;

// This will be our "pen" that defines where we start drawing text.
// For this example, we'll just draw text at a fixed Y-position.
auto position = Vec2(50, 100);

for (const auto& [text, size, color] : texts)
{
    auto textSize = font.measure(text, size);

    painter.drawString(text, font, size, position, color);

    // Advance our pen position.
    position.x += textSize.x + spacing;
}
```

This will greet us with all texts neatly lined up:
![img](/assets/images/handling_text2.webp){.align-center}

Next up is the `lineHeight()` method. It gives you the fixed height of the font itself, **in pixels**. This is for example used to align text objects next to each other vertically ("line by line").

Let's see an example:

```cpp
auto font = Font::builtin();

auto texts = Array {
    std::tuple("Hello World! This is line 1."_sv, 48, white),
    std::tuple("This is line 2, and tiny"_sv, 24, yellow),
    std::tuple("... and this is line 3!"_sv, 48, black),
};

// X and Y are swapped this time, since we're incrementally
// drawing text downwards.
auto position = Vec2(100, 50);

for (const auto& [text, size, color] : texts)
{
  auto lineHeight = font.lineHeight(size);

  painter.drawString(text, font, size, position, color);

  // Advance our pen position
  position.y += lineHeight;
}
```

This gives us perfectly aligned text:
![img](/assets/images/handling_text3.webp){.align-center}

## Glyph Actions

Lastly, the most important of all: `forEachGlyph()`. This is the method all others are based on.
Given a text, size and a `Function`, it allows you to iterate each glyph of a string and perform an arbitrary action. For each iteration, your function gets information about the glyph such as codepoint and position.

Let's look at its signature:

```cpp
using GlyphAction = Function<bool(char32_t codepoint, const Rectangle& rect)>;

// ...

void forEachGlyph( StringView text, float size, const GlyphAction& action ) const;
```

We see that we have to give it a function that takes the glyph's Unicode codepoint and its occupied area within the text, in pixels.
Additionally, it has to return a bool value to indicate whether to keep going. If it returns `false`, the iteration stops after that glyph. `forEachGlyph()` itself returns nothing, as it's just a forward-iterating method.

With this, we could even implement a custom text rendering function. But for this example we'll keep it simple and print glyph information to the console, just to show that it works:

```cpp
auto font     = Font::builtin();
auto fontSize = 32.0f;
auto text     = "Hello\nWorld!"_sv;

font.forEachGlyph(text, fontSize, [](char32_t codepoint, const Rectangle& rect) {
  logInfo("Got glyph {} at {}", char(codepoint), rect);
  return true; // Keep going
});
```

Which prints the following to the console:

```cpp
Got glyph H at [ x=0; y=4.5167785; width=16; height=21 ]
Got glyph e at [ x=20.67114; y=9.516779; width=15; height=17 ]
Got glyph l at [ x=37.583893; y=4.5167785; width=4; height=21 ]
Got glyph l at [ x=45.221478; y=4.5167785; width=4; height=21 ]
Got glyph o at [ x=52.859062; y=9.516779; width=15; height=17 ]
Got glyph W at [ x=0; y=36.516777; width=27; height=21 ]
Got glyph o at [ x=25.570469; y=41.516777; width=15; height=17 ]
Got glyph r at [ x=42.389263; y=41.516777; width=10; height=16 ]
Got glyph l at [ x=53.691277; y=36.516777; width=4; height=21 ]
Got glyph d at [ x=61.328857; y=36.516777; width=14; height=22 ]
Got glyph ! at [ x=78.77853; y=36.516777; width=3; height=21 ]
```

We can observe that the x-position increments steadily until the 'W' is encountered, which starts at a new line. `x` is therefore reset, and `y` is now offset. `x` increments steadily while `y` remains the same until the end of the string.

There is one use case where `forEachGlyph()` really shines, which is hit detection.
When implementing UI controls such as text boxes, the correct position for the caret must be determined.

This is only possible when we have information about each character's location inside such a text box. Whenever a user clicks somewhere inside the text box, we can iterate its text and compare the clicked position to each glyphs area.

Conversely, when the caret's position is already known and the user presses the left or right key to go to the previous or next glyph, we can correctly move the caret, since we know the exact positions of each glyph.

## DPI Awareness

The text size we specify in text-related functions is given in pixels. However, different displays might have different pixel densities (DPI). Polly doesn't handle drawing at different densities itself, but gives you the information so that you can handle it.

When working with text, you should obtain the current DPI scaling factor using `Window::pixelRatio()` of the game's window.
This gives you a floating-point value by which you have to scale the font size, so that you get the "real" pixel size for that window's display.

An example:

```cpp
auto fontSize     = 32.0f;
auto pixelRatio   = window.pixelRatio();

// Calculate the font size that's appropriate for the current display scaling factor.
auto realFontSize = fontSize * pixelRatio;

painter.drawString("Hello World!", myFont, realFontSize);
```

The same is true for other text-related functions such as `Font::measure()` and `Font::forEachGlyph()`.
