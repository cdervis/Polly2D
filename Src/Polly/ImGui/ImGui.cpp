// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/ImGui.hpp"

#include "Polly/Logging.hpp"
#include "Polly/MouseButton.hpp"

#include "Polly/ImGui/ImGuiImpl.hpp"

namespace Polly
{
static ImGuiWindowFlags convert(ImGuiWindowOpts value)
{
    return static_cast<int>(value);
}

static ImGuiFocusedFlags convert(ImGuiFocusedOpts value)
{
    return static_cast<int>(value);
}

static ImGuiHoveredFlags convert(ImGuiHoveredOpts value)
{
    return static_cast<int>(value);
}

static ImGuiCond convert(ImGuiCondition value)
{
    return static_cast<int>(value);
}

static ImGuiButtonFlags convert(ImGuiButtonOpts value)
{
    return static_cast<int>(value);
}

static ImGuiDir convert(Direction value)
{
    return static_cast<ImGuiDir>(static_cast<int>(value));
}

static ImGuiComboFlags convert(ImGuiComboOpts value)
{
    return static_cast<int>(value);
}

static ImGuiSliderFlags convert(ImGuiSliderOpts value)
{
    return static_cast<int>(value);
}

static ImGuiInputTextFlags convert(ImGuiInputTextOpts value)
{
    return static_cast<int>(value);
}

static ImGuiColorEditFlags convert(ImGuiColorEditOpts value)
{
    return static_cast<int>(value);
}

static ImGuiSelectableFlags convert(ImGuiSelectableOpts value)
{
    return static_cast<int>(value);
}

static ImGuiTableFlags convert(ImGuiTableOpts value)
{
    return static_cast<int>(value);
}

static ImGuiTreeNodeFlags convert(ImGuiTreeNodeOpts value)
{
    return static_cast<int>(value);
}

static ImGuiTableRowFlags convert(ImGuiTableRowOpts value)
{
    return static_cast<int>(value);
}

static ImGuiChildFlags convert(ImGuiChildOpts value)
{
    return static_cast<int>(value);
}

static ImGuiTableColumnFlags convert(ImGuiTableColumnOpts value)
{
    return static_cast<int>(value);
}

static ImGuiMouseButton convert(const MouseButton value)
{
    switch (value)
    {
        case MouseButton::Left: return ImGuiMouseButton_Left;
        case MouseButton::Right: return ImGuiMouseButton_Right;
        case MouseButton::Middle: return ImGuiMouseButton_Middle;
        case MouseButton::Extra1: break;
        case MouseButton::Extra2: break;
    }

    return ImGuiMouseButton_Left;
}

static ImGuiPopupFlags convert(ImGuiPopupOpts value)
{
    return static_cast<int>(value);
}

static Vec2 convert(ImVec2 value)
{
    return Vec2(value.x, value.y);
}

static ImVec2 convert(Vec2 value)
{
    return ImVec2(value.x, value.y);
}

static ImVec4 convert(const Color& value)
{
    return ImVec4(value.r, value.g, value.b, value.a);
}

PollyImplementObject(ImGui);

void ImGui::beginWindow(const StringView name, bool* isOpen, const ImGuiWindowOpts flags)
{
    PollyVerifyHaveImpl;
    ::ImGui::Begin(name.data(), isOpen, convert(flags));
}

void ImGui::endWindow()
{
    PollyVerifyHaveImpl;
    ::ImGui::End();
}

void ImGui::beginChildWindow(
    const StringView      id,
    const Maybe<Vec2>     size,
    const ImGuiChildOpts  childFlags,
    const ImGuiWindowOpts windowFlags)
{
    PollyVerifyHaveImpl;
    ::ImGui::BeginChild(id.data(), convert(size.valueOr(Vec2())), convert(childFlags), convert(windowFlags));
}

void ImGui::endChildWindow()
{
    PollyVerifyHaveImpl;
    ::ImGui::EndChild();
}

bool ImGui::isWindowAppearing()
{
    PollyVerifyHaveImpl;
    return ::ImGui::IsWindowAppearing();
}

bool ImGui::isWindowCollapsed()
{
    PollyVerifyHaveImpl;
    return ::ImGui::IsWindowCollapsed();
}

bool ImGui::isWindowFocused(ImGuiFocusedOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::IsWindowFocused(convert(flags));
}

bool ImGui::isWindowHovered(ImGuiHoveredOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::IsWindowHovered(convert(flags));
}

Vec2 ImGui::windowPosition()
{
    PollyVerifyHaveImpl;
    return convert(::ImGui::GetWindowPos());
}

Vec2 ImGui::windowSize()
{
    PollyVerifyHaveImpl;
    return convert(::ImGui::GetWindowSize());
}

float ImGui::windowWidth()
{
    PollyVerifyHaveImpl;
    return ::ImGui::GetWindowWidth();
}

float ImGui::windowHeight()
{
    PollyVerifyHaveImpl;
    return ::ImGui::GetWindowHeight();
}

void ImGui::setNextWindowPosition(Vec2 position, ImGuiCondition cond)
{
    PollyVerifyHaveImpl;
    ::ImGui::SetNextWindowPos(convert(position), convert(cond));
}

void ImGui::setNextWindowSize(Vec2 size, ImGuiCondition cond)
{
    PollyVerifyHaveImpl;
    ::ImGui::SetNextWindowSize(convert(size), convert(cond));
}

void ImGui::setNextWindowCollapsed(bool collapsed, ImGuiCondition cond)
{
    PollyVerifyHaveImpl;
    ::ImGui::SetNextWindowCollapsed(collapsed, convert(cond));
}

void ImGui::setWindowPosition(StringView name, Vec2 position, ImGuiCondition cond)
{
    PollyVerifyHaveImpl;
    ::ImGui::SetWindowPos(name.data(), convert(position), convert(cond));
}

void ImGui::setWindowSize(StringView name, Vec2 size, ImGuiCondition cond)
{
    PollyVerifyHaveImpl;
    ::ImGui::SetWindowSize(name.data(), convert(size), convert(cond));
}

void ImGui::setWindowCollapsed(StringView name, bool collapsed, ImGuiCondition cond)
{
    PollyVerifyHaveImpl;
    ::ImGui::SetWindowCollapsed(name.data(), collapsed, convert(cond));
}

void ImGui::setWindowFocus(StringView name)
{
    PollyVerifyHaveImpl;
    ::ImGui::SetWindowFocus(name.data());
}

void ImGui::separator()
{
    PollyVerifyHaveImpl;
    ::ImGui::Separator();
}

void ImGui::sameLine(float offsetFromStartX, float spacing)
{
    PollyVerifyHaveImpl;
    ::ImGui::SameLine(offsetFromStartX, spacing);
}

void ImGui::newLine()
{
    PollyVerifyHaveImpl;
    ::ImGui::NewLine();
}

void ImGui::spacing()
{
    PollyVerifyHaveImpl;
    ::ImGui::Spacing();
}

void ImGui::dummy(Vec2 size)
{
    PollyVerifyHaveImpl;
    ::ImGui::Dummy(convert(size));
}

void ImGui::indent(float indentW)
{
    PollyVerifyHaveImpl;
    ::ImGui::Indent(indentW);
}

void ImGui::unindent(float indentW)
{
    PollyVerifyHaveImpl;
    ::ImGui::Unindent(indentW);
}

void ImGui::beginGroup()
{
    PollyVerifyHaveImpl;
    ::ImGui::BeginGroup();
}

void ImGui::endGroup()
{
    PollyVerifyHaveImpl;
    ::ImGui::EndGroup();
}

void ImGui::alignTextToFramePadding()
{
    PollyVerifyHaveImpl;
    ::ImGui::AlignTextToFramePadding();
}

float ImGui::textLineHeight()
{
    PollyVerifyHaveImpl;
    return ::ImGui::GetTextLineHeight();
}

float ImGui::textLineHeightWithSpacing()
{
    PollyVerifyHaveImpl;
    return ::ImGui::GetTextLineHeightWithSpacing();
}

float ImGui::frameHeight()
{
    PollyVerifyHaveImpl;
    return ::ImGui::GetFrameHeight();
}

float ImGui::frameHeightWithSpacing()
{
    PollyVerifyHaveImpl;
    return ::ImGui::GetFrameHeightWithSpacing();
}

void ImGui::textUnformatted(StringView text)
{
    PollyVerifyHaveImpl;
    ::ImGui::TextUnformatted(text.data());
}

void ImGui::text(StringView fmt, ...)
{
    PollyVerifyHaveImpl;
    va_list args{};
    va_start(args, fmt);
    ::ImGui::TextV(fmt.data(), args);
    va_end(args);
}

void ImGui::textColored(Color color, StringView fmt, ...)
{
    PollyVerifyHaveImpl;
    va_list args{};
    va_start(args, fmt);
    ::ImGui::TextColoredV(convert(color), fmt.data(), args);
    va_end(args);
}

void ImGui::textDisabled(StringView fmt, ...)
{
    PollyVerifyHaveImpl;
    va_list args{};
    va_start(args, fmt);
    ::ImGui::TextDisabledV(fmt.data(), args);
    va_end(args);
}

void ImGui::textWrapped(StringView fmt, ...)
{
    PollyVerifyHaveImpl;
    va_list args{};
    va_start(args, fmt);
    ::ImGui::TextWrappedV(fmt.data(), args);
    va_end(args);
}

void ImGui::labelText(StringView label, StringView fmt, ...)
{
    PollyVerifyHaveImpl;
    va_list args{};
    va_start(args, fmt);
    ::ImGui::LabelTextV(label.data(), fmt.data(), args);
    va_end(args);
}

void ImGui::bulletText(StringView fmt, ...)
{
    PollyVerifyHaveImpl;
    va_list args{};
    va_start(args, fmt);
    ::ImGui::BulletTextV(fmt.data(), args);
    va_end(args);
}

void ImGui::separatorWithText(StringView label)
{
    PollyVerifyHaveImpl;
    ::ImGui::SeparatorText(label.data());
}

bool ImGui::button(StringView label, Maybe<Vec2> size)
{
    PollyVerifyHaveImpl;
    return ::ImGui::Button(label.data(), convert(size.valueOr(Vec2())));
}

bool ImGui::smallButton(StringView label)
{
    PollyVerifyHaveImpl;
    return ::ImGui::SmallButton(label.data());
}

bool ImGui::invisibleButton(StringView id, Vec2 size, ImGuiButtonOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::InvisibleButton(id.data(), convert(size), convert(flags));
}

bool ImGui::arrowButton(StringView id, Direction direction)
{
    PollyVerifyHaveImpl;
    return ::ImGui::ArrowButton(id.data(), convert(direction));
}

bool ImGui::checkbox(StringView label, bool& isChecked)
{
    PollyVerifyHaveImpl;
    return ::ImGui::Checkbox(label.data(), &isChecked);
}

bool ImGui::checkboxFlags(StringView label, int& flags, int flagsValue)
{
    PollyVerifyHaveImpl;
    return ::ImGui::CheckboxFlags(label.data(), &flags, flagsValue);
}

bool ImGui::radioButton(StringView label, bool isActive)
{
    PollyVerifyHaveImpl;
    return ::ImGui::RadioButton(label.data(), isActive);
}

bool ImGui::radioButton(StringView label, int& value, int valueButton)
{
    PollyVerifyHaveImpl;
    return ::ImGui::RadioButton(label.data(), &value, valueButton);
}

void ImGui::progressBar(float fraction, Maybe<Vec2> size, StringView overlay)
{
    PollyVerifyHaveImpl;
    ::ImGui::ProgressBar(fraction, convert(size.valueOr(Vec2())), overlay.data());
}

void ImGui::bullet()
{
    PollyVerifyHaveImpl;
    ::ImGui::Bullet();
}

bool ImGui::textLink(StringView label)
{
    PollyVerifyHaveImpl;
    return ::ImGui::TextLink(label.data());
}

void ImGui::textLinkOpenUrl(StringView label, StringView url)
{
    PollyVerifyHaveImpl;
    ::ImGui::TextLinkOpenURL(label.data(), url.data());
}

void ImGui::image(
    [[maybe_unused]] Image image,
    [[maybe_unused]] Vec2  imageSize,
    [[maybe_unused]] Vec2  uv0,
    [[maybe_unused]] Vec2  uv1,
    [[maybe_unused]] Color tintColor,
    [[maybe_unused]] Color borderColor)
{
    notImplemented();
}

void ImGui::imageButton(
    [[maybe_unused]] StringView id,
    [[maybe_unused]] Image      image,
    [[maybe_unused]] Vec2       imageSize,
    [[maybe_unused]] Vec2       uv0,
    [[maybe_unused]] Vec2       uv1,
    [[maybe_unused]] Color      backgroundColor,
    [[maybe_unused]] Color      borderColor)
{
    notImplemented();
}

bool ImGui::beginCombo(StringView label, StringView previewValue, ImGuiComboOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::BeginCombo(label.data(), previewValue.data(), convert(flags));
}

void ImGui::endCombo()
{
    PollyVerifyHaveImpl;
    ::ImGui::EndCombo();
}

bool ImGui::combo(
    StringView       label,
    int&             currentItem,
    Span<StringView> items,
    Maybe<u32>       popupMaxHeightInItems)
{
    PollyDeclareThisImpl;

    const auto height = popupMaxHeightInItems.valueOr(-1);

    auto stringPtrList = Impl::StringPtrList();
    stringPtrList.ptrs.reserve(items.size());

    for (const auto& item : items)
    {
        const auto& insertedRef = impl->tmpStrings.emplace(item);
        stringPtrList.ptrs.add(insertedRef.cstring());
    }

    impl->tmpStringPtrLists.add(std::move(stringPtrList));

    return ::ImGui::Combo(
        label.data(),
        &currentItem,
        impl->tmpStringPtrLists.last().ptrs.data(),
        static_cast<int>(items.size()),
        static_cast<int>(height));
}

bool ImGui::drag(
    StringView      label,
    float&          value,
    float           speed,
    float           min,
    float           max,
    StringView      format,
    ImGuiSliderOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::DragFloat(label.data(), &value, speed, min, max, format.data(), convert(flags));
}

bool ImGui::drag(
    StringView      label,
    Vec2&           value,
    float           speed,
    float           min,
    float           max,
    StringView      format,
    ImGuiSliderOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::DragFloat2(label.data(), &value.x, speed, min, max, format.data(), convert(flags));
}

bool ImGui::drag(
    StringView      label,
    Vec3&           value,
    float           speed,
    float           min,
    float           max,
    StringView      format,
    ImGuiSliderOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::DragFloat3(label.data(), &value.x, speed, min, max, format.data(), convert(flags));
}

bool ImGui::drag(
    StringView      label,
    Vec4&           value,
    float           speed,
    float           min,
    float           max,
    StringView      format,
    ImGuiSliderOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::DragFloat4(label.data(), &value.x, speed, min, max, format.data(), convert(flags));
}

bool ImGui::drag(
    StringView      label,
    int&            value,
    float           speed,
    int             min,
    int             max,
    StringView      format,
    ImGuiSliderOpts flags) const
{
    PollyVerifyHaveImpl;
    return ::ImGui::DragInt(label.data(), &value, speed, min, max, format.data(), convert(flags));
}

bool ImGui::drag(
    StringView      label,
    Vec2i&          value,
    float           speed,
    int             min,
    int             max,
    StringView      format,
    ImGuiSliderOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::DragInt2(label.data(), &value.x, speed, min, max, format.data(), convert(flags));
}

bool ImGui::drag(
    StringView      label,
    Vec3i&          value,
    float           speed,
    int             min,
    int             max,
    StringView      format,
    ImGuiSliderOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::DragInt3(label.data(), &value.x, speed, min, max, format.data(), convert(flags));
}

bool ImGui::drag(
    StringView      label,
    Vec4i&          value,
    float           speed,
    int             min,
    int             max,
    StringView      format,
    ImGuiSliderOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::DragInt4(label.data(), &value.x, speed, min, max, format.data(), convert(flags));
}

bool ImGui::slider(
    StringView      label,
    float&          value,
    float           min,
    float           max,
    StringView      format,
    ImGuiSliderOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::SliderFloat(label.data(), &value, min, max, format.data(), convert(flags));
}

bool ImGui::slider(
    StringView      label,
    Vec2&           value,
    float           min,
    float           max,
    StringView      format,
    ImGuiSliderOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::SliderFloat2(label.data(), &value.x, min, max, format.data(), convert(flags));
}

bool ImGui::slider(
    StringView      label,
    Vec3&           value,
    float           min,
    float           max,
    StringView      format,
    ImGuiSliderOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::SliderFloat3(label.data(), &value.x, min, max, format.data(), convert(flags));
}

bool ImGui::slider(
    StringView      label,
    Vec4&           value,
    float           min,
    float           max,
    StringView      format,
    ImGuiSliderOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::SliderFloat4(label.data(), &value.x, min, max, format.data(), convert(flags));
}

bool ImGui::sliderAngle(
    StringView      label,
    Degrees&        value,
    Degrees         min,
    Degrees         max,
    StringView      format,
    ImGuiSliderOpts flags)
{
    PollyVerifyHaveImpl;

    return ::ImGui::SliderAngle(
        label.data(),
        &value.value,
        min.value,
        max.value,
        format.data(),
        convert(flags));
}

bool ImGui::slider(StringView label, int& value, int min, int max, StringView format, ImGuiSliderOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::SliderInt(label.data(), &value, min, max, format.data(), convert(flags));
}

bool ImGui::slider(StringView label, Vec2i& value, int min, int max, StringView format, ImGuiSliderOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::SliderInt2(label.data(), &value.x, min, max, format.data(), convert(flags));
}

bool ImGui::slider(StringView label, Vec3i& value, int min, int max, StringView format, ImGuiSliderOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::SliderInt3(label.data(), &value.x, min, max, format.data(), convert(flags));
}

bool ImGui::slider(StringView label, Vec4i& value, int min, int max, StringView format, ImGuiSliderOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::SliderInt4(label.data(), &value.x, min, max, format.data(), convert(flags));
}

bool ImGui::sliderVertical(
    StringView      label,
    Vec2            size,
    float&          value,
    float           min,
    float           max,
    StringView      format,
    ImGuiSliderOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::VSliderFloat(
        label.data(),
        convert(size),
        &value,
        min,
        max,
        format.data(),
        convert(flags));
}

bool ImGui::sliderVertical(
    StringView      label,
    Vec2            size,
    int&            value,
    int             min,
    int             max,
    StringView      format,
    ImGuiSliderOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::VSliderInt(label.data(), convert(size), &value, min, max, format.data(), convert(flags));
}

bool ImGui::inputText(StringView label, String& value, ImGuiInputTextOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::InputText(label.data(), value.data(), value.capacity(), convert(flags));
}

bool ImGui::inputTextMultiline(StringView label, String& value, Maybe<Vec2> size, ImGuiInputTextOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::InputTextMultiline(
        label.data(),
        value.data(),
        value.size(),
        convert(size.valueOr(Vec2())),
        convert(flags));
}

bool ImGui::inputTextWithHint(StringView label, StringView hint, String& value, ImGuiInputTextOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::InputTextWithHint(label.data(), hint.data(), value.data(), value.size(), convert(flags));
}

bool ImGui::input(
    StringView         label,
    float&             value,
    float              step,
    float              stepFast,
    StringView         format,
    ImGuiInputTextOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::InputFloat(label.data(), &value, step, stepFast, format.data(), convert(flags));
}

bool ImGui::input(StringView label, Vec2& value, StringView format, ImGuiInputTextOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::InputFloat2(label.data(), &value.x, format.data(), convert(flags));
}

bool ImGui::input(StringView label, Vec3& value, StringView format, ImGuiInputTextOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::InputFloat3(label.data(), &value.x, format.data(), convert(flags));
}

bool ImGui::input(StringView label, Vec4& value, StringView format, ImGuiInputTextOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::InputFloat4(label.data(), &value.x, format.data(), convert(flags));
}

bool ImGui::input(StringView label, int& value, int step, int stepFast, ImGuiInputTextOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::InputInt(label.data(), &value, step, stepFast, convert(flags));
}

bool ImGui::input(StringView label, Vec2i& value, ImGuiInputTextOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::InputInt2(label.data(), &value.x, convert(flags));
}

bool ImGui::input(StringView label, Vec3i& value, ImGuiInputTextOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::InputInt3(label.data(), &value.x, convert(flags));
}

bool ImGui::input(StringView label, Vec4i& value, ImGuiInputTextOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::InputInt4(label.data(), &value.x, convert(flags));
}

bool ImGui::colorEdit(StringView label, Color& value, ImGuiColorEditOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::ColorEdit4(label.data(), &value.r, convert(flags));
}

bool ImGui::colorEditRgb(StringView label, Color& value, ImGuiColorEditOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::ColorEdit3(label.data(), &value.r, convert(flags));
}

bool ImGui::colorPicker(StringView label, Color& value, ImGuiColorEditOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::ColorPicker4(label.data(), &value.r, convert(flags));
}

bool ImGui::colorPickerRgb(StringView label, Color& value, ImGuiColorEditOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::ColorPicker3(label.data(), &value.r, convert(flags));
}

bool ImGui::colorButton(StringView id, Color color, ImGuiColorEditOpts flags, Maybe<Vec2> size)
{
    PollyVerifyHaveImpl;
    return ::ImGui::ColorButton(id.data(), convert(color), convert(flags), convert(size.valueOr(Vec2())));
}

bool ImGui::treeNode(StringView label)
{
    PollyVerifyHaveImpl;
    return ::ImGui::TreeNode(label.data());
}

bool ImGui::treeNode(StringView id, StringView fmt, ...)
{
    PollyVerifyHaveImpl;

    va_list args{};
    va_start(args, fmt);
    const auto result = ::ImGui::TreeNodeV(id.data(), fmt.data(), args);
    va_end(args);

    return result;
}

bool ImGui::treeNodeEx(StringView id, ImGuiTreeNodeOpts flags, StringView fmt, ...)
{
    PollyVerifyHaveImpl;

    va_list args{};
    va_start(args, fmt);
    const auto result = ::ImGui::TreeNodeExV(id.data(), convert(flags), fmt.data(), args);
    va_end(args);

    return result;
}

void ImGui::treePush(const void* id)
{
    PollyVerifyHaveImpl;
    ::ImGui::TreePush(id);
}

void ImGui::treePop()
{
    PollyVerifyHaveImpl;
    ::ImGui::TreePop();
}

float ImGui::treeNodeToLabelSpacing()
{
    PollyVerifyHaveImpl;
    return ::ImGui::GetTreeNodeToLabelSpacing();
}

bool ImGui::collapsingHeader(StringView label, ImGuiTreeNodeOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::CollapsingHeader(label.data(), convert(flags));
}

bool ImGui::collapsingHeader(StringView label, bool& isVisible, ImGuiTreeNodeOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::CollapsingHeader(label.data(), &isVisible, convert(flags));
}

void ImGui::setNextItemOpen(bool isOpen, ImGuiCondition cond)
{
    PollyVerifyHaveImpl;
    ::ImGui::SetNextItemOpen(isOpen, convert(cond));
}

bool ImGui::selectable(StringView label, bool isSelected, ImGuiSelectableOpts flags, Maybe<Vec2> size)
{
    PollyVerifyHaveImpl;
    return ::ImGui::Selectable(label.data(), isSelected, convert(flags), convert(size.valueOr(Vec2())));
}

bool ImGui::selectableWithBinding(
    StringView          label,
    bool&               isSelected,
    ImGuiSelectableOpts flags,
    Maybe<Vec2>         size)
{
    PollyVerifyHaveImpl;
    return ::ImGui::Selectable(label.data(), &isSelected, convert(flags), convert(size.valueOr(Vec2())));
}

bool ImGui::beginListBox(StringView label, Maybe<Vec2> size)
{
    PollyVerifyHaveImpl;
    return ::ImGui::BeginListBox(label.data(), convert(size.valueOr(Vec2())));
}

void ImGui::endListBox()
{
    PollyVerifyHaveImpl;
    ::ImGui::EndListBox();
}

bool ImGui::listBox(StringView label, int& currentItem, Span<StringView> items, Maybe<u32> heightInItems)
{
    PollyDeclareThisImpl;

    const auto height = heightInItems.valueOr(-1);

    auto stringPtrList = Impl::StringPtrList();
    stringPtrList.ptrs.reserve(items.size());

    for (const auto& item : items)
    {
        const auto& insertedRef = impl->tmpStrings.emplace(item);
        stringPtrList.ptrs.add(insertedRef.cstring());
    }

    impl->tmpStringPtrLists.add(std::move(stringPtrList));

    return ::ImGui::ListBox(
        label.data(),
        &currentItem,
        impl->tmpStringPtrLists.last().ptrs.data(),
        static_cast<int>(items.size()),
        static_cast<int>(height));
}

void ImGui::value(StringView prefix, bool value)
{
    PollyVerifyHaveImpl;
    ::ImGui::Value(prefix.data(), value);
}

void ImGui::value(StringView prefix, int value)
{
    PollyVerifyHaveImpl;
    ::ImGui::Value(prefix.data(), value);
}

void ImGui::value(StringView prefix, unsigned int value)
{
    PollyVerifyHaveImpl;
    ::ImGui::Value(prefix.data(), value);
}

void ImGui::value(StringView prefix, float value, StringView floatFormat)
{
    PollyVerifyHaveImpl;
    ::ImGui::Value(prefix.data(), value, floatFormat.data());
}

bool ImGui::beginMenuBar()
{
    PollyVerifyHaveImpl;
    return ::ImGui::BeginMenuBar();
}

void ImGui::endMenuBar()
{
    PollyVerifyHaveImpl;
    ::ImGui::EndMenuBar();
}

bool ImGui::beginMainMenuBar()
{
    PollyVerifyHaveImpl;
    return ::ImGui::BeginMainMenuBar();
}

void ImGui::endMainMenuBar()
{
    PollyVerifyHaveImpl;
    ::ImGui::EndMainMenuBar();
}

bool ImGui::beginMenu(StringView label, bool enabled)
{
    PollyVerifyHaveImpl;
    return ::ImGui::BeginMenu(label.data(), enabled);
}

void ImGui::endMenu()
{
    PollyVerifyHaveImpl;
    ::ImGui::EndMenu();
}

bool ImGui::menuItem(StringView label, StringView shortcut, bool selected, bool enabled)
{
    PollyVerifyHaveImpl;
    return ::ImGui::MenuItem(label.data(), shortcut.data(), selected, enabled);
}

bool ImGui::menuItemWithBinding(StringView label, StringView shortcut, bool& isSelected, bool enabled)
{
    PollyVerifyHaveImpl;
    return ::ImGui::MenuItem(label.data(), shortcut.data(), &isSelected, enabled);
}

bool ImGui::beginTooltip()
{
    PollyVerifyHaveImpl;
    return ::ImGui::BeginTooltip();
}

void ImGui::endTooltip()
{
    PollyVerifyHaveImpl;
    ::ImGui::EndTooltip();
}

void ImGui::setTooltip(StringView fmt, ...)
{
    PollyVerifyHaveImpl;

    va_list args{};
    va_start(args, fmt);
    ::ImGui::SetTooltipV(fmt.data(), args);
    va_end(args);
}

bool ImGui::beginItemTooltip()
{
    PollyVerifyHaveImpl;
    return ::ImGui::BeginItemTooltip();
}

void ImGui::setItemTooltip(StringView fmt, ...)
{
    PollyVerifyHaveImpl;

    va_list args{};
    va_start(args, fmt);
    ::ImGui::SetItemTooltipV(fmt.data(), args);
    va_end(args);
}

bool ImGui::beginPopup(StringView id, ImGuiWindowOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::BeginPopup(id.data(), convert(flags));
}

bool ImGui::beginPopupModal(StringView name, bool& isOpen, ImGuiWindowOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::BeginPopupModal(name.data(), &isOpen, convert(flags));
}

void ImGui::endPopup()
{
    PollyVerifyHaveImpl;
    ::ImGui::EndPopup();
}

void ImGui::openPopup(StringView id, ImGuiPopupOpts flags)
{
    PollyVerifyHaveImpl;
    ::ImGui::OpenPopup(id.data(), convert(flags));
}

void ImGui::openPopupOnItemClick(StringView id, ImGuiPopupOpts flags)
{
    PollyVerifyHaveImpl;
    ::ImGui::OpenPopupOnItemClick(id.data(), convert(flags));
}

void ImGui::closeCurrentPopup()
{
    PollyVerifyHaveImpl;
    ::ImGui::CloseCurrentPopup();
}

bool ImGui::beginPopupContextItem(StringView id, ImGuiPopupOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::BeginPopupContextItem(id.data(), convert(flags));
}

bool ImGui::beginPopupContextWindow(StringView id, ImGuiPopupOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::BeginPopupContextWindow(id.data(), convert(flags));
}

bool ImGui::beginPopupContextVoid(StringView id, ImGuiPopupOpts flags)
{
    PollyVerifyHaveImpl;
    return ::ImGui::BeginPopupContextVoid(id.data(), convert(flags));
}

bool ImGui::isPopupOpen(StringView id, ImGuiPopupOpts flags) const
{
    PollyVerifyHaveImpl;
    return ::ImGui::IsPopupOpen(id.data(), convert(flags));
}

bool ImGui::beginTable(
    StringView     id,
    u32            columns,
    ImGuiTableOpts flags,
    Maybe<Vec2>    outerSize,
    Maybe<float>   innerWidth)
{
    PollyVerifyHaveImpl;

    return ::ImGui::BeginTable(
        id.data(),
        static_cast<int>(columns),
        convert(flags),
        convert(outerSize.valueOr(Vec2())),
        innerWidth.valueOr(0.0f));
}

void ImGui::endTable()
{
    PollyVerifyHaveImpl;
    ::ImGui::EndTable();
}

void ImGui::tableNextRow(ImGuiTableRowOpts flags, Maybe<float> minRowHeight)
{
    PollyVerifyHaveImpl;
    ::ImGui::TableNextRow(convert(flags), minRowHeight.valueOr(0.0f));
}

bool ImGui::tableNextColumn()
{
    PollyVerifyHaveImpl;
    return ::ImGui::TableNextColumn();
}

bool ImGui::tableSetColumnIndex(int column)
{
    PollyVerifyHaveImpl;
    return ::ImGui::TableSetColumnIndex(column);
}

void ImGui::tableSetupColumn(StringView label, ImGuiTableColumnOpts flags)
{
    PollyVerifyHaveImpl;
    ::ImGui::TableSetupColumn(label.data(), convert(flags));
}

void ImGui::tableSetupScrollFreeze(u32 cols, u32 rows)
{
    PollyVerifyHaveImpl;
    ::ImGui::TableSetupScrollFreeze(static_cast<int>(cols), static_cast<int>(rows));
}

void ImGui::tableHeader(StringView label)
{
    PollyVerifyHaveImpl;
    ::ImGui::TableHeader(label.data());
}

void ImGui::tableHeadersRow()
{
    PollyVerifyHaveImpl;
    ::ImGui::TableHeadersRow();
}

void ImGui::tableAngledHeadersRow()
{
    PollyVerifyHaveImpl;
    ::ImGui::TableAngledHeadersRow();
}

int ImGui::tableColumnCount() const
{
    PollyVerifyHaveImpl;
    return ::ImGui::TableGetColumnCount();
}

int ImGui::tableColumnIndex() const
{
    PollyVerifyHaveImpl;
    return ::ImGui::TableGetColumnIndex();
}

int ImGui::tableRowIndex() const
{
    PollyVerifyHaveImpl;
    return ::ImGui::TableGetRowIndex();
}

StringView ImGui::tableColumnName(Maybe<int> column) const
{
    PollyVerifyHaveImpl;
    return ::ImGui::TableGetColumnName(column.valueOr(-1));
}

void ImGui::setItemDefaultFocus()
{
    PollyVerifyHaveImpl;
    ::ImGui::SetItemDefaultFocus();
}

void ImGui::setNavCursorVisible(bool value)
{
    PollyVerifyHaveImpl;
    ::ImGui::SetNavCursorVisible(value);
}

void ImGui::setNextItemAllowOverlap()
{
    PollyVerifyHaveImpl;
    ::ImGui::SetNextItemAllowOverlap();
}

bool ImGui::isItemHovered(ImGuiHoveredOpts flags) const
{
    PollyVerifyHaveImpl;
    return ::ImGui::IsItemHovered(convert(flags));
}

bool ImGui::isItemActive() const
{
    PollyVerifyHaveImpl;
    return ::ImGui::IsItemActive();
}

bool ImGui::isItemFocused() const
{
    PollyVerifyHaveImpl;
    return ::ImGui::IsItemFocused();
}

bool ImGui::isItemClicked(MouseButton button) const
{
    PollyVerifyHaveImpl;
    return ::ImGui::IsItemClicked(convert(button));
}

bool ImGui::isItemVisible() const
{
    PollyVerifyHaveImpl;
    return ::ImGui::IsItemVisible();
}

bool ImGui::isItemEdited() const
{
    PollyVerifyHaveImpl;
    return ::ImGui::IsItemEdited();
}

bool ImGui::isItemActivated() const
{
    PollyVerifyHaveImpl;
    return ::ImGui::IsItemActivated();
}

bool ImGui::isItemDeactivated() const
{
    PollyVerifyHaveImpl;
    return ::ImGui::IsItemDeactivated();
}

bool ImGui::isItemDeactivatedAfterEdit() const
{
    PollyVerifyHaveImpl;
    return ::ImGui::IsItemDeactivatedAfterEdit();
}

bool ImGui::isItemToggledOpen() const
{
    PollyVerifyHaveImpl;
    return ::ImGui::IsItemToggledOpen();
}

bool ImGui::isAnyItemHovered() const
{
    PollyVerifyHaveImpl;
    return ::ImGui::IsAnyItemHovered();
}

bool ImGui::isAnyItemActive() const
{
    PollyVerifyHaveImpl;
    return ::ImGui::IsAnyItemActive();
}

bool ImGui::isAnyItemFocused() const
{
    PollyVerifyHaveImpl;
    return ::ImGui::IsAnyItemFocused();
}

Rectangle ImGui::itemRect() const
{
    PollyVerifyHaveImpl;

    const auto topLeft = ::ImGui::GetItemRectMin();
    const auto size    = ::ImGui::GetItemRectSize();

    return Rectangle(convert(topLeft), convert(size));
}
} // namespace Polly