// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "GraphicsResource.hpp"

#include "Polly/Graphics/ImageImpl.hpp"
#include "Polly/Graphics/PainterImpl.hpp"

namespace Polly
{
GraphicsResource::GraphicsResource(Painter::Impl& painter, GraphicsResourceType type)
    : _painter(painter)
    , _resourceType(type)
{
    _painter.notifyResourceCreated(*this);
}

GraphicsResource::~GraphicsResource() noexcept
{
    logVerbose("~GraphicsResourceImpl( {} )", displayString());

    _painter.notifyResourceDestroyed(*this);
}

StringView GraphicsResource::debuggingLabel() const
{
    return _label;
}

void GraphicsResource::setDebuggingLabel(StringView name)
{
    _label = name;
}

String GraphicsResource::displayString() const
{
    const auto typeName = [type = _resourceType, this]() -> String
    {
        switch (type)
        {
            case GraphicsResourceType::Image: {
                const auto* meAsImage = static_cast<const Image::Impl*>(this);

                return formatString("Image @ {}x{}", meAsImage->width(), meAsImage->height());
            }
            case GraphicsResourceType::Shader: return "Shader";
        }
        return "<unknown>";
    }();

    auto label = StringView(_label);

    if (_label.isEmpty())
    {
        if (const auto meAsAsset = dynamic_cast<const Asset*>(this))
        {
            label = meAsAsset->assetName();
        }
    }

    if (label.isEmpty())
    {
        label = "<unnamed>";
    }

    return formatString("{} [{}]", label, typeName);
}

Painter::Impl& GraphicsResource::painter()
{
    return _painter;
}

GraphicsResourceType GraphicsResource::type() const
{
    return _resourceType;
}
} // namespace Polly