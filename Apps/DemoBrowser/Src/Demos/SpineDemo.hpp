#pragma once

#include "Demo.hpp"

class SpineDemo final : public Demo
{
  public:
    explicit SpineDemo(DemoBrowser* browser);

    void tick(GameTime time) override;

    void draw(GraphicsDevice gfx) override;

    void doImGui(ImGui imgui) override;

  private:
    void switch_to_spine_entry(u32 index);

    void onMouseWheelScrolled(const MouseWheelEvent& event) override;

    void onMouseButtonPressed(MouseButtonEvent event) override;

    void onMouseButtonReleased(const MouseButtonEvent& event) override;

    void onMouseMoved(const MouseMoveEvent& event) override;

    Camera                  _camera;
    int                     _current_asset_index = 0;
    SpineSkeletonData       _skeleton_data;
    SpineAnimationStateData _animation_state_data;
    SpineAnimationState     _animation_state;
    SpineSkeleton           _skeleton;
    float                   _target_zoom             = 1.0f;
    bool                    _is_dragging_camera      = false;
    int                     _current_animation_index = 0;
    bool                    _is_animation_playing    = true;
    float                   _playback_speed          = 1.0f;
    bool                    _should_draw_extras      = false;
};
