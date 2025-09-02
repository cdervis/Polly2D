// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Spine.hpp"

#include "Polly/Algorithm.hpp"
#include "Polly/ContentManagement/ContentManager.hpp"
#include "Polly/Game/GameImpl.hpp"
#include "Polly/Logging.hpp"
#include "Polly/Spine/SpineImpl.hpp"
#include <spine/AnimationStateData.h>

namespace Polly
{
static spine::String convert(StringView str)
{
    return str.isNullTerminated() ? spine::String(str.cstring(), false, false) : convert(String(str));
}

static spine::String convert(const String& str)
{
    return spine::String(str.cstring(), true, true);
}

[[maybe_unused]]
static String convert(const spine::String& str)
{
    return String(str.buffer(), static_cast<u32>(str.length()));
}

static StringView convertView(const spine::String& str)
{
    return StringView(str.buffer(), static_cast<u32>(str.length()));
}

static String getAnimationNotFoundMessage(StringView searched_for, Span<SpineAnimation> animations)
{
    auto minDistanceName = StringView();
    auto minDistance     = std::numeric_limits<double>::max();

    for (const auto& anim : animations)
    {
        const auto anim_name = anim.name();
        const auto len       = max(searched_for.size(), anim_name.size());
        const auto normalized =
            static_cast<double>(levensteinDistance(searched_for, anim_name)) / static_cast<double>(len);

        if (normalized < minDistance)
        {
            minDistance     = normalized;
            minDistanceName = anim_name;
        }
    }

    if (not minDistanceName.isEmpty())
    {
        return formatString(
            "No animation named '{}' in the skeleton; did you mean '{}'?",
            searched_for,
            minDistanceName);
    }

    return formatString("No animation named '{}' in the skeleton.", searched_for);
}

PollyImplementObject(SpineAtlas);

SpineAtlas::SpineAtlas(StringView assetName)
    : SpineAtlas()
{
    auto& content = Game::Impl::instance().contentManager();

    *this = content.loadSpineAtlas(assetName);
}

StringView SpineAtlas::assetName() const
{
    PollyDeclareThisImpl;
    return impl->assetName();
}

StringView SpineAnimation::name() const
{
    PollyDeclareThisImplAs(spine::Animation);
    return convertView(impl->getName());
}

Seconds SpineAnimation::duration() const
{
    PollyDeclareThisImplAs(spine::Animation);
    return impl->getDuration();
}

void SpineAnimation::setDuration(Seconds value)
{
    PollyDeclareThisImplAs(spine::Animation);
    impl->setDuration(value);
}

Vec2 SpineBone::worldToLocal(Vec2 worldPosition) const
{
    PollyDeclareThisImplAs(spine::Bone);

    auto x = 0.0f;
    auto y = 0.0f;
    impl->worldToLocal(worldPosition.x, worldPosition.y, x, y);

    return Vec2(x, y);
}

Vec2 SpineBone::worldToParent(Vec2 worldPosition) const
{
    PollyDeclareThisImplAs(spine::Bone);

    auto x = 0.0f;
    auto y = 0.0f;
    impl->worldToParent(worldPosition.x, worldPosition.y, x, y);

    return Vec2(x, y);
}

Vec2 SpineBone::localToWorld(Vec2 localPosition) const
{
    PollyDeclareThisImplAs(spine::Bone);

    auto x = 0.0f;
    auto y = 0.0f;
    impl->localToWorld(localPosition.x, localPosition.y, x, y);

    return Vec2(x, y);
}

Vec2 SpineBone::parentToWorld(Vec2 worldPosition) const
{
    PollyDeclareThisImplAs(spine::Bone);

    auto x = 0.0f;
    auto y = 0.0f;
    impl->parentToWorld(worldPosition.x, worldPosition.y, x, y);

    return Vec2(x, y);
}

Degrees SpineBone::worldToLocalRotation(Degrees worldRotation) const
{
    PollyDeclareThisImplAs(spine::Bone);
    return Degrees(impl->worldToLocalRotation(worldRotation.value));
}

Degrees SpineBone::localToWorldRotation(Degrees localRotation) const
{
    PollyDeclareThisImplAs(spine::Bone);
    return Degrees(impl->localToWorldRotation(localRotation.value));
}

void SpineBone::rotateWorld(Degrees amount)
{
    PollyDeclareThisImplAs(spine::Bone);
    impl->rotateWorld(amount.value);
}

Vec2 SpineBone::worldToLocalRotation() const
{
    PollyDeclareThisImplAs(spine::Bone);
    return {impl->getWorldToLocalRotationX(), impl->getWorldToLocalRotationY()};
}

Vec2 SpineBone::localPosition() const
{
    PollyDeclareThisImplAs(spine::Bone);
    return {impl->getX(), impl->getY()};
}

void SpineBone::setLocalPosition(Vec2 value)
{
    PollyDeclareThisImplAs(spine::Bone);
    impl->setX(value.x);
    impl->setY(value.y);
}

Degrees SpineBone::localRotation() const
{
    PollyDeclareThisImplAs(spine::Bone);
    return Degrees(impl->getRotation());
}

void SpineBone::setLocalRotation(Degrees value)
{
    PollyDeclareThisImplAs(spine::Bone);
    impl->setRotation(value.value);
}

Vec2 SpineBone::localScale() const
{
    PollyDeclareThisImplAs(spine::Bone);
    return {impl->getScaleX(), impl->getScaleY()};
}

void SpineBone::setLocalScale(Vec2 value)
{
    PollyDeclareThisImplAs(spine::Bone);
    impl->setScaleX(value.x);
    impl->setScaleY(value.y);
}

Vec2 SpineBone::localShear() const
{
    PollyDeclareThisImplAs(spine::Bone);
    return {impl->getShearX(), impl->getShearY()};
}

void SpineBone::setLocalShear(Vec2 value)
{
    PollyDeclareThisImplAs(spine::Bone);
    impl->setShearX(value.x);
    impl->setShearY(value.y);
}

Vec2 SpineBone::worldPosition() const
{
    PollyDeclareThisImplAs(spine::Bone);
    return Vec2(impl->getWorldX(), impl->getWorldY());
}

void SpineBone::setWorldPosition(Vec2 value)
{
    PollyDeclareThisImplAs(spine::Bone);
    impl->setWorldX(value.x);
    impl->setWorldY(value.y);
}

Vec2 SpineBone::worldRotation() const
{
    PollyDeclareThisImplAs(spine::Bone);
    return Vec2(impl->getWorldRotationX(), impl->getWorldRotationY());
}

Vec2 SpineBone::worldScale() const
{
    PollyDeclareThisImplAs(spine::Bone);
    return Vec2(impl->getWorldScaleX(), impl->getWorldScaleY());
}

u32 SpineSlotData::index() const
{
    PollyDeclareThisImplAs(spine::SlotData);
    return static_cast<u32>(impl->getIndex());
}

StringView SpineSlotData::name() const
{
    PollyDeclareThisImplAs(spine::SlotData);
    return StringView(impl->getName().buffer(), static_cast<u32>(impl->getName().length()));
}

SpineBoneData SpineSlotData::boneData() const
{
    PollyDeclareThisImplAs(spine::SlotData);
    return SpineBoneData(std::addressof(impl->getBoneData()));
}

Color SpineSlotData::color() const
{
    PollyDeclareThisImplAs(spine::SlotData);
    const auto color = impl->getColor();
    return Color(color.r, color.g, color.b, color.a);
}

Color SpineSlotData::darkColor() const
{
    PollyDeclareThisImplAs(spine::SlotData);
    const auto color = impl->getDarkColor();
    return Color(color.r, color.g, color.b, color.a);
}

bool SpineSlotData::hasDarkColor() const
{
    PollyDeclareThisImplAs(spine::SlotData);
    return impl->hasDarkColor();
}

void SpineSlotData::setHasDarkColor(bool value)
{
    PollyDeclareThisImplAs(spine::SlotData);
    impl->setHasDarkColor(value);
}

StringView SpineSlotData::attachmentName() const
{
    PollyDeclareThisImplAs(spine::SlotData);
    return StringView(
        impl->getAttachmentName().buffer(),
        static_cast<u32>(impl->getAttachmentName().length()));
}

void SpineSlotData::setAttachmentName(StringView value)
{
    PollyDeclareThisImplAs(spine::SlotData);
    impl->setAttachmentName(
        spine::String(value.isNullTerminated() ? value.cstring() : String(value).cstring(), true, true));
}

BlendState SpineSlotData::blendState() const
{
    // TODO
    return non_premultiplied;
}

bool SpineSlotData::isVisible() const
{
    PollyDeclareThisImplAs(spine::SlotData);
    return impl->isVisible();
}

void SpineSlotData::setVisible(bool value)
{
    PollyDeclareThisImplAs(spine::SlotData);
    impl->setVisible(value);
}

PollyImplementObject(SpineSkeletonData);

SpineSkeletonData::SpineSkeletonData(StringView assetName, SpineAtlas atlas, float scale)
    : SpineSkeletonData()
{
    if (assetName.isEmpty())
    {
        throw Error("No asset name specified.");
    }

    if (not atlas)
    {
        throw Error("No atlas specified.");
    }

    if (scale <= 0.0f)
    {
        throw Error("Invalid scale specified.");
    }

    auto& content = Game::Impl::instance().contentManager();

    *this = content.loadSpineSkeletonData(assetName, std::move(atlas), scale);
}

StringView SpineSkeletonData::assetName() const
{
    PollyDeclareThisImpl;
    return impl->assetName();
}

SpineBoneData SpineSkeletonData::findBone(StringView name)
{
    PollyDeclareThisImpl;
    return SpineBoneData(impl->skeleton_data->findBone(convert(name)));
}

SpineSlotData SpineSkeletonData::findSlot(StringView name)
{
    PollyDeclareThisImpl;
    return SpineSlotData(impl->skeleton_data->findSlot(convert(name)));
}

SpineSkin SpineSkeletonData::findSkin(StringView name)
{
    PollyDeclareThisImpl;
    return SpineSkin(impl->skeleton_data->findSkin(convert(name)));
}

SpineEventData SpineSkeletonData::findEvent(StringView name)
{
    PollyDeclareThisImpl;
    return SpineEventData(impl->skeleton_data->findEvent(convert(name)));
}

SpineAnimation SpineSkeletonData::findAnimation(StringView name)
{
    PollyDeclareThisImpl;
    return SpineAnimation(impl->skeleton_data->findAnimation(convert(name)));
}

SpineIKConstraintData SpineSkeletonData::findIKConstraint(StringView name)
{
    PollyDeclareThisImpl;
    return SpineIKConstraintData(impl->skeleton_data->findIkConstraint(convert(name)));
}

SpineTransformConstraintData SpineSkeletonData::findTransformConstraint(StringView name)
{
    PollyDeclareThisImpl;
    return SpineTransformConstraintData(impl->skeleton_data->findTransformConstraint(convert(name)));
}

SpinePathConstraintData SpineSkeletonData::findPathConstraint(StringView name)
{
    PollyDeclareThisImpl;
    return SpinePathConstraintData(impl->skeleton_data->findPathConstraint(convert(name)));
}

SpinePhysicsConstraintData SpineSkeletonData::findPhysicsConstraint(StringView name)
{
    PollyDeclareThisImpl;
    return SpinePhysicsConstraintData(impl->skeleton_data->findPhysicsConstraint(convert(name)));
}

StringView SpineSkeletonData::name() const
{
    PollyDeclareThisImpl;
    return convertView(const_cast<Impl*>(impl)->skeleton_data->getName());
}

void SpineSkeletonData::setName(StringView value)
{
    PollyDeclareThisImpl;
    impl->skeleton_data->setName(convert(value));
}

Span<SpineBoneData> SpineSkeletonData::bones()
{
    PollyDeclareThisImpl;
    return impl->bones;
}

Span<SpineSlotData> SpineSkeletonData::slots()
{
    PollyDeclareThisImpl;
    return impl->slots;
}

Span<SpineSkin> SpineSkeletonData::skins()
{
    PollyDeclareThisImpl;
    return impl->skins;
}

SpineSkin SpineSkeletonData::defaultSkin()
{
    PollyDeclareThisImpl;
    return SpineSkin(impl->skeleton_data->getDefaultSkin());
}

void SpineSkeletonData::setDefaultSkin(const SpineSkin& value)
{
    PollyDeclareThisImpl;
    impl->skeleton_data->setDefaultSkin(static_cast<spine::Skin*>(value.impl()));
}

Span<SpineEventData> SpineSkeletonData::events()
{
    PollyDeclareThisImpl;
    return impl->events;
}

Span<SpineAnimation> SpineSkeletonData::animations()
{
    PollyDeclareThisImpl;
    return impl->animations;
}

bool SpineSkeletonData::hasAnimationNamed(StringView name) const
{
    PollyDeclareThisImpl;
    return const_cast<Impl*>(impl)->skeleton_data->findAnimation(convert(name)) != nullptr;
}

bool SpineSkeletonData::hasAnimationsNamed(Span<StringView> names) const
{
    PollyDeclareThisImpl;
    auto* non_const_impl = const_cast<Impl*>(impl);

    return all(
        names,
        [&](StringView name)
        { return non_const_impl->skeleton_data->findAnimation(convert(name)) != nullptr; });
}

Maybe<u32> SpineSkeletonData::indexOfAnimation(StringView name) const
{
    PollyDeclareThisImpl;

    return indexOfWhere(_impl->animations, [name](const auto& anim) { return anim.name() == name; });
}

Span<SpineIKConstraintData> SpineSkeletonData::ikConstraints()
{
    PollyDeclareThisImpl;
    return impl->ik_constraints;
}

Span<SpineTransformConstraintData> SpineSkeletonData::transformConstraints()
{
    PollyDeclareThisImpl;
    return impl->transform_constraints;
}

Span<SpinePathConstraintData> SpineSkeletonData::pathConstraints()
{
    PollyDeclareThisImpl;
    return impl->path_constraints;
}

Span<SpinePhysicsConstraintData> SpineSkeletonData::physicsConstraints()
{
    PollyDeclareThisImpl;
    return impl->physics_constraints;
}

Vec2 SpineSkeletonData::position() const
{
    PollyDeclareThisImpl;
    auto* non_const_impl = const_cast<Impl*>(impl);

    return Vec2(non_const_impl->skeleton_data->getX(), non_const_impl->skeleton_data->getY());
}

void SpineSkeletonData::setPosition(Vec2 value)
{
    PollyDeclareThisImpl;

    impl->skeleton_data->setX(value.x);
    impl->skeleton_data->setY(value.y);
}

Vec2 SpineSkeletonData::size() const
{
    PollyDeclareThisImpl;
    auto* non_const_impl = const_cast<Impl*>(impl);

    return {non_const_impl->skeleton_data->getWidth(), non_const_impl->skeleton_data->getHeight()};
}

void SpineSkeletonData::setSize(Vec2 value)
{
    PollyDeclareThisImpl;

    impl->skeleton_data->setWidth(value.x);
    impl->skeleton_data->setHeight(value.y);
}

float SpineSkeletonData::referenceScale() const
{
    PollyDeclareThisImpl;
    auto* non_const_impl = const_cast<Impl*>(impl);

    return non_const_impl->skeleton_data->getReferenceScale();
}

void SpineSkeletonData::setReferenceScale(float value)
{
    PollyDeclareThisImpl;
    impl->skeleton_data->setReferenceScale(value);
}

float SpineSkeletonData::framesPerSecond() const
{
    PollyDeclareThisImpl;
    auto* non_const_impl = const_cast<Impl*>(impl);

    return non_const_impl->skeleton_data->getFps();
}

void SpineSkeletonData::setFramesPerSecond(float value)
{
    PollyDeclareThisImpl;
    impl->skeleton_data->setFps(value);
}

Span<StringView> SpineSkeletonData::animationNames() const
{
    PollyDeclareThisImpl;
    return impl->animation_names_views;
}

StringView SpineAttachment::name() const
{
    PollyDeclareThisImplAs(spine::Attachment);
    return convertView(impl->getName());
}

PollyImplementObject(SpineSkeleton);

SpineSkeleton::SpineSkeleton(SpineSkeletonData skeletonData)
    : SpineSkeleton()
{
    if (not skeletonData)
    {
        throw Error("No skeleton data specified.");
    }

    setImpl(*this, makeUnique<Impl>(std::move(skeletonData)).release());
}

void SpineSkeleton::update(float dt)
{
    PollyDeclareThisImpl;
    impl->update(dt);
}

void SpineSkeleton::updateWorldTransform(SpineUpdatePhysics physics)
{
    PollyDeclareThisImpl;
    impl->update_world_transform(physics);
}

SpineAnimationState SpineSkeleton::animationState() const
{
    PollyDeclareThisImpl;
    return impl->animation_state;
}

void SpineSkeleton::setAnimationState(SpineAnimationState value)
{
    PollyDeclareThisImpl;
    impl->animation_state = std::move(value);

    if (impl->animation_state)
    {
        auto* anim_state = impl->animation_state.impl()->state.get();
        auto* skeleton   = impl->skeleton.get();

        anim_state->apply(*skeleton);

        skeleton->update(0.0f);
        skeleton->updateWorldTransform(spine::Physics_Update);
    }
}

SpineBone SpineSkeleton::findBone(StringView name)
{
    PollyDeclareThisImpl;
    return SpineBone(impl->skeleton->findBone(convert(name)));
}

SpineSlot SpineSkeleton::findSlot(StringView name)
{
    PollyDeclareThisImpl;
    return SpineSlot(impl->skeleton->findSlot(convert(name)));
}

void SpineSkeleton::setSkin(StringView skinName)
{
    PollyDeclareThisImpl;
    impl->skeleton->setSkin(convert(skinName));
}

void SpineSkeleton::setSkin(const SpineSkin& newSkin)
{
    PollyDeclareThisImpl;
    impl->skeleton->setSkin(static_cast<spine::Skin*>(newSkin.impl()));
}

SpineAttachment SpineSkeleton::attachment(StringView slotName, StringView attachmentName)
{
    PollyDeclareThisImpl;
    return SpineAttachment(impl->skeleton->getAttachment(convert(slotName), convert(attachmentName)));
}

SpineAttachment SpineSkeleton::attachment(u32 slotIndex, StringView attachmentName)
{
    PollyDeclareThisImpl;
    return SpineAttachment(
        impl->skeleton->getAttachment(static_cast<int>(slotIndex), convert(attachmentName)));
}

void SpineSkeleton::setAttachment(StringView slotName, StringView attachmentName)
{
    PollyDeclareThisImpl;
    impl->skeleton->setAttachment(convert(slotName), convert(attachmentName));
}

SpineIkConstraint SpineSkeleton::findIKConstraint(StringView name)
{
    PollyDeclareThisImpl;
    return SpineIkConstraint(impl->skeleton->findIkConstraint(convert(name)));
}

SpineTransformConstraint SpineSkeleton::findTransformConstraint(StringView name)
{
    PollyDeclareThisImpl;
    return SpineTransformConstraint(impl->skeleton->findTransformConstraint(convert(name)));
}

SpinePathConstraint SpineSkeleton::findPathConstraint(StringView name)
{
    PollyDeclareThisImpl;
    return SpinePathConstraint(impl->skeleton->findPathConstraint(convert(name)));
}

SpinePhysicsConstraint SpineSkeleton::findPhysicsConstraint(StringView name)
{
    PollyDeclareThisImpl;
    return SpinePhysicsConstraint(impl->skeleton->findPhysicsConstraint(convert(name)));
}

Rectangle SpineSkeleton::bounds() const
{
    PollyDeclareThisImpl;
    auto* nonConstImpl = const_cast<Impl*>(impl);

    auto vec = spine::Vector<float>();

    auto x      = 0.0f;
    auto y      = 0.0f;
    auto width  = 0.0f;
    auto height = 0.0f;
    nonConstImpl->skeleton->getBounds(x, y, width, height, vec);

    return {x, y, width, height};
}

SpineBone SpineSkeleton::rootBone()
{
    PollyDeclareThisImpl;
    return SpineBone(impl->skeleton->getRootBone());
}

SpineSkeletonData SpineSkeleton::skeletonData()
{
    PollyDeclareThisImpl;
    return impl->skeleton_data;
}

Span<SpineBone> SpineSkeleton::bones()
{
    PollyDeclareThisImpl;
    return impl->bones;
}

Span<SpineSlot> SpineSkeleton::slots()
{
    PollyDeclareThisImpl;
    return impl->slots;
}

Span<SpineIkConstraint> SpineSkeleton::ikConstraints()
{
    PollyDeclareThisImpl;
    return impl->ik_constraints;
}

Span<SpinePathConstraint> SpineSkeleton::pathConstraints()
{
    PollyDeclareThisImpl;
    return impl->path_constraints;
}

Span<SpineTransformConstraint> SpineSkeleton::transformConstraints()
{
    PollyDeclareThisImpl;
    return impl->transform_constraints;
}

Span<SpinePhysicsConstraint> SpineSkeleton::physicsConstraints()
{
    PollyDeclareThisImpl;
    return impl->physics_constraints;
}

SpineSkin SpineSkeleton::skin()
{
    PollyDeclareThisImpl;
    return SpineSkin(impl->skeleton->getSkin());
}

Vec2 SpineSkeleton::position() const
{
    PollyDeclareThisImpl;
    auto* non_const_impl = const_cast<Impl*>(impl);

    return {non_const_impl->skeleton->getX(), non_const_impl->skeleton->getY()};
}

void SpineSkeleton::setPosition(Vec2 value)
{
    PollyDeclareThisImpl;
    impl->skeleton->setPosition(value.x, value.y);
}

Vec2 SpineSkeleton::scale() const
{
    PollyDeclareThisImpl;
    auto* non_const_impl = const_cast<Impl*>(impl);

    return {non_const_impl->skeleton->getScaleX(), non_const_impl->skeleton->getScaleY()};
}

void SpineSkeleton::setScale(Vec2 value)
{
    PollyDeclareThisImpl;

    impl->skeleton->setScaleX(value.x);
    impl->skeleton->setScaleY(value.y);
}

Seconds SpineSkeleton::time() const
{
    PollyDeclareThisImpl;
    auto* non_const_impl = const_cast<Impl*>(impl);

    return non_const_impl->skeleton->getTime();
}

void SpineSkeleton::setTime(Seconds value)
{
    PollyDeclareThisImpl;
    impl->skeleton->setTime(value);
}

u32 SpineTrack::trackIndex() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return static_cast<u32>(impl->getTrackIndex());
}

