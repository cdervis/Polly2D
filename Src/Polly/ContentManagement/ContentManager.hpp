// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Algorithm.hpp"
#include "Polly/ContentManagement/Archive.hpp"
#include "Polly/ContentManagement/ImageIO.hpp"
#include "Polly/CopyMoveMacros.hpp"
#include "Polly/Defer.hpp"
#include "Polly/Font.hpp"
#include "Polly/Graphics/ShaderImpl.hpp"
#include "Polly/Image.hpp"
#include "Polly/Logging.hpp"
#include "Polly/Pair.hpp"
#include "Polly/Shader.hpp"
#include "Polly/Sound.hpp"
#include "Polly/Spine.hpp"
#include "Polly/Spine/SpineImpl.hpp"
#include "Polly/String.hpp"
#include <mutex>

namespace Polly
{
class ContentManager final
{
  public:
    // The content manager doesn't manage loaded assets, only non-owning references are
    // stored. When assets are destroyed (because their external references have
    // vanished), they notify the content manager. The content manager then removes those
    // assets from its list.
    enum class AssetKind
    {
        Image,
        Sound,
        Shader,
        Font,
        SpineAtlas,
        SpineSkeletonData,
    };

    struct ReferenceToLoadedAsset
    {
        AssetKind kind;

        union
        {
            Image::Impl*             image;
            Sound::Impl*             sound;
            Shader::Impl*            shader;
            Font::Impl*              font;
            SpineAtlas::Impl*        spineAtlas;
            SpineSkeletonData::Impl* spineSkeletonData;
        } u;
    };

    explicit ContentManager();

    DeleteCopyAndMove(ContentManager);

    ~ContentManager() noexcept;

    Image loadImage(StringView name);

    Shader loadShader(StringView name);

    Font loadFont(StringView name);

    Sound loadSound(StringView name);

    List<u8> loadAssetData(StringView name);

    SpineAtlas loadSpineAtlas(StringView name);

    SpineSkeletonData loadSpineSkeletonData(StringView name, SpineAtlas atlas, float scale);

    void notifyAssetDestroyed(const Asset* asset);

  private:
    using MapOfLoadedAssets = List<Pair<String, ReferenceToLoadedAsset>>;

    template<typename TBase, typename TImpl, AssetKind Kind, typename TRefExtractorFunc, typename TLoadFunc>
    auto lazyLoad(
        StringView          name,
        StringView          key,
        TRefExtractorFunc&& refExtractorFunc,
        TLoadFunc&&         loadFunc);

    auto findAsset(StringView name)
    {
        return findWhere(_loadedAssets, [&name](const auto& pair) { return pair.first == name; });
    }

    std::mutex        _mutex;
    Archive           _archive;
    MapOfLoadedAssets _loadedAssets;
    ImageIO           _imageIO;
};

template<
    typename TBase,
    typename TImpl,
    ContentManager::AssetKind Kind,
    typename TRefExtractorFunc,
    typename TLoadFunc>
auto ContentManager::lazyLoad(
    StringView          name,
    StringView          key,
    TRefExtractorFunc&& refExtractorFunc,
    TLoadFunc&&         loadFunc)
{
    static_assert(std::is_base_of_v<Asset, TImpl>, "Type must derive from Asset");

    auto hasMutexBeenLocked = false;
    if (_mutex.try_lock())
    {
        hasMutexBeenLocked = true;
    }

    defer
    {
        if (hasMutexBeenLocked)
        {
            _mutex.unlock();
        }
    };

    const auto nameStr = String(name);
    auto       keyStr  = String(key);

    if (auto maybePair = findAsset(keyStr))
    {
        auto& ref = std::invoke(refExtractorFunc, maybePair->second);

        if (not ref)
        {
            throw Error(formatString(
                "Attempting to load asset '{}'. However, the "
                "asset was previously loaded as a different type.",
                name));
        }

        // Construct object, increment reference count to impl object.
        auto obj = TBase();
        setImpl(obj, ref);

        return obj;
    }

    // Load fresh object, store its impl pointer in the map, but return the object.
    auto asset = loadFunc(nameStr);

    if (not asset)
    {
        throw Error(formatString("Loaded asset '{}', but its creation failed.", name));
    }

    defer
    {
        logDebug("Loaded asset '{}'", nameStr);
    };

    auto* impl = asset.impl();
    impl->attachToContentManager(this);
    impl->setAssetName(nameStr);

    auto refToLoadedAsset = ReferenceToLoadedAsset{
        .kind = Kind,
        .u    = {},
    };

    refExtractorFunc(refToLoadedAsset) = static_cast<TImpl*>(impl);

    _loadedAssets.emplace(std::move(keyStr), refToLoadedAsset);

    return asset;
}
} // namespace Polly
