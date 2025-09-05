// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/ContentManagement/ContentManager.hpp"

#include "Polly/Audio/SoundImpl.hpp"
#include "Polly/AudioDevice.hpp"
#include "Polly/BinaryReader.hpp"
#include "Polly/Details/ContentManagement.hpp"
#include "Polly/Font.hpp"
#include "Polly/Game/GameImpl.hpp"
#include "Polly/Graphics/FontImpl.hpp"
#include "Polly/Graphics/ImageImpl.hpp"
#include "Polly/Graphics/PainterImpl.hpp"
#include "Polly/Graphics/ShaderImpl.hpp"
#include "Polly/Image.hpp"
#include "Polly/Logging.hpp"
#include "Polly/Narrow.hpp"
#include "Polly/Shader.hpp"
#include "Polly/Sound.hpp"
#include "Polly/Spine/SpineImpl.hpp"
#include <algorithm>

#ifdef polly_platform_windows
#include <Windows.h>
#endif

spine::SpineExtension* spine::getDefaultExtension()
{
    return new DefaultSpineExtension();
}

namespace Polly
{
static String getAssetTypeName(const ContentManager::ReferenceToLoadedAsset& asset)
{
    switch (asset.kind)
    {
        case ContentManager::AssetKind::Image: {
            const auto* image = asset.u.image;
            return formatString("Image @ {}x{}", image->width(), image->height());
        }
        case ContentManager::AssetKind::Sound: return "Sound";
        case ContentManager::AssetKind::Shader: return "Shader";
        case ContentManager::AssetKind::Font: return "Font";
        case ContentManager::AssetKind::SpineAtlas: return "SpineAtlas";
        case ContentManager::AssetKind::SpineSkeletonData: return "SpineSkeletonData";
    }

    throw Error("Unknown asset type specified.");
}

static bool isAssetReferenceEqual(
    const Asset&                                  asset,
    const ContentManager::ReferenceToLoadedAsset& loadedAsset)
{
    switch (loadedAsset.kind)
    {
        case ContentManager::AssetKind::Image: return &asset == loadedAsset.u.image;
        case ContentManager::AssetKind::Sound:
            return loadedAsset.u.sound != nullptr ? loadedAsset.u.sound == &asset : false;
        case ContentManager::AssetKind::Shader: return &asset == loadedAsset.u.shader;
        case ContentManager::AssetKind::Font: return &asset == loadedAsset.u.font;
        case ContentManager::AssetKind::SpineAtlas: return &asset == loadedAsset.u.spineAtlas;
        case ContentManager::AssetKind::SpineSkeletonData: return &asset == loadedAsset.u.spineSkeletonData;
    }

    return false;
}

static void verifyAssetType(
    StringView name,
    char       storedAssetTypeId,
    char       expectedAssetTypeId,
    StringView assetTypeName)
{
    if (storedAssetTypeId != expectedAssetTypeId)
    {
        throw Error(formatString("Attempting to load asset '{}' as {}.", name, assetTypeName));
    }
}

ContentManager::ContentManager()
    : _archive("data.pla")
{
}

ContentManager::~ContentManager() noexcept
{
    logDebug("Destroying ContentManager");

    for (auto& [name, asset] : _loadedAssets)
    {
        // Prevent the asset from calling ContentManager::NotifyAssetDestroyed()
        // when its destroyed later. Because by then, the ContentManager is gone.

        // NOTE: This is ugly, but necessary until pattern matching is available.
        // std::visit is not noexcept and therefore not allowed in this destructor.

        switch (asset.kind)
        {
            case AssetKind::Image: asset.u.image->detachFromContentManager(); break;
            case AssetKind::Sound:
                if (asset.u.sound)
                {
                    asset.u.sound->detachFromContentManager();
                }
                break;
            case AssetKind::Shader: asset.u.shader->detachFromContentManager(); break;
            case AssetKind::Font: asset.u.font->detachFromContentManager(); break;
            case AssetKind::SpineAtlas: asset.u.spineAtlas->detachFromContentManager(); break;
            case AssetKind::SpineSkeletonData: asset.u.spineSkeletonData->detachFromContentManager(); break;
        }
    }
}

Image ContentManager::loadImage(StringView name)
{
    return lazyLoad<Image, Image::Impl, AssetKind::Image>(
        name,
        name,
        [](ReferenceToLoadedAsset& asset) -> Image::Impl*& { return asset.u.image; },
        [this](StringView assetName)
        {
            const auto [type, unpacked_data] = _archive.unpackAsset(assetName);
            verifyAssetType(assetName, type, 'i', "an image");

            auto& painterImpl = *Game::Impl::instance().painter().impl();

            auto img = _imageIO.loadImageFromMemory(painterImpl, unpacked_data);
            img->setAssetName(assetName);
            img->setDebuggingLabel(assetName);

            return Image(img.release());
        });
}

Shader ContentManager::loadShader(StringView name)
{
    return lazyLoad<Shader, Shader::Impl, AssetKind::Shader>(
        name,
        name,
        [](ReferenceToLoadedAsset& asset) -> Shader::Impl*& { return asset.u.shader; },
        [this](StringView assetName)
        {
            const auto [type, unpacked_data] = _archive.unpackAsset(assetName);
            auto reader                      = BinaryReader(unpacked_data, Details::assetDecryptionKey);

            verifyAssetType(assetName, type, 's', "a shader");

            auto& painterImpl = *Game::Impl::instance().painter().impl();

            const auto sourceCode = reader.readEncryptedString();

            auto shaderImpl = painterImpl.createUserShader(sourceCode, assetName);
            shaderImpl->setAssetName(assetName);

            auto shader = Shader(shaderImpl.release());
            shader.setDebuggingLabel(assetName);

            return shader;
        });
}

Font ContentManager::loadFont(StringView name)
{
    return lazyLoad<Font, Font::Impl, AssetKind::Font>(
        name,
        name,
        [](ReferenceToLoadedAsset& asset) -> Font::Impl*& { return asset.u.font; },
        [this](StringView assetName)
        {
            auto [type, unpacked_data] = _archive.unpackAsset(assetName);

            verifyAssetType(assetName, type, 'f', "a font");

            auto fontImpl = makeUnique<Font::Impl>(std::move(unpacked_data));
            fontImpl->setAssetName(assetName);

            return Font(fontImpl.release());
        });
}

Sound ContentManager::loadSound(StringView name)
{
    return lazyLoad<Sound, Sound::Impl, AssetKind::Sound>(
        name,
        name,
        [](ReferenceToLoadedAsset& asset) -> Sound::Impl*& { return asset.u.sound; },
        [this](StringView assetName)
        {
            auto& audioDevice = Game::Impl::instance().audioDevice();

            if (audioDevice.isNullDevice())
            {
                return Sound(makeUnique<Sound::Impl>(*audioDevice.impl(), none).release());
            }

            auto [type, unpacked_data] = _archive.unpackAsset(assetName);
            verifyAssetType(assetName, type, 'a', "a sound");

            return Sound(makeUnique<Sound::Impl>(*audioDevice.impl(), std::move(unpacked_data)).release());
        });
}

List<u8> ContentManager::loadAssetData(StringView name)
{
    const auto _ = std::lock_guard(_mutex);
    return _archive.unpackAsset(name).data;
}

void ContentManager::notifyAssetDestroyed(const Asset* asset)
{
    for (auto idx = 0u; const auto& [key, refToLoadedAsset] : _loadedAssets)
    {
        if (isAssetReferenceEqual(*asset, refToLoadedAsset))
        {
            logDebug("Unloading asset '{}' [{}]", key, getAssetTypeName(refToLoadedAsset));
            _loadedAssets.removeAt(idx);
            break;
        }

        ++idx;
    }
}

SpineAtlas ContentManager::loadSpineAtlas(StringView name)
{
    return lazyLoad<SpineAtlas, SpineAtlas::Impl, AssetKind::SpineAtlas>(
        name,
        name,
        [](ReferenceToLoadedAsset& asset) -> SpineAtlas::Impl*& { return asset.u.spineAtlas; },
        [this](StringView assetName)
        {
            const auto [type, unpacked_data] = _archive.unpackAsset(assetName);
            auto reader                      = BinaryReader(unpacked_data, Details::assetDecryptionKey);

            verifyAssetType(assetName, type, 'y', "a Spine atlas");

            auto atlasImpl = makeUnique<SpineAtlas::Impl>(reader.currentDataSpan(), assetName);
            atlasImpl->setAssetName(assetName);

            return SpineAtlas(atlasImpl.release());
        });
}

SpineSkeletonData ContentManager::loadSpineSkeletonData(StringView name, SpineAtlas atlas, float scale)
{
    const auto atlasName       = atlas.assetName();
    auto       scaleStr        = String();
    const auto isUnnormalScale = scale != 1.0f;
    auto       keyLength       = name.size() + atlasName.size() + 1;

    if (isUnnormalScale)
    {
        scaleStr = toString(scale);
        keyLength += 1;
        keyLength += scaleStr.size();
    }

    auto key = String();
    key.reserve(keyLength);
    key += name;
    key += "::";
    key += atlasName;

    if (isUnnormalScale)
    {
        key += "::";
        key += scaleStr;
    }

    return lazyLoad<SpineSkeletonData, SpineSkeletonData::Impl, AssetKind::SpineSkeletonData>(
        name,
        key,
        [](ReferenceToLoadedAsset& asset) -> SpineSkeletonData::Impl*& { return asset.u.spineSkeletonData; },
        [this, &atlas, scale, &key](StringView assetName)
        {
            const auto [type, unpacked_data] = _archive.unpackAsset(assetName);
            auto reader                      = BinaryReader(unpacked_data, Details::assetDecryptionKey);

            verifyAssetType(assetName, type, 'x', "a Spine skeleton");

            const auto isJson = reader.readUInt8() == 1;

            auto impl = makeUnique<SpineSkeletonData::Impl>(
                std::move(atlas),
                scale,
                reader.currentDataSpan(),
                isJson);

            impl->setAssetName(key);

            return SpineSkeletonData(impl.release());
        });
}
} // namespace Polly