bool SpineTrack::shouldLoop() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return impl->getLoop();
}

void SpineTrack::setShouldLoop(bool value)
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    impl->setLoop(value);
}

bool SpineTrack::shouldHoldPrevious() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return impl->getHoldPrevious();
}

void SpineTrack::setShouldHoldPrevious(bool value)
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    impl->setHoldPrevious(value);
}

bool SpineTrack::shouldReverse() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return impl->getReverse();
}

void SpineTrack::setShouldReverse(bool value)
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    impl->setReverse(value);
}

bool SpineTrack::shouldUseShortestRotation() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return impl->getShortestRotation();
}

void SpineTrack::setShouldUseShortestRotation(bool value)
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    impl->setShortestRotation(value);
}

Seconds SpineTrack::delay() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return impl->getDelay();
}

void SpineTrack::setDelay(Seconds value)
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    impl->setDelay(value);
}

Seconds SpineTrack::trackTime() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return impl->getTrackTime();
}

void SpineTrack::setTrackTime(Seconds value)
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    impl->setTrackEnd(value);
}

Seconds SpineTrack::trackEnd() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return impl->getTrackEnd();
}

void SpineTrack::setTrackEnd(Seconds value)
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    impl->setTrackEnd(value);
}

Seconds SpineTrack::animationStart() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return impl->getAnimationStart();
}

