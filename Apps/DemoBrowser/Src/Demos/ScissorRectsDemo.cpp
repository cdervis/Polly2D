#include "ScissorRectsDemo.hpp"

#include "DemoBrowser.hpp"

ScissorRectsDemo::ScissorRectsDemo(DemoBrowser* browser)
    : Demo("Scissor Rects", browser)
{
    _image = Image("logo256.png");
}

void ScissorRectsDemo::draw(Painter painter)
{
    const auto viewSize = painter.viewSize();

    // We'll draw an image in the center of the window.
    const auto imageRectangle = Rectangle((viewSize - _image.size()) / 2, _image.size());

    if (_isScissorEnabled)
    {
        const auto mousePos    = currentMousePosition() * painter.pixelRatio();
        auto       scissorRect = Rectangle(mousePos - _scissorSize / 2, _scissorSize);

        // Scissor rectangles must be within the window's viewport bounds.
        // This check is not done by Polly, it has to be done by us.
        // Luckily, this is easy by just using the clampedTo() method.
        scissorRect = scissorRect.clampedTo(Rectangle(Vec2(0, 0), viewSize));

        // Show a green-ish rectangle where the image will be drawn.
        painter.drawRectangle(imageRectangle, lime, painter.pixelRatio());

        // Draw a red rectangle that visualizes the scissor rectangle to the user.
        painter.drawRectangle(scissorRect, red, painter.pixelRatio());

        // Now apply the scissor rectangle for all subsequent drawings.
        painter.setScissorRects(Array{scissorRect});
    }

    painter.drawSprite(
        Sprite{
            .image   = _image,
            .dstRect = imageRectangle,
        });
}

void ScissorRectsDemo::onImGui(ImGui imgui)
{
    imgui.checkbox("Enable Scissor", _isScissorEnabled);
    imgui.slider("Scissor Size", _scissorSize, 1.0f, 300.0f, "%.1f");
}
