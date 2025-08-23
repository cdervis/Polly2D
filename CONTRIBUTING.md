# Contributing

Contributions are always welcome, be it in the form of [feedback](mailto:cem@dervis.de), [bug reports](https://github.com/cdervis/Polly/issues) or code changes.

There are no special rules when it comes to discussions and requests, just be friendly.

To contribute code changes, simply fork the repository and open a pull request, i.e. the usual GitHub workflow.

## Building Polly from source

To build Polly and all of its demo applications from source, you'll need to install everything that's required
for Polly-based games, see [Getting started](https://github.com/cdervis/Polly?tab=readme-ov-file#getting-started).

To build Polly from the command line, use CMake:

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Debug
```

Then build it:

```sh
cmake --build build --parallel
```

All dependencies will be fetched and built automatically.

## Coding conventions

Polly uses C++20 (without modules, yet), with exceptions and RTTI **enabled**.

Exceptions should not be used for control flow, only for critical errors that occur rarely (e.g. out of memory).

All containers and similar types in Polly are hardened, meaning that they perform validation checks by default.
Such checks are out-of-bounds, nullptr-access and empty-optional checks.

These can be turned off by `#define`-ing `polly_no_hardening`.

For simple result checking, use Polly's `Maybe<T>` type.

Usage of standard library types is allowed, even for containers where Polly doesn't provide an alternative, for example `std::unordered_map`.

### Naming

Symbols are named as follows:
- Types: PascalCase
- Functions: camelCase
- Parameters: camelCase
- Variables: camelCase
- Public members: camelCase
- Private members: _camelCase
