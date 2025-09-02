// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

// This file contains interface for the Spine Pro runtime integration in Polly.
// Any use of this interface is subject to the Spine Editor License Agreement:
// https://esotericsoftware.com/spine-editor-license

#pragma once

#include "Polly/BlendState.hpp"
#include "Polly/Color.hpp"
#include "Polly/Linalg.hpp"
#include "Polly/List.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/Seconds.hpp"
#include "Polly/StringView.hpp"

namespace Polly
{
struct Rectangle;
class SpineAttachment;
class SpineSkeleton;

class SpineAtlas
{
    PollyObject(SpineAtlas);

  public:
    explicit SpineAtlas(StringView assetName);

    StringView assetName() const;
};

class SpineSkin
{
    PollyTransientObject(SpineSkin);
};

class SpineAnimation
{
    PollyTransientObject(SpineAnimation);

  public:
    StringView name() const;

    Seconds duration() const;

    void setDuration(Seconds value);
};

class SpineBoneData
{
    PollyTransientObject(SpineBoneData);
};

class SpineBone
{
    PollyTransientObject(SpineBone);

  public:
    [[nodiscard]]
    Vec2 worldToLocal(Vec2 worldPosition) const;

    [[nodiscard]]
    Vec2 worldToParent(Vec2 worldPosition) const;

    [[nodiscard]]
    Vec2 localToWorld(Vec2 localPosition) const;

    [[nodiscard]]
    Vec2 parentToWorld(Vec2 worldPosition) const;

    [[nodiscard]]
    Degrees worldToLocalRotation(Degrees worldRotation) const;

    [[nodiscard]]
    Degrees localToWorldRotation(Degrees localRotation) const;

    void rotateWorld(Degrees amount);

    Vec2 worldToLocalRotation() const;

    Vec2 localPosition() const;

    void setLocalPosition(Vec2 value);

    Degrees localRotation() const;

    void setLocalRotation(Degrees value);

    Vec2 localScale() const;

    void setLocalScale(Vec2 value);

    Vec2 localShear() const;

    void setLocalShear(Vec2 value);

    Vec2 worldPosition() const;

    void setWorldPosition(Vec2 value);

    Vec2 worldRotation() const;

    Vec2 worldScale() const;
};

class SpineSlotData
{
    PollyTransientObject(SpineSlotData);

    u32 index() const;

    StringView name() const;

    SpineBoneData boneData() const;

    Color color() const;

    Color darkColor() const;

    bool hasDarkColor() const;

    void setHasDarkColor(bool value);

    StringView attachmentName() const;

    void setAttachmentName(StringView value);

    BlendState blendState() const;

    bool isVisible() const;

    void setVisible(bool value);
};

class SpineSlot
{
    PollyTransientObject(SpineSlot);

    void setToSetupPose();

    SpineSlotData data() const;

    SpineBone bone() const;

    SpineSkeleton skeleton() const;

    Color color() const;

    Color darkColor() const;

    bool hasDarkColor() const;

    SpineAttachment attachment() const;

    void setAttachment(SpineAttachment value);

    u32 attachmentState() const;

    void setAttachmentState(u32 value);

    [[nodiscard]]
    Span<float> deform() const;

    u32 sequenceIndex() const;

    void setSequenceIndex(u32 value);
};

class SpineIKConstraintData
{
    PollyTransientObject(SpineIKConstraintData);

    StringView name() const;

    u32 order() const;

    void setOrder(u32 value);

    bool isSkinRequired() const;

    void setIsSkinRequired(bool value);
};

class SpineTransformConstraintData
{
    PollyTransientObject(SpineTransformConstraintData);

    List<SpineBoneData> bones() const;

    void bonesInto(List<SpineBoneData>& dst) const;

    SpineBoneData target() const;

    void setTarget(SpineBoneData value);

    float mixRotate() const;

    void setMixRotate(float value);

    float mixX() const;

    void setMixX(float value);

    float mixY() const;

    void setMixY(float value);

    float mixScaleX() const;

    void setMixScaleX(float value);

    float mixScaleY() const;

    void setMixScaleY(float value);

    float mixShearY() const;

    void setMixShearY(float value);

    float offsetRotation() const;

    void setOffsetRotation(float value);

    float offsetX() const;

    void setOffsetX(float value);

    float offsetY() const;

    void setOffsetY(float value);

    float offsetScaleX() const;

    void setOffsetScaleX(float value);

