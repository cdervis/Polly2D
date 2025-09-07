# Shading Language

Polly uses its own platform-agnostic shading language.

It includes a hand-written, small and efficient compiler that translates its shaders to native shading languages such as GLSL, HLSL and MSL.

The language is designed as a C-like language with simple constructs. Its goal is to provide an easy to understand shading language tailored to Polly's domain, namely sprite shading.

The advantage of having a custom shading language is the ability to closely match it to what the library is capable of. Conversely, the library can optimize how shader data is stored and sent to the GPU, because it understands the language's behavior and restrictions.

Users of GLSL, HLSL or MSL should feel familiar with the language.

## Basic Syntax

Similar to C++, a function is defined in the form of:

```cpp
Vec3 multiplyAdd(Vec3 first, Vec3 second, Vec3 third)
{
    return first * second + third;
}
```

Some rules for functions:

- Every function must return a value; there is no void type.
  - A function is allowed at most one return statement, which must be the last statement in its body.
- Function parameters are immutable.
- Function overloading is not supported.
- Every code block must be surrounded by `{` and `}`, even if it contains a single statement.

## Comments

Comments start with `//`. Multiline comments in the form of `/* ... */` are not supported.

## Local Variables

To be consistent with C++, local variables are declared using the `auto` keyword. The type of the variable is deduced from its initially assigned value.

```cpp
auto a = 1;
a = a * 2;
a += 2;
```

Because shaders are mostly simple and isolated functions, all variables are mutable and can't be made immutable, which means there is no `const` equivalent.

::: callout info
The prefix `pl_` is a reserved prefix for built-in variables and may not be used as a prefix for variable and function names.
:::

## If Statements

Use `if` statements to conditionally execute a portion of code at runtime:

```cpp
Vec3 someConditions(Vec3 v)
{
    auto result = v;
    auto len    = length(v);
  
    if (len > 4.0)
    {
        result *= 10.0;
    }
    else if (len > 2.0)
    {
        result *= 5.0;
    }
    else
    {
        result = Vec3(0);
    }
  
    return result;
}
```

Ternary conditional operators are also supported:

```cpp
float myMaxFunc(float a, float b)
{
  return a > b ? a : b;
}
```

## Loops

Loops can be realized using a `for` statement. A for loop requires a name for the iterator variable and a range in the form of `<start> .. <endExclusive>`:

```cpp
auto sum = 0;

for i in 1 .. 4    // i will be 1, 2, 3
{ 
  sum += i;
}

// sum: 1 + 2 + 3 = 6
```

**Note** that the iterator variable (in this case `i`) is immutable.

Just like in C++, you can use the `continue` keyword to skip a loop iteration, and `break` to stop the loop altogether.

## The Main Function

The `main()` function is the entry point in a shader.

```cpp
Vec4 main()
{
  return Vec4(0);
}
```

`main()` must not have any parameters and must always return a `Vec4` value.

## Shader Parameters

A shader can declare parameters that are accessible globally, for example:

```cpp
Vec3 someColor;
float intensity = 1.0; // Assigning a default value

Vec3 someFunction(Vec3 value)
{
    return value + someColor;
}

Vec4 main()
{
    return someFunction(Vec3(1, 2, 3), intensity);
}
```

Parameter declarations may optionally assign a default value. If no default value is specified for a parameter, it receives a zero-value.
Meaning that a `float` will be `0.0`, a `Vec2` will be `Vec2(0, 0)`, a `Matrix` will be all zeroes, etc.

Supported parameter types are:

- `bool`
- `int`, `uint`, `float`
- `Vec2`, `Vec3`, `Vec4`
- `Matrix`

::: callout info
As of the current version, `Image` parameters are not supported yet. This feature is in development.
:::

## Parameter Values

To set parameters on shader objects, call the `Shader::set()` method:

```cpp
myShader.set("baseColor", Vec3(1, 0, 0));
myShader.set("intensity", 2.0f);
```

The method has overloads for each parameter type.
When attempting to set a value that is incompatible with the parameter type, an `Error` is thrown.

Conversely, shader parameter values can be obtained using the `Shader::...Value()` methods:

