# C++ API Design

This page explains how the Polly API is designed around C++ and some general advice on how to get the most out of it.

Polly's API is designed to be as simple to use as possible from the beginning, while providing enough flexibility to accomplish more advanced goals later on.

It follows a modern style of C++ with heavy emphasis on automatic ownership (using RAII) and value semantics.

Polly uses C++ exceptions and RTTI, which is also encouraged to be used by the game code itself. This way, error handling on both sides is consistent and aligns with Standard C++.

## Objects

Polly uses automatic reference counting for its object types. Object types are all types that have some sort of ownership over resources.

Typical reference-counted objects are `Image`, `Font`, `Shader`, but also others like `Game`, `Gamepad` and `Window`.

Objects can be freely copied and moved around. While a construction and / or copy will increase an object's reference count, its destruction will simply decrease it. When an object's reference count reaches zero (i.e. it's not referenced by any scope anymore), its underlying resources are destroyed with it.

Every reference-counted object is made up of exactly one pointer, which is a pointer to its internal implementation object that is stored in the system's memory heap.

This behavior matches that of [`std::shared_ptr`](https://en.cppreference.com/w/cpp/memory/shared_ptr).

Although it would be wise to use `std::move` and pass-by-reference to pass objects around, the reference counting mechanism in Polly is very efficient (and unlike `std::shared_ptr`, not an atomic operation).

You can therefore simply pass Polly objects around by value without incurring any significant overhead.

It's important to note that a default-constructed object is always invalid and can't be used. Calling such an object's methods will throw an appropriate exception.

Other types, such as scalar types like vectors, matrices and `GameTime` are **not reference-counted**; they're simply trivial types.

If you're unsure whether a type is reference-counted, look at its declaration in the C++ header. Reference-counted types are denoted by their usage of the `pl_object()` macro.

### Object Operators

All Polly objects support operators `==`, `!=`, `<`, `<=`, `>` and `>=`, which compare the implementation pointers between
two objects numerically.

This means that, for example, `obj1 == obj2` will compare whether two objects point to the same internal object. In other words, whether they share a reference to the same object.

An object can also be evaluated as a `bool`, where it returns true if it's a valid object.

```cpp
auto img = Image(); // Default-constructed image

if (img) // false

// ...

img = Image("ExistingImage.png");

if (img) // true
```


