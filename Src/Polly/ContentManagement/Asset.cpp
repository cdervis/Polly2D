// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/ContentManagement/Asset.hpp"

#include "Polly/ContentManagement/ContentManager.hpp"

namespace Polly
{
Asset::Asset()
    : _contentManager(nullptr)
{
}

Asset::~Asset() noexcept
{
    if (_contentManager)
    {
        assume(not _assetName.isEmpty());
        _contentManager->notifyAssetDestroyed(this);
    }
}

StringView Asset::assetName() const
{
    return _assetName;
}

void Asset::setAssetName(StringView value)
{
    _assetName = value;
}

void Asset::setAssetNameStr(String value)
{
    _assetName = std::move(value);
}

void Asset::attachToContentManager(ContentManager* manager)
{
    assume(manager != nullptr);
    _contentManager = manager;
}

void Asset::detachFromContentManager()
{
    _contentManager = nullptr;
}
} // namespace Polly