void SpineTrack::setAnimationStart(Seconds value)
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    impl->setAnimationStart(value);
}

Seconds SpineTrack::animationEnd() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return impl->getAnimationEnd();
}

void SpineTrack::setAnimationEnd(Seconds value)
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    impl->setAnimationEnd(value);
}

Seconds SpineTrack::animationLast() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return impl->getAnimationLast();
}

void SpineTrack::setAnimationLast(Seconds value)
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    impl->setAnimationLast(value);
}

Seconds SpineTrack::animationTime() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return impl->getAnimationTime();
}

Seconds SpineTrack::timeScale() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return impl->getTimeScale();
}

void SpineTrack::setTimeScale(Seconds value)
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    impl->setTimeScale(value);
}

float SpineTrack::alpha() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return impl->getAlpha();
}

void SpineTrack::setAlpha(float value)
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    impl->setAlpha(value);
}

float SpineTrack::eventThreshold() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return impl->getEventThreshold();
}

void SpineTrack::setEventThreshold(float value)
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    impl->setEventThreshold(value);
}

float SpineTrack::mixAttachmentThreshold() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return impl->getMixAttachmentThreshold();
}

void SpineTrack::setMixAttachmentThreshold(float value)
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    impl->setMixAttachmentThreshold(value);
}

float SpineTrack::alphaAttachmentThreshold() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return impl->getAlphaAttachmentThreshold();
}

void SpineTrack::setAlphaAttachmentThreshold(float value)
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    impl->setAlphaAttachmentThreshold(value);
}

float SpineTrack::mixDrawOrderThreshold() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return impl->getMixDrawOrderThreshold();
}

void SpineTrack::setMixDrawOrderThreshold(float value)
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    impl->setMixDrawOrderThreshold(value);
}

SpineTrack SpineTrack::next() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return SpineTrack(impl->getNext());
}

bool SpineTrack::isComplete() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return impl->isComplete();
}

Seconds SpineTrack::mixTime() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return impl->getMixTime();
}

void SpineTrack::setMixTime(Seconds value)
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    impl->setMixTime(value);
}

Seconds SpineTrack::mixDuration() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return impl->getMixDuration();
}

void SpineTrack::mixDuration(Seconds value)
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    impl->setMixDuration(value);
}

void SpineTrack::mixDuration(Seconds duration, Seconds delay)
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    impl->setMixDuration(duration, delay);
}

SpineMixBlend SpineTrack::mixBlend() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);

    return [value = impl->getMixBlend()]
    {
        switch (value)
        {
            case spine::MixBlend_Add: return SpineMixBlend::Add;
            case spine::MixBlend_First: return SpineMixBlend::First;
            case spine::MixBlend_Replace: return SpineMixBlend::Replace;
            case spine::MixBlend_Setup: return SpineMixBlend::Setup;
        }

        return SpineMixBlend::Add;
    }();
}

