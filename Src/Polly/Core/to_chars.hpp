#ifndef SIMDJSON_SRC_TO_CHARS_CPP
#define SIMDJSON_SRC_TO_CHARS_CPP

namespace simdjson::internal
{
/*!
The format of the resulting decimal representation is similar to printf's %g
format. Returns an iterator pointing past-the-end of the decimal representation.
@note The input number must be finite, i.e. NaN's and Inf's are not supported.
@note The buffer must be large enough.
@note The result is NOT null-terminated.
*/
char* to_chars(char* first, const char* last, double value, bool isInt);
} // namespace simdjson::internal

#endif // SIMDJSON_SRC_TO_CHARS_CPP