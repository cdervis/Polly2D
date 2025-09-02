# Spine

Polly integrates the [Spine](https://esotericsoftware.com) C++ runtime to provide high-quality, physics-based skeletal animation playback and control.

::: callout info
Please note that any use of types starting with `Spine` must conform to the [Spine Editor License Agreement](https://esotericsoftware.com/spine-editor-license).

Polly can't grant such a license to you, meaning that one must be acquired separately.
:::

<div class="centering-container">
    <video width="480" controls>
        <source src="/assets/videos/spine_anim.mp4" type="video/mp4">
    </video>
</div>

## Creating Spine Objects

The asset compiler is able to process Spine assets (`.skel`, `.json`, `.atlas`), so simply drop them into your `assets` folder and they're
ready to be loaded.

There are three parts to playing a Spine skeletal animation:

1. The atlas (`SpineAtlas`)
2. The skeleton data (`SpineSkeletonData`)
3. The skeleton instance (`SpineSkeleton`)

The first part is a `SpineAtlas`, which contains a skeleton's texture atlas data. This is produced by the Spine editor.

The second part is a `SpineSkeletonData`, which links skeletal data to a skeleton's texture atlas. This is also produced by the Spine editor.

The last part is the `SpineSkeleton`, which is an instance of a `SpineSkeletonData`.

Only `SpineAtlas` and `SpineSkeletonData` are **assets**.

In contrast, a `SpineSkeleton` is created **in memory**.

Let's load an atlas and a skeleton data based on that atlas:

```cpp
auto atlas        = SpineAtlas("anim/cloud-pot.atlas");
auto skeletonData = SpineSkeletonData("anim/cloud-pot.json", atlas);
```

We can then create a skeleton instance based on this:

```cpp
auto skeleton = SpineSkeleton(skeletonData);
```

::: callout info
It's safe to drop references to `SpineAtlas` and `SpineSkeletonData` after they're loaded, because a `SpineSkeletonData` will hold a reference to a `SpineAtlas`.

So as long as a `SkeletonData` lives, its atlas also lives. In turn, a `SpineSkeleton` will hold a reference to a `SkeletonData`.

It's therefore enough to only store a `SpineSkeleton` in your game instead of bookkeeping `SpineAtlas` and `SpineSkeletonData` objects.
:::

## Playing an Animation

Now that the skeleton is set up, it's time to play an animation!

For this, we have to create a `SpineAnimationStateData` based on the skeleton data, and then a `SpineAnimationState`, which is an instance of it:

```cpp
auto animationStateData = SpineAnimationStateData(skeletonData);

// Set the default cross-mixing duration between animations to 0.1 seconds.
animationStateData.setDefaultMix(0.1f);

auto animationState = SpineAnimationState(animationStateData);
```

We can now assign this animation state to the skeleton instance and play an animation:

```cpp
// Hook the animation state to the skeleton.
skeleton.setAnimationState(animationState);

// Start playing the animation "playing-in-the-rain" of the skeleton on track 0.
animationState.setAnimation(0, "playing-in-the-rain", /*shouldLoop:*/ true)
```

## Drawing a Spine Skeleton

Spine skeletons assume a coordinate system where the positive Y-axis points upwards. However, the default 2D coordinate system in Polly are image coordinates where the positive Y-axis points downwards.

Luckily, Polly provides a `Camera` class that allows us to transform this coordinate system.

Let's create a camera that looks at the skeleton instance and draw it:

```cpp
auto camera = Camera {
    .position = skeleton.bounds().center(),
};

void draw(Painter painter) override
{
    // Apply the camera's transformation to all subsequent drawings.
    gfx.setTransformation(camera.transformation());

    // Draw the skeleton instance.
    gfx.drawSpineSkeleton(skeleton);
}
```

## Drawing Extras

To draw some extra information about the skeleton such as bones and bounding box (as seen in the video above), we simply utilize Polly's vector graphics API:

```cpp
if (shouldDrawExtras)
{
    for (const auto& bone : skeleton.bones())
    {
        painter.drawEllipse(
            /*position: */    bone.worldPosition(),
            /*radius: */      Vec2(12, 12),
            /*color: */       lime.withAlpha(0.5f),
            /*strokeWidth: */ 3.0f);
    }

    painter.drawRectangle(skeleton.bounds(), red, 5.0f);
}
```

## Putting it all together

```cpp
auto atlas              = SpineAtlas("anim/cloud-pot.atlas");
auto skeletonData       = SpineSkeletonData("anim/cloud-pot.json", atlas);
auto skeleton           = SpineSkeleton(skeletonData);
auto animationStateData = SpineAnimationStateData(skeletonData);

// Set the default cross-mixing duration between animations to 0.1 seconds.
animationStateData.setDefaultMix(0.1f);

auto animationState = SpineAnimationState(animationStateData);

// Hook the animation state to the skeleton.
skeleton.setAnimationState(animationState);

// Start playing the animation "playing-in-the-rain" of the skeleton on track 0.
animationState.setAnimation(0, "playing-in-the-rain", /*shouldLoop:*/ true)

auto camera = Camera {
    .position = skeleton.bounds().center(),
};

void draw(Painter painter) override
{
    // Apply the camera's transformation to all subsequent drawings.
    painter.setTransformation(camera.transformation());

    // Draw the skeleton instance.
    painter.drawSpineSkeleton(skeleton);

    if (shouldDrawExtras)
    {
        for (const auto& bone : skeleton.bones())
        {
            painter.drawEllipse(bone.worldPosition(), Vec2(12, 12), lime.withAlpha(0.5f), 3.0f);
        }

        painter.drawRectangle(skeleton.bounds(), red, 5.0f);
    }
}
```

## More

The Spine API of Polly closely mirrors the official Spine C++ API, you can learn more about its usage
from the [official documentation](https://en.esotericsoftware.com/spine-cpp#Skeletons).

