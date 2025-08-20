// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/ContentManagement/Asset.hpp"
#include "Polly/Core/Object.hpp"
#include "Polly/GraphicsDevice.hpp"
#include "Polly/String.hpp"

namespace Polly
{
enum class GraphicsResourceType
{
    Image  = 1,
    Shader = 2,
};

class GraphicsResource : public Object,
                         public Asset
{
  protected:
    explicit GraphicsResource(GraphicsDevice::Impl& parentDevice, GraphicsResourceType type);

  public:
    deleteCopyAndMove(GraphicsResource);

    ~GraphicsResource() noexcept override;

    GraphicsDevice::Impl& parentDevice();

    GraphicsResourceType type() const;

    StringView debuggingLabel() const;

    virtual void setDebuggingLabel(StringView name);

    [[nodiscard]]
    String displayString() const;

  private:
    GraphicsDevice::Impl& _parentDevice;
    GraphicsResourceType  _resourceType;
    String                _label;
};
} // namespace Polly
