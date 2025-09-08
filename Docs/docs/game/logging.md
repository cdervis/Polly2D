# Logging

Polly provides a simple, unified API for logging messages to the platform's appropriate output.

On Windows with Visual Studio, this would be the Output Window. On Android, this would be Logcat. And on other platforms, it might be the standard output, i.e. terminal window.

Example:

```cpp
logInfo("Hello {}!", "World");          // Hello World!
logInfo("Numbers: {}", Array{1, 2, 3}); // Numbers: [1; 2; 3]
```

Message logging has different levels: information, warning, error, debug and verbose, with their respective functions:

- `logInfo()`
- `logWarning()`
- `logError()`
- `logDebug()`
- `logVerbose()`

All functions have the same signature; they take a format string and zero or more values that are to be formatted.