```cpp
auto baseColor = myShader.vec3Value("baseColor");  // Maybe<Vec3>
auto intensity = myShader.floatValue("intensity"); // Maybe<Float>
```

The value of a parameter is always part of a shader object's state.
This means that shader parameters can be updated even when a shader isn't actively used.

## Array Parameters

It's possible to declare array parameters for scalar types using an array specifier:

```cpp
// Arrays must always have a known size at compile time.
Vec3[12] someArrayOf3DVectors;

// Expressions may be used as an array size, but are required to be known at compile time.
const someValue = 4;
const someConstant = 12 * someValue;

float[someConstant + 2] someArrayOfFloats;
```

Array parameter values are also modified using `Shader::set()`. Arrays are specified as `Span` values:

```cpp
myShader.set("someFloats", Array{0.5f, 1.0f, 1.25f, 5.0f});
```

# Shading Language Reference

## Syntax

The following table describes the syntax of the shading language.

| Construct          | Form                                                  | Example                                |
|--------------------|-------------------------------------------------------|----------------------------------------|
| Function parameter | `<type> <name>`                                       | `int a`                                |
| Function signature | `<type> <name> '(' <parameter> (',' <parameter)* ')'` | `int add( int a, int b )`              |
| Function body      | `'{' stmt* return_stmt '}'`                           | `{ return a + b; }`                    |
| Function           | `<signature> <body>`                                  | `float pow( int x ) { return x * x; }` |
| Shader parameter   | `<type> <name>`                                       | `float someParameter`                  |
| Array type         | `<type>[<size>]`                                      | `Vec2[10]`                             |

## Types

| Type   | Description                  | C++ equivalent | Can be array |
|--------|------------------------------|----------------|--------------|
| bool   | Boolean true / false value	  | `i32`          | ✅            |
| int    | Signed 32-bit integer        | `i32`          | ✅            |
| uint   | Unsigned 32-bit integer      | `u32`          | ✅            |
| float  | 32-bit floating point number | `float`        | ✅            |
| Vec2   | 2D floating point vector     | `Vec2`         | ✅            |
| Vec3   | 3D floating point vector     | `Vec3`         | ✅            |
| Vec4   | 4D floating point vector     | `Vec4`         | ✅            |
| Matrix | 4×4 row-major matrix         | `Matrix`       | ✅            |
| Image  | 2D texture                   | `Image`        | ❌            |

## Vector Swizzling

All vector types provide fields according to their dimensions:

- `Vec2` provides `x` and `y`
- `Vec3` provides `x`, `y` and `z`
- `Vec4` provides `x`, `y`, `z` and `w`

In addition to these fields, vectors allows them to be swizzled, meaning that the access can be freely intermixed.
An example:

```cpp
auto v2 = Vec2(1, 2);
auto f1 = v2.xx;  // Same as Vec2(1, 1)
auto f2 = v2.yx;  // Same as Vec2(2, 1)

auto v3 = Vec3(1, 2, 3);
auto f3 = v3.xy;  // Same as Vec2(1, 2)
auto f4 = v3.yxz; // Same as Vec3(2, 1, 3)

// ...
```

## Built-in Variables

The following lists all variables that are always available within a shader.

### General

| Variable                | Description                                                                | Type   |
|-------------------------|----------------------------------------------------------------------------|--------|
| `pl_pixelPos`           | The position of the pixel, in pixel coordinates within the current canvas. | `Vec2` |
| `pl_pixelPosNormalized` | The position of the pixel, normalized to range `[0.0 .. 1.0]`.             | `Vec2` |
| `pl_viewportSize`       | The size of the current canvas, in pixels.                                 | `Vec2` |
| `pl_viewportSizeInv`    | The reciprocal size of the current canvas, in pixels.                      | `Vec2` |

### #type sprite

| Variable          | Description                                        | Type    |
|-------------------|----------------------------------------------------|---------|
| `pl_spriteImage`  | The image of the sprite that's drawn.              | `Image` |
| `pl_spriteColor`  | The color of the sprite that's drawn.              | `Vec4`  |
| `pl_spriteUV`     | The texture coordinate of the sprite that's drawn. | `Vec2`  |
| `pl_polygonColor` | The color of the polygon that's drawn.             | `Vec2`  |

