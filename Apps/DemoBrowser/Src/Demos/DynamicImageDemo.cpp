#include "DynamicImageDemo.hpp"

#include "DemoBrowser.hpp"

constexpr auto maxBrushRadius = 48;

DynamicImageDemo::DynamicImageDemo(DemoBrowser* browser)
    : Demo("Dynamic Image", browser)
{
    const auto windowSize = browser->window().sizePx();
    createImage(Vec2ui(u32(windowSize.x), u32(windowSize.y)));

    browser->window().setIsDisplaySyncEnabled(false);
}

DynamicImageDemo::~DynamicImageDemo()
{
    browser().window().setIsDisplaySyncEnabled(true);
}

void DynamicImageDemo::update(GameTime time)
{
}

void DynamicImageDemo::draw(Painter painter)
{
    painter.drawSprite(_image, Vec2(0, 0));
}

void DynamicImageDemo::onMouseMoved(const MouseMoveEvent& event)
{
    if (!isMouseButtonDown(MouseButton::Left))
    {
        return;
    }

    const auto viewSize = browser().window().size();

    auto center = Vec2(
        remap(0.0f, viewSize.x, 0.0f, _image.widthf(), event.position.x),
        remap(0.0f, viewSize.y, 0.0f, _image.heightf(), event.position.y));

    center -= Vec2(float(_brushRadius) / 2);

    const auto colorData = List<R8G8B8A8, 32>(_brushRadius * _brushRadius, R8G8B8A8(_brushColor));

    _image.updateData(
        u32(center.x),
        u32(center.y),
        _brushRadius,
        _brushRadius,
        colorData.data(),
        _image.supportsImmediateUpdate());
}

void DynamicImageDemo::onImGui(ImGui imgui)
{
    imgui.colorPicker("Brush Color", _brushColor, ImGuiColorEditOpts::PickerHueWheel);
    imgui.slider("Brush Radius", _brushRadius, 1, maxBrushRadius);

    if (imgui.button("Clear Image"))
    {
        _image.clear(black, _image.supportsImmediateUpdate());
    }
}

void DynamicImageDemo::createImage(Vec2ui size)
{
    const auto initialData = List<R8G8B8A8>(size.x * size.y, R8G8B8A8(black));

    _image = Image(
        ImageUsage::FrequentlyUpdatable,
        size.x,
        size.y,
        ImageFormat::R8G8B8A8UNorm,
        initialData.data());
}