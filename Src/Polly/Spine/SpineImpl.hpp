// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/ContentManagement/Asset.hpp"
#include "Polly/Core/Object.hpp"
#include "Polly/Image.hpp"
#include "Polly/Spine.hpp"
#include "Polly/UniquePtr.hpp"

#include <spine/spine.h>

namespace Polly
{
class SpineAtlas::Impl final : public Object,
                               public Asset
{
  public:
    explicit Impl(Span<u8> data, StringView asset_name_hint);

    DeleteCopyAndMove(Impl);

    class TextureLoader final : public spine::TextureLoader
    {
      public:
        explicit TextureLoader(StringView atlas_asset_name_hint);

        void load(spine::AtlasPage& page, const spine::String& path) override;

        void unload(void* texture) override;

        Span<Image> images() const
        {
            return _images;
        }

      private:
        String      _atlas_asset_name_hint;
        List<Image> _images;
    };

    TextureLoader           texture_loader;
    UniquePtr<spine::Atlas> atlas;
};

class SpineSkeletonData::Impl final : public Object,
                                      public Asset
{
  public:
    explicit Impl(SpineAtlas atlas, float scale, Span<u8> data, bool is_json);

    SpineAtlas                         atlas;
    UniquePtr<spine::SkeletonData>     skeleton_data;
    List<SpineBoneData>                bones;
    List<SpineSlotData>                slots;
    List<SpineSkin>                    skins;
    List<SpineEventData>               events;
    List<SpineAnimation>               animations;
    List<SpineIKConstraintData>        ik_constraints;
    List<SpineTransformConstraintData> transform_constraints;
    List<SpinePathConstraintData>      path_constraints;
    List<SpinePhysicsConstraintData>   physics_constraints;
    List<String>                       animation_names;
    List<StringView>                   animation_names_views;
};

class SpineAnimationStateData::Impl final : public Object
{
  public:
    explicit Impl(SpineSkeletonData skeleton_data);

    SpineSkeletonData                    skeleton_data;
    UniquePtr<spine::AnimationStateData> data;
};

class SpineAnimationState::Impl final : public Object
{
  public:
    explicit Impl(SpineAnimationStateData data);

    void update(float dt);

    void apply_to(SpineSkeleton& skeleton);

    SpineAnimationStateData          data;
    UniquePtr<spine::AnimationState> state;
};

class SpineSkeleton::Impl final : public Object
{
  public:
    explicit Impl(SpineSkeletonData skeleton_data);

    void update(float dt);

    void update_world_transform(SpineUpdatePhysics physics);

    SpineSkeletonData          skeleton_data;
    UniquePtr<spine::Skeleton> skeleton;
    SpineAnimationState        animation_state;

    List<SpineBone>                bones;
    List<SpineSlot>                slots;
    List<SpineIkConstraint>        ik_constraints;
    List<SpineTransformConstraint> transform_constraints;
    List<SpinePathConstraint>      path_constraints;
    List<SpinePhysicsConstraint>   physics_constraints;
};
} // namespace Polly
