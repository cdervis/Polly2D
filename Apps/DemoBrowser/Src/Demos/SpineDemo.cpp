#include "SpineDemo.hpp"

#include "DemoBrowser.hpp"

struct SpineAssetEntry
{
    StringView display_name;
    StringView atlas_name;
    StringView skeleton_data_name;
    StringView initial_animation_name;
    float      scale = 1.0f;
};

constexpr Array s_spine_asset_entries = {
    SpineAssetEntry{
        .display_name           = "Cloud Pot",
        .atlas_name             = "Spine/cloud-pot.atlas",
        .skeleton_data_name     = "Spine/cloud-pot.json",
        .initial_animation_name = "playing-in-the-rain",
    },
    SpineAssetEntry{
        .display_name           = "Snow Globe",
        .atlas_name             = "Spine/snowglobe.atlas",
        .skeleton_data_name     = "Spine/snowglobe-pro.json",
        .initial_animation_name = "idle",
        .scale                  = 0.4f,
    },
    SpineAssetEntry{
        .display_name           = "Windmill",
        .atlas_name             = "Spine/windmill.atlas",
        .skeleton_data_name     = "Spine/windmill-ess.skel",
        .initial_animation_name = "animation",
        .scale                  = 1.0f,
    },
    SpineAssetEntry{
        .display_name           = "Spineboy",
        .atlas_name             = "Spine/spineboy-ess.atlas",
        .skeleton_data_name     = "Spine/spineboy-ess.json",
        .initial_animation_name = "walk",
    },
    SpineAssetEntry{
        .display_name           = "Dragon",
        .atlas_name             = "Spine/dragon-ess.atlas",
        .skeleton_data_name     = "Spine/dragon-ess.json",
        .initial_animation_name = "flying",
    },
    SpineAssetEntry{
        .display_name           = "Power-Up",
        .atlas_name             = "Spine/powerup.atlas",
        .skeleton_data_name     = "Spine/powerup-ess.json",
        .initial_animation_name = "bounce",
    },
    SpineAssetEntry{
        .display_name           = "Hero",
        .atlas_name             = "Spine/hero.atlas",
        .skeleton_data_name     = "Spine/hero-pro.json",
        .initial_animation_name = "walk",
        .scale                  = 1.5f,
    },
};

// Save the display names for the combo box in the GUI.
constexpr auto extractDisplayNames()
{
    auto list = Array<StringView, s_spine_asset_entries.size()>();

    for (int i = 0; const auto& entry : s_spine_asset_entries)
        list[i++] = entry.display_name;

    return list;
}

constexpr auto sAssetDisplayNames = extractDisplayNames();

SpineDemo::SpineDemo(DemoBrowser* browser)
    : Demo("Spine Demo", browser)
{
    switchToSpineEntry(0);
}

void SpineDemo::tick(GameTime time)
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

void SpineDemo::doImGui(ImGui imgui)
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
    const auto& entry = s_spine_asset_entries[index];

    // Load the atlas image for the Spine animation.
    const auto atlas = SpineAtlas(entry.atlas_name);

    // Load the Spine skeleton data.
    _skeletonData = SpineSkeletonData(entry.skeleton_data_name, atlas, entry.scale);

    // Create an animation state buffer from the skeleton data.
    _animationStateData = SpineAnimationStateData(_skeletonData);
    _animationStateData.setDefaultMix(0.1f);

    if (_skeletonData.hasAnimationsNamed(SmallList<StringView>{"jump"_sv, "walk"_sv}))
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
    _animationState.setAnimation(0, entry.initial_animation_name, true);

    _camera.position = _skeleton.bounds().center() + Vec2(250, 0);

    // Update some UI properties.
    _currentAnimationIndex = static_cast<int>(*_skeletonData.indexOfAnimation(entry.initial_animation_name));
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
