// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4245)
#pragma warning(disable : 4456)
#pragma warning(disable : 4457)
#pragma warning(disable : 4701)
#endif

#if defined(__GNUC__) or defined(__clang__)
#pragma GCC diagnostic push
#endif

#if defined(__GNUC__) and not defined(__clang__)
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wlanguage-extension-token"
#pragma GCC diagnostic ignored "-Wtautological-compare"
#endif

#include "stb_vorbis.h"

#if defined(__GNUC__) or defined(__clang__)
#pragma GCC diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif
