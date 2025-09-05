#pragma once

#include "Demo.hpp"

class SpineDemo final : public Demo
{
  public:
    explicit SpineDemo(DemoBrowser* browser);

    void tick(GameTime time) override;

    void draw(Painter painter) override;

    void doImGui(ImGui imgui) override;

  private:
    void switchToSpineEntry(u32 index);

    void onMouseWheelScrolled(const MouseWheelEvent& event) override;

    void onMouseButtonPressed(MouseButtonEvent event) override;

    void onMouseButtonReleased(const MouseButtonEvent& event) override;

    void onMouseMoved(const MouseMoveEvent& event) override;

    Camera                  _camera;
    int                     _currentAssetIndex = 0;
    SpineSkeletonData       _skeletonData;
    SpineAnimationStateData _animationStateData;
    SpineAnimationState     _animationState;
    SpineSkeleton           _skeleton;
    float                   _targetZoom            = 1.0f;
    bool                    _isDraggingCamera      = false;
    int                     _currentAnimationIndex = 0;
    bool                    _isAnimationPlaying    = true;
    float                   _playbackSpeed         = 1.0f;
    bool                    _shouldDrawExtras      = false;
};