    float offsetScaleY() const;

    void setOffsetScaleY(float value);

    float offsetShearY() const;

    void setOffsetShearY(float value);

    bool isRelative() const;

    void setIsRelative(bool value);

    bool isLocal() const;

    void setIsLocal(bool value);
};

class SpinePathConstraintData
{
    PollyTransientObject(SpinePathConstraintData);
};

class SpinePhysicsConstraintData
{
    PollyTransientObject(SpinePhysicsConstraintData);
};

class SpineIkConstraint
{
    PollyTransientObject(SpineIkConstraint);
};

class SpineTransformConstraint
{
    PollyTransientObject(SpineTransformConstraint);
};

class SpinePathConstraint
{
    PollyTransientObject(SpinePathConstraint);
};

class SpinePhysicsConstraint
{
    PollyTransientObject(SpinePhysicsConstraint);
};

class SpineEventData
{
    PollyTransientObject(SpineEventData);
};

class SpineSkeletonData
{
    PollyObject(SpineSkeletonData);

  public:
    explicit SpineSkeletonData(StringView assetName, SpineAtlas atlas, float scale = 1.0f);

    StringView assetName() const;

    SpineBoneData findBone(StringView name);

    SpineSlotData findSlot(StringView name);

    SpineSkin findSkin(StringView name);

    SpineEventData findEvent(StringView name);

    SpineAnimation findAnimation(StringView name);

    SpineIKConstraintData findIKConstraint(StringView name);

    SpineTransformConstraintData findTransformConstraint(StringView name);

    SpinePathConstraintData findPathConstraint(StringView name);

    SpinePhysicsConstraintData findPhysicsConstraint(StringView name);

    StringView name() const;

    void setName(StringView value);

    Span<SpineBoneData> bones();

    Span<SpineSlotData> slots();

    Span<SpineSkin> skins();

    SpineSkin defaultSkin();

    void setDefaultSkin(const SpineSkin& value);

    Span<SpineEventData> events();

    Span<SpineAnimation> animations();

    bool hasAnimationNamed(StringView name) const;

    bool hasAnimationsNamed(Span<StringView> names) const;

    Maybe<u32> indexOfAnimation(StringView name) const;

    Span<SpineIKConstraintData> ikConstraints();

    Span<SpineTransformConstraintData> transformConstraints();

    Span<SpinePathConstraintData> pathConstraints();

    Span<SpinePhysicsConstraintData> physicsConstraints();

    Vec2 position() const;

    void setPosition(Vec2 value);

    Vec2 size() const;

    void setSize(Vec2 value);

    float referenceScale() const;

    void setReferenceScale(float value);

    float framesPerSecond() const;

    void setFramesPerSecond(float value);

    Span<StringView> animationNames() const;
};

class SpineAnimationStateData
{
    PollyObject(SpineAnimationStateData);

  public:
    explicit SpineAnimationStateData(SpineSkeletonData skeletonData);

    Seconds defaultMix() const;

    void setDefaultMix(float duration);

    Seconds mix(const SpineAnimation& from, const SpineAnimation& to) const;

    void setMix(StringView fromName, StringView toName, Seconds duration);

    void setMix(const SpineAnimation& from, const SpineAnimation& to, Seconds duration);

    void clear();
};

class SpineAttachment
{
    PollyTransientObject(SpineAttachment);

  public:
    StringView name() const;
};

class SpineAnimationState;

enum class SpineUpdatePhysics
{
    /// Physics are not updated or applied.
    None,

    /// Physics are reset to the current pose.
    Reset,

    /// Physics are updated and the pose from physics is applied.
    Update,

    /// Physics are not updated but the pose from physics is applied.
    Pose,
};

class SpineSkeleton
{
    PollyObject(SpineSkeleton);

  public:
    explicit SpineSkeleton(SpineSkeletonData skeletonData);

    void update(float dt);

    void updateWorldTransform(SpineUpdatePhysics physics = SpineUpdatePhysics::Update);

    SpineAnimationState animationState() const;

    void setAnimationState(SpineAnimationState value);

    SpineBone findBone(StringView name);

    SpineSlot findSlot(StringView name);

    void setSkin(StringView skinName);

    void setSkin(const SpineSkin& newSkin);

    SpineAttachment attachment(StringView slotName, StringView attachmentName);

    SpineAttachment attachment(u32 slotIndex, StringView attachmentName);

    void setAttachment(StringView slotName, StringView attachmentName);

