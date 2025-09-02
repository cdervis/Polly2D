# Custom Shaders

Custom shading is necessary when you want to add certain effects to your sprites that can't be achieved with Polly's built-in drawing.

You might want to (de)saturate your sprites based on the game's state, or add a power-up effect on top of your sprites which require more sophisticated equations.

Further advanced effects include shaders where scene information is utilized, such as dynamic 2D lighting and shadowing.

With custom shaders, you can decide a sprite's color at pixel-level with logic as sophisticated as you want (or, as the hardware allows). We'll go over some examples of how you can easily add shaders to your game.

## A Simple Shader

First, create a new file in your assets folder called `"MyShader.shd"`, with the following contents:

```cpp
#type sprite

Vec4 main()
{
    return Vec4(0, 1, 0, 1);
}
```

This shader declares that it outputs a color with the RGBA components being `0, 1, 0, 1`, meaning it's fully green and fully opaque. The shader's type is specified in the first line (`#type sprite`).

Next, load the shader and in your draw method, set it as active:

```cpp
auto shader = Shader("MyShader.shd");

void draw(Painter painter) override
{
    // ...
    
    painter.setSpriteShader(shader);
    
    painter.drawSprite(image, Vec2(100, 100));
    
    // ...
}
```

You should now expect a fully green rectangle:
![img](/assets/images/custom_shaders1.webp){.align-center}

## Built-in Variables

Every sprite shader automatically has access to the sprite's properties:

| Variable | Description |
|----------|-------------|
| `pl_spriteImage` | The sprite's image, which corresponds to `Sprite::image` |
| `pl_spriteColor` | The sprite's color, which corresponds to `Sprite::color` |
| `pl_spriteUV` | The sprite's image (UV) coordinates, which corresponds to `Sprite::srcRect` |

_For the list of all available variables, see [Shading Language](/graphics/shading-language)._

As a test, modify the shader so that it outputs the sprite's UV coordinates as a color, where red is the X and green is the Y component:

```cpp
Vec4 main()
{
    return Vec4(pl_spriteUV, 0, 1);
}
```

This gives you the following image:
![img](/assets/images/custom_shaders2.webp){.align-center}

As you can see, the upper-left corner is black, meaning that a `Vec2(0, 0)` was the result.
As we continue along the X-axis, the X component (red) increases.
And as we continue along the Y-axis, the Y component (green) increases.
So it makes sense that the bottom-right corner is yellow, since both components are 1.

For example, the default sprite shader uses this coordinate to sample the sprite image.
Its code is simply this:

```cpp
return sample(pl_spriteImage, pl_spriteUV) * pl_spriteColor;
```

Imagine an image of size 32×32 was laid on top of the sprite's rectangle.
As we use the sample function, we pass the image coordinate to it (second argument).
This coordinate is then used to read the image at that location.

The coordinates range from `[0.0 .. 1.0]`, where 0 is the left-most or upper-most pixel and 1 is the right-most or bottom-most pixel location.

So `sample(spriteImage, Vec2(0, 0))` would give us the pixel value at location `{0 * 32, 0 * 32}`.

On the other hand, sampling at `Vec2(1, 1)` would give us the pixel value at location `{1 * 32, 1 * 32}`.

Logically, `Vec2(0.5, 0.5)` would then be the center pixel value.
The GPU automatically performs the interpolation for us when we use the sample function, depending on which `Sampler` is active at that time.

To implement image tiling, you would simply pass an image coordinate that goes beyond the `[0.0 .. 1.0]` range and use a `Sampler` that uses `ImageAddressMode::Repeat` or similar. To implement image scrolling, you simply add some offset to your image coordinates.

## Modifying Shader Parameters

Let's have some fun and add a shader parameter that we can control from within our game.

Modify the shader code as follows:

```cpp
float redIntensity;
float greenIntensity;

Vec4 main()
{
    return Vec4(redIntensity, greenIntensity, 0, 1);
}
```

We can set these parameters from wherever we want; the shader object stores them in a persistent manner. As long as it's done before the shader is set as active, it doesn't matter.
Example:

```cpp
struct MyGame : Game
{
    Shader shader = Shader("MyShader.shd");
    Image image = Image("MyImage.png");

    MyGame()
    {
        // Set the parameter's initial value explicitly.
        shader.set("redIntensity", 1.0f);
    }

    void update(GameTime time) override
    {
        // Set the parameter's value every update.
        shader.set("greenIntensity", 1.0f);
    }

    void draw(Painter painter) override
    {
        painter.setSpriteShader(shader);
        painter.drawSprite(myImage, Vec2(100, 100));
    }
};
```

We should now see a yellow rectangle:

![img](/assets/images/custom_shaders3.webp){.align-center}

Let's make it more interactive and update these values when the player presses certain keys.
Change the `update()` method as follows:

```cpp
// ...

void update(GameTime time) override
{
  auto dt         = time.elapsed();
  auto redValue   = *shader.floatValue("redIntensity");
  auto greenValue = *shader.floatValue("greenIntensity");

  if (isKeyDown(Scancode::Left))
  {
    shader.set("redIntensity", clampZeroOne(redValue - dt));
  }

  if (isKeyDown(Scancode::Right))
  {
    shader.set("redIntensity", clampZeroOne(redValue + dt));
  }

  if (isKeyDown(Scancode::Up))
  {
    shader.set("greenIntensity", clampZeroOne(greenValue + dt));
  }

  if (isKeyDown(Scancode::Down))
  {
    shader.set("greenIntensity", clampZeroOne(greenValue - dt));
  }
}
```

We modify the red intensity using the left / right arrow keys, and the green intensity using the up / down arrow keys.

To do that, we use `Shader::floatValue()` to obtain the parameter's current value.

**Note** that we must dereference it, because all `Shader::...Value()` overloads return a `Maybe`.

This is because it's possible that we're asking for the value of a parameter that doesn't exist in the shader. This happens when we either specify an incorrect name or when the parameter was optimized away by the shader compiler.

A safer approach would be to use `.valueOr(0.0f)`. In our case, we know that this parameter exists, since we're using it in the shader and therefore it can't be optimized away.

Using `set()` doesn't have this problem.

If a parameter name was incorrect or the parameter doesn't exist, the method simply won't do anything.
In other words, it's safe to use `SetValue()` even if you're unsure about the parameter's existence.

So now, depending on which key was pressed, we increment / decrement the corresponding intensity by the delta time. Additionally, we clamp the result to the range `[0.0 .. 1.0]`.

We should now be able to modify the red and green components separately.

