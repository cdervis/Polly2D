#pragma once

#include "Demo.hpp"

class ShadersDemo final : public Demo
{
  public:
    explicit ShadersDemo(DemoBrowser* browser);

    void tick(GameTime time) override;

    void draw(GraphicsDevice gfx) override;

    void doImGui(ImGui imgui) override;

  private:
    void drawWobble(GraphicsDevice gfx);

    void drawGrayscale(GraphicsDevice gfx);

    void drawEdgeDetection(GraphicsDevice gfx);

    Image _logo;

    Shader _wobbleShader;
    Shader _grayscaleShader;
    Shader _edgeDetectionShader;

    int _shaderIndex = 0;

    bool  _verticalWobble      = false;
    float _waveFrequency       = 1.0f;
    float _waveSpeed           = 2.0f;
    float _waveAmplitude       = 0.5f;
    float _wobbleScale         = 1.0f;
    Color _wobbleColor         = white;
    float _grayscaleSaturation = 0.0f;
    float _edgeDetectionOffset = 1.0f;
};