void SpineTrack::setMixBlend(SpineMixBlend value)
{
    PollyDeclareThisImplAs(spine::TrackEntry);

    impl->setMixBlend(
        [value]
        {
            switch (value)
            {
                case SpineMixBlend::Add: return spine::MixBlend_Add;
                case SpineMixBlend::First: return spine::MixBlend_First;
                case SpineMixBlend::Replace: return spine::MixBlend_Replace;
                case SpineMixBlend::Setup: return spine::MixBlend_Setup;
            }

            return spine::MixBlend_Add;
        }());
}

SpineTrack SpineTrack::mixingFrom() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return SpineTrack(impl->getMixingFrom());
}

SpineTrack SpineTrack::mixingTo() const
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    return SpineTrack(impl->getMixingTo());
}

void SpineTrack::resetRotationDirections()
{
    PollyDeclareThisImplAs(spine::TrackEntry);
    impl->resetRotationDirections();
}

PollyImplementObject(SpineAnimationState);

SpineAnimationState::SpineAnimationState(SpineAnimationStateData data)
    : SpineAnimationState()
{
    setImpl(*this, makeUnique<Impl>(std::move(data)).release());
}

void SpineAnimationState::update(float dt)
{
    PollyDeclareThisImpl;
    impl->update(dt);
}

