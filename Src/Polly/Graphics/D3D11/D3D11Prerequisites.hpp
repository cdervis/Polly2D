// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Maybe.hpp"
#include <d3d11.h>
#include <d3d11_1.h>
#include <dxgi.h>
#include <Windows.h>
#include <WRL/client.h>

using Microsoft::WRL::ComPtr;

namespace Polly
{
enum class ImageFormat;

void checkHResult(HRESULT result, StringView message);

void setD3D11ObjectLabel(ID3D11DeviceChild* object, StringView name);

Maybe<DXGI_FORMAT> convert(ImageFormat format);
} // namespace Polly