### #type polygon

| Variable          | Description                            | Type   |
|-------------------|----------------------------------------|--------|
| `pl_polygonColor` | The color of the polygon that's drawn. | `Vec2` |

### #type mesh

| Variable       | Description                                        | Type    |
|----------------|----------------------------------------------------|---------|
| `pl_meshImage` | The image of the sprite that's drawn.              | `Image` |
| `pl_meshColor` | The color of the sprite that's drawn.              | `Vec4`  |
| `pl_meshUV`    | The texture coordinate of the sprite that's drawn. | `Vec2`  |

## Functions

The following lists all available intrinsic functions.

Within this table the following names are defined as groups of types:

- Vec: `Vec2`, `Vec3`, `Vec4`
- Fto4: `float`, `Vec2`, `Vec3`, `Vec4`
- FtoM: `float`, `Vec2`, `Vec3`, `Vec4`, `Matrix`

| Name       | Parameters → Return Type |
|------------|--------------------------|
| abs        | Fto4 → Fto4              |
| acos       | Fto4 → Fto4              |
| all        | FtoM → FtoM              |
| any        | FtoM → FtoM              |
| asin       | Fto4 → Fto4              |
| atan       | Fto4 → Fto4              |
| atan2      | Fto4 → Fto4              |
| ceil       | FtoM → FtoM              |
| clamp      | Fto4 → Fto4              |
| cos        | Fto4 → Fto4              |
| degrees    | Fto4 → Fto4              |
| distance   | Fto4 → Vec               |
| dot        | Vec → Vec                |
| exp        | Vec → Fto4               |
| exp2       | Fto4 → Fto4              |
| floor      | Fto4 → Fto4              |
| fmod       | Fto4 → Fto4              |
| frac       | Fto4 → Fto4              |
| length     | Vec → Vec                |
| lerp       | Fto4 → Fto4              |
| log        | Fto4 → Fto4              |
| log2       | Fto4 → Fto4              |
| max        | Fto4 → Fto4              |
| min        | Fto4 → Fto4              |
| normalize  | Vec → Vec                |
| pow        | Fto4 → Fto4              |
| radians    | Fto4 → Fto4              |
| round      | Fto4 → Fto4              |
| sample     | (Image, Vec2) → Vec4     |
| saturate   | Fto4 → Fto4              |
| sign       | Fto4 → Fto4              |
| sin        | Fto4 → Fto4              |
| smoothstep | Fto4 → Fto4              |
| sqrt       | Fto4 → Fto4              |
| tan        | Fto4 → Fto4              |
| transpose  | Matrix → Matrix          |
| trunc      | Fto4 → Fto4              |

## Constructors

The following lists all available type constructors.

| Type    | Parameters                         | Effect                         |
|---------|------------------------------------|--------------------------------|
| `float` | int x                              | Cast x to float                |
| `float` | uint x                             | Cast x to float                |
| `int`   | float x                            | Cast x to int                  |
| `int`   | uint x                             | Cast x to int                  |
| `uint`  | int x                              | Cast x to uint                 |
| `uint`  | float x                            | Cast x to uint                 |
| `Vec2`  | float x, float y                   | x=x, y=y                       |
| `Vec2`  | float xy                           | x=xy, y=xy                     |
| `Vec3`  | float x, float y, float z          | x=x, y=y, z=z                  |
| `Vec3`  | float xyz                          | x=xyz, y=xyz, z=xyz            |
| `Vec4`  | float x, float y, float z, float w | x=x, y=y, z=z, w=w             |
| `Vec4`  | Vec2 xy, Vec2 zw                   | x=xy.x, y=xy.y, z=zw.x, w=zw.y |
| `Vec4`  | Vec2 xy, float z, float w          | x=xy.x, y=xy.y, z=z, w=w       |
| `Vec4`  | Vec3 xyz, float w                  | x=xyz.x, y=xyz.y, z=xyz.z, w=w |
| `Vec4`  | float xyzw                         | x=xyzw, y=xyzw, z=xyzw, w=xyzw |
