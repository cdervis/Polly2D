#pragma once

#include "Demo.hpp"

class SpritesDemo final : public Demo
{
  public:
    explicit SpritesDemo(DemoBrowser* browser);

    void tick(GameTime time) override;

    void draw(GraphicsDevice gfx) override;

    void doImGui(ImGui imgui) override;

  private:
    Image m_logo;
    Image m_logoTiny;
    Color m_imageColor          = white;
    float m_imageRotation       = 0.0f;
    bool  m_rotate              = true;
    float m_rotationSpeed       = 5.0f;
    Vec2  m_imageScale          = {1, 1};
    Vec2  m_imageOrigin         = {16, 16};
    bool  m_enableInterpolation = true;
    bool  m_enableAlphaBlending = true;
    bool  m_flipImage           = false;
    bool  m_samplerMirror       = false;
    float m_uvScale             = 1.0f;
};
