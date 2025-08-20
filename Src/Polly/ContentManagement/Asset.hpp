// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Maybe.hpp"
#include "Polly/String.hpp"

namespace Polly
{
class ContentManager;

class Asset
{
  protected:
    Asset();

  public:
    defaultCopyAndMove(Asset);

    virtual ~Asset() noexcept;

    StringView assetName() const;

    void setAssetName(StringView value);

    void setAssetNameStr(String value);

    void attachToContentManager(ContentManager* manager);

    void detachFromContentManager();

  private:
    ContentManager* _contentManager;
    String          _assetName;
};
} // namespace Polly
