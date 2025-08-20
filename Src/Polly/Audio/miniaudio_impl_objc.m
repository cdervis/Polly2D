#ifdef __APPLE__
#define MA_NO_RUNTIME_LINKING
#endif

#if defined( __GNUC__ )
#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wc23-extensions"
#pragma GCC diagnostic ignored "-Wc2x-extensions"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_NULL
#define MA_NO_DECODING
#include "miniaudio.h"

#if defined( __GNUC__ )
#pragma GCC diagnostic pop
#endif