void SpineAnimationState::applyTo(SpineSkeleton skeleton)
{
    PollyDeclareThisImpl;
    impl->apply_to(skeleton);
}

void SpineAnimationState::clearTracks()
{
    PollyDeclareThisImpl;
    impl->state->clearTracks();
}

void SpineAnimationState::clearTrack(u32 trackIndex)
{
    PollyDeclareThisImpl;
    impl->state->clearTrack(trackIndex);
}

SpineTrack SpineAnimationState::setAnimation(u32 trackIndex, StringView animationName, bool shouldLoop)
{
    PollyDeclareThisImpl;

    auto& skeleton_data_impl = *impl->data.impl()->skeleton_data.impl();
    auto& skeleton_data      = *skeleton_data_impl.skeleton_data;
    auto* anim_ptr           = skeleton_data.findAnimation(convert(animationName));

    if (not anim_ptr)
    {
        throw Error(getAnimationNotFoundMessage(animationName, skeleton_data_impl.animations));
    }

    return SpineTrack(impl->state->setAnimation(trackIndex, anim_ptr, shouldLoop));
}

SpineTrack SpineAnimationState::setAnimation(u32 trackIndex, const SpineAnimation& animation, bool shouldLoop)
{
    if (not animation)
    {
        throw Error("No animation specified.");
    }

    PollyDeclareThisImpl;

    return SpineTrack(
        impl->state->setAnimation(trackIndex, static_cast<spine::Animation*>(animation.impl()), shouldLoop));
}