    SpineIkConstraint findIKConstraint(StringView name);

    SpineTransformConstraint findTransformConstraint(StringView name);

    SpinePathConstraint findPathConstraint(StringView name);

    SpinePhysicsConstraint findPhysicsConstraint(StringView name);

    Rectangle bounds() const;

    SpineBone rootBone();

    SpineSkeletonData skeletonData();

    Span<SpineBone> bones();

    Span<SpineSlot> slots();

    Span<SpineIkConstraint> ikConstraints();

    Span<SpinePathConstraint> pathConstraints();

    [[nodiscard]]
    Span<SpineTransformConstraint> transformConstraints();

    Span<SpinePhysicsConstraint> physicsConstraints();

    SpineSkin skin();

    Color color() const;

    void setColor(Color value);

    Vec2 position() const;

    void setPosition(Vec2 value);

    Vec2 scale() const;

    void setScale(Vec2 value);

    Seconds time() const;

    void setTime(Seconds value);
};

enum class SpineMixBlend
{
    Setup,
    First,
    Replace,
    Add,
};

class SpineTrack
{
    PollyTransientObject(SpineTrack);

  public:
    u32 trackIndex() const;

    [[nodiscard]]
    bool shouldLoop() const;

    void setShouldLoop(bool value);

    [[nodiscard]]
    bool shouldHoldPrevious() const;

    void setShouldHoldPrevious(bool value);

    [[nodiscard]]
    bool shouldReverse() const;

    void setShouldReverse(bool value);

    [[nodiscard]]
    bool shouldUseShortestRotation() const;

    void setShouldUseShortestRotation(bool value);

    Seconds delay() const;

    void setDelay(Seconds value);

    [[nodiscard]]
    Seconds trackTime() const;

    void setTrackTime(Seconds value);

    [[nodiscard]]
    Seconds trackEnd() const;

    void setTrackEnd(Seconds value);

    Seconds animationStart() const;

    void setAnimationStart(Seconds value);

    [[nodiscard]]
    Seconds animationEnd() const;

    void setAnimationEnd(Seconds value);

    [[nodiscard]]
    Seconds animationLast() const;

    void setAnimationLast(Seconds value);

    Seconds animationTime() const;

    Seconds timeScale() const;

    void setTimeScale(Seconds value);

    float alpha() const;

    void setAlpha(float value);

    float eventThreshold() const;

    void setEventThreshold(float value);

    [[nodiscard]]
    float mixAttachmentThreshold() const;

    void setMixAttachmentThreshold(float value);

    float alphaAttachmentThreshold() const;

    void setAlphaAttachmentThreshold(float value);

    [[nodiscard]]
    float mixDrawOrderThreshold() const;

    void setMixDrawOrderThreshold(float value);

    [[nodiscard]]
    SpineTrack next() const;

    bool isComplete() const;

    [[nodiscard]]
    Seconds mixTime() const;

    void setMixTime(Seconds value);

    [[nodiscard]]
    Seconds mixDuration() const;

    void mixDuration(Seconds value);

    void mixDuration(Seconds duration, Seconds delay);

    [[nodiscard]]
    SpineMixBlend mixBlend() const;

    void setMixBlend(SpineMixBlend value);

    SpineTrack mixingFrom() const;

    SpineTrack mixingTo() const;

    void resetRotationDirections();
};

class SpineAnimationState
{
    PollyObject(SpineAnimationState);

  public:
    explicit SpineAnimationState(SpineAnimationStateData data);

    void update(float dt);

    void applyTo(SpineSkeleton skeleton);

    void clearTracks();

    void clearTrack(u32 trackIndex);

    SpineTrack setAnimation(u32 trackIndex, StringView animationName, bool shouldLoop);

    SpineTrack setAnimation(u32 trackIndex, const SpineAnimation& animation, bool shouldLoop);

    SpineTrack addAnimation(u32 trackIndex, StringView animationName, bool shouldLoop, Seconds delay);

    SpineTrack addAnimation(u32 trackIndex, const SpineAnimation& animation, bool shouldLoop, Seconds delay);

    SpineTrack setEmptyAnimation(u32 trackIndex, Seconds mixDuration);

    void setEmptyAnimations(Seconds mixDuration);

    SpineTrack current(u32 trackIndex);

    SpineAnimationStateData animationStateData();

    float timeScale() const;

    void setTimeScale(Seconds value);
};
} // namespace Polly