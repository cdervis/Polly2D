#pragma once

#include "Demo.hpp"

class SpritesDemo final : public Demo
{
  public:
    explicit SpritesDemo(DemoBrowser* browser);

    void tick(GameTime time) override;

    void draw(Painter painter) override;

    void doImGui(ImGui imgui) override;

  private:
    Image _logo;
    Image _logoTiny;
    Color _imageColor          = white;
    float _imageRotation       = 0.0f;
    bool  _rotate              = true;
    float _rotationSpeed       = 5.0f;
    Vec2  _imageScale          = Vec2(1, 1);
    Vec2  _imageOrigin         = Vec2(16, 16);
    bool  _enableInterpolation = true;
    bool  _enableAlphaBlending = true;
    bool  _flipImage           = false;
    bool  _samplerMirror       = false;
    float _uvScale             = 1.0f;
};
