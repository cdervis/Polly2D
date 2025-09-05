# Samplers

[Drawing sprites](/graphics/sprites) shows how the destination rectangle is able to scale a sprite, disregarding its image size.
This is thanks to image interpolation, which is controllable via `Painter::setSampler()`.

To illustrate this, let's add a low-resolution image, for example this small 32×32 image:
![img](/assets/images/test32x32.png){.align-center}

A `Sampler` describes how an image is interpolated and repeated across image coordinate boundaries.
This is especially useful if you desire a pixelated look for your game, or for effects such as texture scrolling.

Now load the 32×32 image and draw it upscaled using a destination rectangle:

```cpp
image = Image("Test32x32.png");

// Draw the image at position {200, 200}, but scale it up to 300x300 pixels.
painter.drawSprite(Sprite {
    .image   = image,
    .dstRect = Rectangle(200, 200, 300, 300),
});
```

The image should now be upscaled and blurry, since by default Polly uses linear interpolation:
![img](/assets/images/samplers-upscaled.webp){.align-center}

Now set a sampler before drawing the sprite that disables image interpolation:

```cpp
painter.setSampler(Sampler {
    .filter   = ImageFilter::Point,
    .addressU = ImageAddressMode::ClampToEdgeTexels,
    .addressV = ImageAddressMode::ClampToEdgeTexels,
});

painter.drawSprite(Sprite {
    .image   = image,
    .dstRect = Rectangle(200, 200, 300, 300),
});
```

This will result in a pixelated sprite:
![img](/assets/images/samplers-pixelated.webp){.align-center}

`filter` refers to the interpolation mode. Point uses nearest neighbor filtering. The `addressU` and `addressV` fields refer to how
the image is sampled when coordinates fall outside the image bounds. `ClampToEdgeTexels`, for example, specifies that every pixel that
lies outside the image bounds results in the image border's color. `addressU` specifically refers to pixels in the X-axis of the image,
while `addressV` refers to pixels in the Y-axis.

As an example, try changing the `addressU` value to `Repeat` and `addressV` to `Mirror`:

```cpp
painter.setSampler(Sampler {
    .filter   = ImageFilter::Point,
    .addressU = ImageAddressMode::Repeat,
    .addressV = ImageAddressMode::Mirror,
});

painter.drawSprite(Sprite {
    .image   = image,
    .dstRect = Rectangle( 200, 200, 300, 300 ),
    .srcRect = Rectangle( 0, 0, 128, 128 ),
});
```

We can now see that the image is repeated across the X-axis and mirrored across the Y-axis:
![img](/assets/images/samplers-repeat-mirror.webp){.align-center}

The image repeats four times, since we specified a source rectangle size of 128×128 pixels, while the image is 32×32. Remember that you could use the source rectangle to implement texture scrolling. If you are curious, try using the game's total running time ([`GameTime::totalTime`](/docs/api/Core/GameTime#fields)) as the X or Y value for the source rectangle's position.

The default sampler is equivalent to `linearClamp` (see below).

## Predefined Samplers

Polly provides the following predefined samplers:

```cpp
constexpr auto pointRepeat = Sampler {
    .filter   = ImageFilter::Point,
    .addressU = ImageAddressMode::Repeat,
    .addressV = ImageAddressMode::Repeat,
}

constexpr auto pointClamp = Sampler {
    .filter   = ImageFilter::Point,
    .addressU = ImageAddressMode::ClampToEdgeTexels,
    .addressV = ImageAddressMode::ClampToEdgeTexels,
}

constexpr auto linearRepeat = Sampler {
    .filter   = ImageFilter::Linear,
    .addressU = ImageAddressMode::Repeat,
    .addressV = ImageAddressMode::Repeat,
}

constexpr auto linearClamp = Sampler {
    .filter   = ImageFilter::Linear,
    .addressU = ImageAddressMode::ClampToEdgeTexels,
    .addressV = ImageAddressMode::ClampToEdgeTexels,
}
```

Using a predefined sampler is as simple as:

```cpp
painter.setSampler(pointRepeat);
```