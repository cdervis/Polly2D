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
constexpr auto extract_display_names()
{
    auto list = Array<StringView, s_spine_asset_entries.size()>();

    for (int i = 0; const auto& entry : s_spine_asset_entries)
        list[i++] = entry.display_name;

    return list;
}

constexpr auto s_asset_display_names = extract_display_names();

SpineDemo::SpineDemo(DemoBrowser* browser)
    : Demo("Spine Demo", browser)
{
    switch_to_spine_entry(0);
}

void SpineDemo::tick(GameTime time)
{
    if (distance(_camera.zoom, _target_zoom) > 0.001f)
    {
        _camera.zoom = lerp(_camera.zoom, _target_zoom, time.elapsed() * 3.0f);
    }

    if (_is_animation_playing)
    {
        _animation_state.update(time.elapsed());
        _animation_state.applyTo(_skeleton);
        _skeleton.update(time.elapsed());
        _skeleton.updateWorldTransform();
    }
}

void SpineDemo::draw(Painter painter)
{
    const auto window        = browser().window();
    const auto pixel_ratio   = painter.pixelRatio();
    const auto cam_transform = _camera.transformation(painter.viewSize());

    // Apply the camera's transformation to the scene.
    painter.setTransformation(cam_transform);

    // Draw the entire Spine skeleton instance.
    painter.drawSpineSkeleton(_skeleton);

    // Draw some extra information about the skeleton.
    if (_should_draw_extras)
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
        16 * pixel_ratio,
        Vec2(50, 50));

    painter.drawStringWithBasicShadow(
        formatString("{}", browser().performanceStats()),
        Font::builtin(),
        32,
        Vec2(50, 130));
}

void SpineDemo::doImGui(ImGui imgui)
{
    if (imgui.combo("Asset", _current_asset_index, s_asset_display_names))
    {
        switch_to_spine_entry(_current_asset_index);
        _animation_state.setTimeScale(_is_animation_playing ? _playback_speed : 0.0f);
    }

    if (const Span animations = _skeleton_data.animations(); animations.size() > 1)
    {
        if (imgui.combo("Animation", _current_animation_index, _skeleton_data.animationNames()))
        {
            _animation_state.setAnimation(0, animations[_current_animation_index], true);
        }
    }

    if (imgui.checkbox("Animate", _is_animation_playing))
    {
        _animation_state.setTimeScale(_is_animation_playing ? _playback_speed : 0.0f);
    }

    if (_is_animation_playing)
    {
        bool should_update_anim_state = false;

        if (imgui.slider("Speed", _playback_speed, 0.1f, 3.0f))
        {
            should_update_anim_state = true;
        }

        if (imgui.button("Reset"))
        {
            _playback_speed          = 1.0f;
            should_update_anim_state = true;
        }

        if (should_update_anim_state)
        {
            _animation_state.setTimeScale(_playback_speed);
        }
    }

    imgui.checkbox("Draw Extras", _should_draw_extras);
}

void SpineDemo::switch_to_spine_entry(u32 index)
{
    const auto& entry = s_spine_asset_entries[index];

    // Load the atlas image for the Spine animation.
    const auto atlas = SpineAtlas(entry.atlas_name);

    // Load the Spine skeleton data.
    _skeleton_data = SpineSkeletonData(entry.skeleton_data_name, atlas, entry.scale);

    // Create an animation state buffer from the skeleton data.
    _animation_state_data = SpineAnimationStateData(_skeleton_data);
    _animation_state_data.setDefaultMix(0.1f);

    if (_skeleton_data.hasAnimationsNamed(SmallList{"jump"_sv, "walk"_sv}))
    {
        _animation_state_data.setMix("jump", "walk", 0.5f);
    }

    // Create an instance of the animation state data.
    _animation_state = SpineAnimationState(_animation_state_data);

    // Create a skeleton instance from the skeleton data.
    _skeleton = SpineSkeleton(_skeleton_data);

    // Hook the animation state to the skeleton.
    _skeleton.setAnimationState(_animation_state);

    // Start playing the default initial animation of the skeleton.
    _animation_state.setAnimation(0, entry.initial_animation_name, true);

    _camera.position = _skeleton.bounds().center() + Vec2(250, 0);

    // Update some UI properties.
    _current_animation_index = int(*_skeleton_data.indexOfAnimation(entry.initial_animation_name));
}

void SpineDemo::onMouseWheelScrolled(const MouseWheelEvent& event)
{
    _target_zoom = clamp(_target_zoom + (event.delta.y * 0.1f), 0.1f, 1.5f);
}

void SpineDemo::onMouseButtonPressed(MouseButtonEvent event)
{
    if (event.button == MouseButton::Right)
    {
        _is_dragging_camera = true;
    }
}

void SpineDemo::onMouseButtonReleased(const MouseButtonEvent& event)
{
    if (event.button == MouseButton::Right)
    {
        _is_dragging_camera = false;
    }
}

void SpineDemo::onMouseMoved(const MouseMoveEvent& event)
{
    if (_is_dragging_camera)
    {
        _camera.position += Vec2(-event.delta.x, event.delta.y);
    }
}