SpineTrack SpineAnimationState::addAnimation(
    u32        trackIndex,
    StringView animationName,
    bool       shouldLoop,
    Seconds    delay)
{
    PollyDeclareThisImpl;
    return SpineTrack(impl->state->addAnimation(trackIndex, convert(animationName), shouldLoop, delay));
}

SpineTrack SpineAnimationState::addAnimation(
    u32                   trackIndex,
    const SpineAnimation& animation,
    bool                  shouldLoop,
    Seconds               delay)
{
    if (not animation)
    {
        throw Error("No animation specified.");
    }

    PollyDeclareThisImpl;

    return SpineTrack(
        impl->state
            ->addAnimation(trackIndex, static_cast<spine::Animation*>(animation.impl()), shouldLoop, delay));
}

SpineTrack SpineAnimationState::setEmptyAnimation(u32 trackIndex, Seconds mixDuration)
{
    PollyDeclareThisImpl;
    return SpineTrack(impl->state->setEmptyAnimation(trackIndex, mixDuration));
}

void SpineAnimationState::setEmptyAnimations(Seconds mixDuration)
{
    PollyDeclareThisImpl;
    impl->state->setEmptyAnimations(mixDuration);
}

SpineTrack SpineAnimationState::current(u32 trackIndex)
{
    PollyDeclareThisImpl;
    return SpineTrack(impl->state->getCurrent(trackIndex));
}

