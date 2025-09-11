#include "SpineDemo.hpp"

#include "DemoBrowser.hpp"

struct SpineAssetEntry
{
    StringView displayName;
    StringView atlasName;
    StringView skeletonDataName;
    StringView initialAnimationName;
    float      scale = 1.0f;
};

constexpr Array sSpineAssetEntries = {
    SpineAssetEntry{
        .displayName          = "Cloud Pot",
        .atlasName            = "Spine/cloud-pot.atlas",
        .skeletonDataName     = "Spine/cloud-pot.json",
        .initialAnimationName = "playing-in-the-rain",
    },
    SpineAssetEntry{
        .displayName          = "Snow Globe",
        .atlasName            = "Spine/snowglobe.atlas",
        .skeletonDataName     = "Spine/snowglobe-pro.json",
        .initialAnimationName = "idle",
        .scale                = 0.4f,
    },
    SpineAssetEntry{
        .displayName          = "Windmill",
        .atlasName            = "Spine/windmill.atlas",
        .skeletonDataName     = "Spine/windmill-ess.skel",
        .initialAnimationName = "animation",
        .scale                = 1.0f,
    },
    SpineAssetEntry{
        .displayName          = "Spineboy",
        .atlasName            = "Spine/spineboy-ess.atlas",
        .skeletonDataName     = "Spine/spineboy-ess.json",
        .initialAnimationName = "walk",
    },
    SpineAssetEntry{
        .displayName          = "Dragon",
        .atlasName            = "Spine/dragon-ess.atlas",
        .skeletonDataName     = "Spine/dragon-ess.json",
        .initialAnimationName = "flying",
    },
    SpineAssetEntry{
        .displayName          = "Power-Up",
        .atlasName            = "Spine/powerup.atlas",
        .skeletonDataName     = "Spine/powerup-ess.json",
        .initialAnimationName = "bounce",
    },
    SpineAssetEntry{
        .displayName          = "Hero",
        .atlasName            = "Spine/hero.atlas",
        .skeletonDataName     = "Spine/hero-pro.json",
        .initialAnimationName = "walk",
        .scale                = 1.5f,
    },
};

// Save the display names for the combo box in the GUI.
constexpr auto extractDisplayNames()
{
    auto list = Array<StringView, sSpineAssetEntries.size()>();

    for (int i = 0; const auto& entry : sSpineAssetEntries)
        list[i++] = entry.displayName;

    return list;
}

constexpr auto sAssetDisplayNames = extractDisplayNames();

SpineDemo::SpineDemo(DemoBrowser* browser)
    : Demo("Spine Demo", browser)
{
    switchToSpineEntry(0);
}

void SpineDemo::update(GameTime time)
{
    if (distance(_camera.zoom, _targetZoom) > 0.001f)
    {
        _camera.zoom = lerp(_camera.zoom, _targetZoom, time.elapsed() * 3.0f);
    }

    if (_isAnimationPlaying)
    {
        _animationState.update(time.elapsed());
        _animationState.applyTo(_skeleton);
        _skeleton.update(time.elapsed());
        _skeleton.updateWorldTransform();
    }
}

void SpineDemo::draw(Painter painter)
{
    const auto window       = browser().window();
    const auto pixelRatio   = painter.pixelRatio();
    const auto camTransform = _camera.transformation(painter.viewSize());

    // Apply the camera's transformation to the scene.
    painter.setTransformation(camTransform);

    // Draw the entire Spine skeleton instance.
    painter.drawSpineSkeleton(_skeleton);

    // Draw some extra information about the skeleton.
    if (_shouldDrawExtras)
    {
        for (const auto& bone : _skeleton.bones())
        {
            painter.drawEllipse(bone.worldPosition(), Vec2(12, 12), lime.withAlpha(0.5f), 3.0f);
        }

        painter.drawRectangle(_skeleton.bounds(), red, 5.0f);
    }

    // Reset the drawing transformation in order to draw neutral text.
    painter.setTransformation({});

    painter.drawStringWithBasicShadow(
        "This demo shows how to load and play Spine sprites.\nVisit https://esotericsoftware.com "
        "for more information.",
        Font::builtin(),
        16 * pixelRatio,
        Vec2(50, 50));

    painter.drawStringWithBasicShadow(
        formatString("{}", browser().performanceStats()),
        Font::builtin(),
        32,
        Vec2(50, 130));
}

