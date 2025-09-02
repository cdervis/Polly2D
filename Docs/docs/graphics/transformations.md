# Transformations

Sometimes it's necessary to transform a specific group of 2D objects (or even all of them) without manually modifying their positions, rotations and sizes.

A transformation matrix allows you to do just that. It encompasses a certain order of transformations that are applied  to your objects relative to their default transformation.

Imagine drawing a 2D scene and wanting to scale the entirety of it by a specific factor, e.g. 1.5.
You could draw all objects of that scene by offset their positions and scaling their sizes accordingly, for each `drawSprite()` call.

Or you could construct a matrix using `scale(Vec2(1.5f))` and apply that to all of them.

You can apply a transformation using `Painter::setTransformation()`:

```cpp
painter.setTransformation(scale(Vec2(1.5f)));

painter.drawSprite(myImage, Vce2(200, 200));
painter.drawSprite(myImage, Vce2(400, 300));
painter.drawSprite(myImage, Vce2(600, 400));
```

This will scale everything by a factor of 1.5 across both the X and Y axis.

As an example, `Vec2(-1.5f, 2.0f)` will mirror the objects along the Y-axis, and also scale them by a factor of 2.0 along the Y-axis.
In other words, negative factors allow mirroring effects.

Another example would be if you wanted to first rotate the objects, then scale them, and finally offset their positions by a specific amount.
This is done by multiplying such matrices in a specific order:

```cpp
// Rotation by 45 degrees.
auto rotation    = rotate(Radians(45.0_deg));

// Scale by 1.5 along x-axis, and 3.5 along y-axis.
auto scale       = scale(Vec2(1.5f, 3.5f));

// Move by 100 along x-axis, and -100 along y-axis.
auto translation = translate(Vec2(100, -100));

// Combine all transformations into one.
auto transform   = rotation * scale * translation;

// Apply to all subsequent drawings.
painter.setTransformation(transform);
```

Transformation matrices are often used to implement 2D cameras, since they typically require movement (`translate()`) as well as a zoom `scale()`).
A camera's transformation is therefore a combination of both matrices.

Polly provides a `Camera` class that does just that!

As with all states, the transformation is remembered by the graphics device until it's changed again.
To restore the default transformation, which is an identity matrix, simply set a default-constructed matrix:

```cpp
painter.setTransformation(Matrix());
```

If you wish to set a transformation temporarily and restore the previously set transformation afterwards,
you can obtain the active transformation before setting yours.

```cpp title="Temporary transformations"
const auto previousTransformation = painter.transformation();

painter.setTransformation(myTransformation);

// Draw your objects
// ...

// Restore the previous transformation
painter.setTransformation(previousTransformation);
```
