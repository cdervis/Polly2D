// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/ImageImpl.hpp"

#include "Polly/FileSystem.hpp"
#include "Polly/Game/GameImpl.hpp"
#include "Polly/Narrow.hpp"
#include "Polly/Spine/SpineImpl.hpp"

namespace Polly
{
SpineAnimationStateData::Impl::Impl(SpineSkeletonData skeleton_data)
    : skeleton_data(skeleton_data)
    , data(makeUnique<spine::AnimationStateData>(skeleton_data.impl()->skeleton_data.get()))
{
}

SpineAnimationState::Impl::Impl(SpineAnimationStateData data)
    : data(data)
    , state(makeUnique<spine::AnimationState>(data.impl()->data.get()))
{
}

void SpineAnimationState::Impl::update(float dt)
{
    state->update(dt);
}

void SpineAnimationState::Impl::apply_to(SpineSkeleton& skeleton)
{
    state->apply(*skeleton.impl()->skeleton);
}

SpineSkeleton::Impl::Impl(SpineSkeletonData skeleton_data)
    : skeleton_data(skeleton_data)
{
    auto* internal_skeleton_data = skeleton_data.impl()->skeleton_data.get();

    skeleton = makeUnique<spine::Skeleton>(internal_skeleton_data);

    // The containers as of Spine 4.2 do not support range-for yet.
    // Therefore just use a lambda to avoid boilerplate.
#if 0
    bones.addRange(skeleton->getBones());
    slots.addRange(skeleton->getSlots());
    ik_constraints.addRange(skeleton->getIkConstraints());
    transform_constraints.addRange(skeleton->getTransformConstraints());
    path_constraints.addRange(skeleton->getPathConstraints());
    physics_constraints.addRange(skeleton->getPhysicsConstraints());
#else
    const auto add_spine_elements = [](auto& dst, auto& src)
    {
        const auto size = static_cast<u32>(src.size());
        dst.reserve(size);

        for (u32 i = 0; i < size; ++i)
        {
            dst.emplace(src[i]);
        }
    };

    add_spine_elements(bones, skeleton->getBones());
    add_spine_elements(slots, skeleton->getSlots());
    add_spine_elements(ik_constraints, skeleton->getIkConstraints());
    add_spine_elements(transform_constraints, skeleton->getTransformConstraints());
    add_spine_elements(physics_constraints, skeleton->getPhysicsConstraints());
    add_spine_elements(path_constraints, skeleton->getPathConstraints());
    add_spine_elements(physics_constraints, skeleton->getPhysicsConstraints());
#endif
}

void SpineSkeleton::Impl::update(float dt)
{
    skeleton->update(dt);
}

void SpineSkeleton::Impl::update_world_transform(SpineUpdatePhysics physics)
{
    skeleton->updateWorldTransform(static_cast<spine::Physics>(physics));
}

SpineSkeletonData::Impl::Impl(SpineAtlas atlas, float scale, Span<u8> data, bool is_json)
    : atlas(atlas)
{
    if (is_json)
    {
        const auto json_source_str = String(reinterpret_cast<const char*>(data.data()), data.size());

        auto json = spine::SkeletonJson(atlas.impl()->atlas.get());
        json.setScale(scale);

        skeleton_data.reset(json.readSkeletonData(json_source_str.cstring()));

        if (not skeleton_data)
        {
            const auto& error_str = json.getError();

            throw Error(formatString(
                "Failed to load skeleton data. Reason: {}",
                StringView(error_str.buffer(), static_cast<u32>(error_str.length()))));
        }
    }
    else
    {
        auto binary = spine::SkeletonBinary(atlas.impl()->atlas.get());
        binary.setScale(scale);

        skeleton_data.reset(binary.readSkeletonData(data.data(), static_cast<int>(data.size())));

        if (not skeleton_data)
        {
            const auto& error_str = binary.getError();

            throw Error(formatString(
                "Failed to load skeleton data. Reason: {}",
                StringView(error_str.buffer(), static_cast<u32>(error_str.length()))));
        }
    }

    // Same as in SpineSkeleton::Impl::Impl().
#if 0
    bones.addRange(skeleton_data->getBones());
    slots.addRange(skeleton_data->getSlots());
    skins.addRange(skeleton_data->getSkins());
    events.addRange(skeleton_data->getEvents());
    animations.addRange(skeleton_data->getAnimations());
    ik_constraints.addRange(skeleton_data->getIkConstraints());
    transform_constraints.addRange(skeleton_data->getTransformConstraints());
    path_constraints.addRange(skeleton_data->getPathConstraints());
    physics_constraints.addRange(skeleton_data->getPhysicsConstraints());
#else
    const auto add_spine_elements = [](auto& dst, auto& src)
    {
        const auto size = static_cast<u32>(src.size());
        dst.reserve(size);

        for (u32 i = 0; i < size; ++i)
        {
            dst.emplace(src[i]);
        }
    };

    add_spine_elements(bones, skeleton_data->getBones());
    add_spine_elements(slots, skeleton_data->getSlots());
    add_spine_elements(skins, skeleton_data->getSkins());
    add_spine_elements(events, skeleton_data->getEvents());
    add_spine_elements(animations, skeleton_data->getAnimations());
    add_spine_elements(ik_constraints, skeleton_data->getIkConstraints());
    add_spine_elements(transform_constraints, skeleton_data->getTransformConstraints());
    add_spine_elements(physics_constraints, skeleton_data->getPhysicsConstraints());
#endif

    animation_names.reserve(animations.size());
    animation_names_views.reserve(animations.size());

    for (const auto& anim : animations)
    {
        animation_names.emplace(anim.name());
        animation_names_views.emplace(anim.name());
    }
}

SpineAtlas::Impl::TextureLoader::TextureLoader(StringView atlas_asset_name_hint)
    : _atlas_asset_name_hint(atlas_asset_name_hint)
{
}

void SpineAtlas::Impl::TextureLoader::load(spine::AtlasPage& page, const spine::String& path)
{
    const auto full_path = formatString(
        "{}/{}",
        FileSystem::pathParent(_atlas_asset_name_hint),
        StringView(path.buffer(), narrow<int>(path.length())));

    auto maybe_img =
        findWhere(_images, [&full_path](const Image& e) { return e.impl()->assetName() == full_path; });

    if (not maybe_img)
    {
        maybe_img = _images.emplace(full_path);
        maybe_img->impl()->setAssetName(full_path);
    }

    page.texture = maybe_img->impl();
}

void SpineAtlas::Impl::TextureLoader::unload(void* texture)
{
    // Nothing to do here.
    // The images are owned by the SpineAtlas.
}

SpineAtlas::Impl::Impl(Span<u8> data, StringView asset_name_hint)
    : texture_loader(asset_name_hint)
{
    atlas = makeUnique<spine::Atlas>(
        /*data=*/reinterpret_cast<const char*>(data.data()),
        /*length=*/narrow<int>(data.size()),
        /*dir=*/"",
        /*textureLoader=*/&texture_loader);
}
} // namespace pl