void SpineDemo::onImGui(ImGui imgui)
{
    if (imgui.combo("Asset", _currentAssetIndex, sAssetDisplayNames))
    {
        switchToSpineEntry(_currentAssetIndex);
        _animationState.setTimeScale(_isAnimationPlaying ? _playbackSpeed : 0.0f);
    }

    if (const Span animations = _skeletonData.animations(); animations.size() > 1)
    {
        if (imgui.combo("Animation", _currentAnimationIndex, _skeletonData.animationNames()))
        {
            _animationState.setAnimation(0, animations[_currentAnimationIndex], true);
        }
    }

    if (imgui.checkbox("Animate", _isAnimationPlaying))
    {
        _animationState.setTimeScale(_isAnimationPlaying ? _playbackSpeed : 0.0f);
    }

    if (_isAnimationPlaying)
    {
        auto shouldUpdateAnimState = false;

        if (imgui.slider("Speed", _playbackSpeed, 0.1f, 3.0f))
        {
            shouldUpdateAnimState = true;
        }

        if (imgui.button("Reset"))
        {
            _playbackSpeed        = 1.0f;
            shouldUpdateAnimState = true;
        }

        if (shouldUpdateAnimState)
        {
            _animationState.setTimeScale(_playbackSpeed);
        }
    }

    imgui.checkbox("Draw Extras", _shouldDrawExtras);
}

void SpineDemo::switchToSpineEntry(u32 index)
{
    const auto& entry = sSpineAssetEntries[index];

    // Load the atlas image for the Spine animation.
    const auto atlas = SpineAtlas(entry.atlasName);

    // Load the Spine skeleton data.
    _skeletonData = SpineSkeletonData(entry.skeletonDataName, atlas, entry.scale);

    // Create an animation state buffer from the skeleton data.
    _animationStateData = SpineAnimationStateData(_skeletonData);
    _animationStateData.setDefaultMix(0.1f);

    if (_skeletonData.hasAnimationsNamed(Array{"jump"_sv, "walk"_sv}))
    {
        _animationStateData.setMix("jump", "walk", 0.5f);
    }

    // Create an instance of the animation state data.
    _animationState = SpineAnimationState(_animationStateData);

    // Create a skeleton instance from the skeleton data.
    _skeleton = SpineSkeleton(_skeletonData);

    // Hook the animation state to the skeleton.
    _skeleton.setAnimationState(_animationState);

    // Start playing the default initial animation of the skeleton.
    _animationState.setAnimation(0, entry.initialAnimationName, true);

    _camera.position = _skeleton.bounds().center() + Vec2(250, 0);

    // Update some UI properties.
    _currentAnimationIndex = int(*_skeletonData.indexOfAnimation(entry.initialAnimationName));
}

void SpineDemo::onMouseWheelScrolled(const MouseWheelEvent& event)
{
    _targetZoom = clamp(_targetZoom + (event.delta.y * 0.1f), 0.1f, 1.5f);
}

void SpineDemo::onMouseButtonPressed(MouseButtonEvent event)
{
    if (event.button == MouseButton::Right)
    {
        _isDraggingCamera = true;
    }
}

void SpineDemo::onMouseButtonReleased(const MouseButtonEvent& event)
{
    if (event.button == MouseButton::Right)
    {
        _isDraggingCamera = false;
    }
}

void SpineDemo::onMouseMoved(const MouseMoveEvent& event)
{
    if (_isDraggingCamera)
    {
        _camera.position += Vec2(-event.delta.x, event.delta.y);
    }
}
