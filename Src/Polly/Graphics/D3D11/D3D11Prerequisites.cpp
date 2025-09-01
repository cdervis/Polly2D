#include "D3D11Prerequisites.hpp"

#include "Polly/Error.hpp"
#include "Polly/Format.hpp"
#include "Polly/Image.hpp"

namespace Polly
{
void checkHResult(HRESULT result, StringView message)
{
    if (FAILED(result))
    {
        throw Error(formatString("{} (error code {})", message, static_cast<i32>(result)));
    }
}

void setD3D11ObjectLabel([[maybe_unused]] ID3D11DeviceChild* object, [[maybe_unused]] StringView name)
{
#ifndef NDEBUG
    object->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.data());
#endif
}

Maybe<DXGI_FORMAT> convert(ImageFormat format)
{
    switch (format)
    {
        case ImageFormat::R8Unorm: return DXGI_FORMAT_R8_UNORM;
        case ImageFormat::R8G8B8A8UNorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
        case ImageFormat::R8G8B8A8Srgb: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case ImageFormat::R32G32B32A32Float: return DXGI_FORMAT_R32G32B32A32_FLOAT;
    }

    return none;
}
} // namespace Polly