SpineAnimationStateData SpineAnimationState::animationStateData()
{
    PollyDeclareThisImpl;
    return impl->data;
}

Seconds SpineAnimationState::timeScale() const
{
    PollyDeclareThisImpl;
    auto* non_const_impl = const_cast<Impl*>(impl);

    return non_const_impl->state->getTimeScale();
}

void SpineAnimationState::setTimeScale(Seconds value)
{
    PollyDeclareThisImpl;
    impl->state->setTimeScale(value);
}

Color SpineSkeleton::color() const
{
    PollyDeclareThisImpl;
    auto* non_const_impl = const_cast<Impl*>(impl);

    const auto color = non_const_impl->skeleton->getColor();
    return {color.r, color.g, color.b, color.a};
}

void SpineSkeleton::setColor(Color value)
{
    PollyDeclareThisImpl;
    impl->skeleton->getColor() = spine::Color(value.r, value.g, value.b, value.a);
}

PollyImplementObject(SpineAnimationStateData);

SpineAnimationStateData::SpineAnimationStateData(SpineSkeletonData skeletonData)
    : SpineAnimationStateData()
{
    if (not skeletonData)
    {
        throw Error("No skeleton data specified.");
    }

    setImpl(*this, makeUnique<Impl>(std::move(skeletonData)).release());
}

Seconds SpineAnimationStateData::defaultMix() const
{
    PollyDeclareThisImpl;
    auto* non_const_impl = const_cast<Impl*>(impl);

    return non_const_impl->data->getDefaultMix();
}

void SpineAnimationStateData::setDefaultMix(float duration)
{
    PollyDeclareThisImpl;
    impl->data->setDefaultMix(duration);
}

Seconds SpineAnimationStateData::mix(const SpineAnimation& from, const SpineAnimation& to) const
{
    PollyDeclareThisImpl;
    auto* non_const_impl = const_cast<Impl*>(impl);

    return non_const_impl->data->getMix(
        static_cast<spine::Animation*>(from.impl()),
        static_cast<spine::Animation*>(to.impl()));
}

void SpineAnimationStateData::setMix(StringView fromName, StringView toName, float duration)
{
    PollyDeclareThisImpl;

    auto& skeleton_data_impl = *impl->skeleton_data.impl();
    auto& skeleton_data      = *skeleton_data_impl.skeleton_data;

    auto* anim1_ptr = skeleton_data.findAnimation(convert(fromName));
    auto* anim2_ptr = skeleton_data.findAnimation(convert(toName));

    if (not anim1_ptr)
    {
        throw Error(getAnimationNotFoundMessage(fromName, skeleton_data_impl.animations));
    }

    if (not anim2_ptr)
    {
        throw Error(getAnimationNotFoundMessage(toName, skeleton_data_impl.animations));
    }

    impl->data->setMix(anim1_ptr, anim2_ptr, duration);
}

void SpineAnimationStateData::setMix(const SpineAnimation& from, const SpineAnimation& to, Seconds duration)
{
    PollyDeclareThisImpl;

    impl->data->setMix(
        static_cast<spine::Animation*>(from.impl()),
        static_cast<spine::Animation*>(to.impl()),
        duration);
}

void SpineAnimationStateData::clear()
{
    PollyDeclareThisImpl;
    impl->data->clear();
}
} // namespace Polly