// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

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

static ImGuiInputTextFlags convert(imgui_input_text_opts value)
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

static ImGuiMouseButton convert(MouseButton value)
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

pl_implement_object(ImGui);

void ImGui::beginWindow(StringView name, bool* is_open, ImGuiWindowOpts flags)
{
    verifyHaveImpl;
    ::ImGui::Begin(name.data(), is_open, convert(flags));
}

void ImGui::endWindow()
{
    verifyHaveImpl;
    ::ImGui::End();
}

void ImGui::beginChildWindow(
    StringView      id,
    Maybe<Vec2>     size,
    ImGuiChildOpts  child_flags,
    ImGuiWindowOpts window_flags)
{
    verifyHaveImpl;
    ::ImGui::BeginChild(
        id.data(),
        convert(size.valueOr(Vec2())),
        convert(child_flags),
        convert(window_flags));
}

void ImGui::endChildWindow()
{
    verifyHaveImpl;
    ::ImGui::EndChild();
}

bool ImGui::isWindowAppearing()
{
    verifyHaveImpl;
    return ::ImGui::IsWindowAppearing();
}

bool ImGui::isWindowCollapsed()
{
    verifyHaveImpl;
    return ::ImGui::IsWindowCollapsed();
}

bool ImGui::isWindowFocused(ImGuiFocusedOpts flags)
{
    verifyHaveImpl;
    return ::ImGui::IsWindowFocused(convert(flags));
}

bool ImGui::isWindowHovered(ImGuiHoveredOpts flags)
{
    verifyHaveImpl;
    return ::ImGui::IsWindowHovered(convert(flags));
}

Vec2 ImGui::windowPosition()
{
    verifyHaveImpl;
    return convert(::ImGui::GetWindowPos());
}

Vec2 ImGui::windowSize()
{
    verifyHaveImpl;
    return convert(::ImGui::GetWindowSize());
}

float ImGui::windowWidth()
{
    verifyHaveImpl;
    return ::ImGui::GetWindowWidth();
}

float ImGui::windowHeight()
{
    verifyHaveImpl;
    return ::ImGui::GetWindowHeight();
}

void ImGui::setNextWindowPosition(Vec2 position, ImGuiCondition cond)
{
    verifyHaveImpl;
    ::ImGui::SetNextWindowPos(convert(position), convert(cond));
}

void ImGui::setNextWindowSize(Vec2 size, ImGuiCondition cond)
{
    verifyHaveImpl;
    ::ImGui::SetNextWindowSize(convert(size), convert(cond));
}

void ImGui::setNextWindowCollapsed(bool collapsed, ImGuiCondition cond)
{
    verifyHaveImpl;
    ::ImGui::SetNextWindowCollapsed(collapsed, convert(cond));
}

void ImGui::setWindowPosition(StringView name, Vec2 position, ImGuiCondition cond)
{
    verifyHaveImpl;
    ::ImGui::SetWindowPos(name.data(), convert(position), convert(cond));
}

void ImGui::setWindowSize(StringView name, Vec2 size, ImGuiCondition cond)
{
    verifyHaveImpl;
    ::ImGui::SetWindowSize(name.data(), convert(size), convert(cond));
}

void ImGui::setWindowCollapsed(StringView name, bool collapsed, ImGuiCondition cond)
{
    verifyHaveImpl;
    ::ImGui::SetWindowCollapsed(name.data(), collapsed, convert(cond));
}

void ImGui::setWindowFocus(StringView name)
{
    verifyHaveImpl;
    ::ImGui::SetWindowFocus(name.data());
}

void ImGui::separator()
{
    verifyHaveImpl;
    ::ImGui::Separator();
}

void ImGui::sameLine(float offset_from_start_x, float spacing)
{
    verifyHaveImpl;
    ::ImGui::SameLine(offset_from_start_x, spacing);
}

void ImGui::newLine()
{
    verifyHaveImpl;
    ::ImGui::NewLine();
}

void ImGui::spacing()
{
    verifyHaveImpl;
    ::ImGui::Spacing();
}

void ImGui::dummy(Vec2 size)
{
    verifyHaveImpl;
    ::ImGui::Dummy(convert(size));
}

void ImGui::indent(float indent_w)
{
    verifyHaveImpl;
    ::ImGui::Indent(indent_w);
}

void ImGui::unindent(float indent_w)
{
    verifyHaveImpl;
    ::ImGui::Unindent(indent_w);
}

void ImGui::beginGroup()
{
    verifyHaveImpl;
    ::ImGui::BeginGroup();
}

void ImGui::endGroup()
{
    verifyHaveImpl;
    ::ImGui::EndGroup();
}

void ImGui::alignTextToFramePadding()
{
    verifyHaveImpl;
    ::ImGui::AlignTextToFramePadding();
}

float ImGui::textLineHeight()
{
    verifyHaveImpl;
    return ::ImGui::GetTextLineHeight();
}

float ImGui::textLineHeightWithSpacing()
{
    verifyHaveImpl;
    return ::ImGui::GetTextLineHeightWithSpacing();
}

float ImGui::frameHeight()
{
    verifyHaveImpl;
    return ::ImGui::GetFrameHeight();
}

float ImGui::frameHeightWithSpacing()
{
    verifyHaveImpl;
    return ::ImGui::GetFrameHeightWithSpacing();
}

void ImGui::textUnformatted(StringView text)
{
    verifyHaveImpl;
    ::ImGui::TextUnformatted(text.data());
}

void ImGui::text(StringView fmt, ...)
{
    verifyHaveImpl;
    va_list args;
    va_start(args, fmt);
    ::ImGui::TextV(fmt.data(), args);
    va_end(args);
}

void ImGui::textColored(Color color, StringView fmt, ...)
{
    verifyHaveImpl;
    va_list args;
    va_start(args, fmt);
    ::ImGui::TextColoredV(convert(color), fmt.data(), args);
    va_end(args);
}

void ImGui::textDisabled(StringView fmt, ...)
{
    verifyHaveImpl;
    va_list args;
    va_start(args, fmt);
    ::ImGui::TextDisabledV(fmt.data(), args);
    va_end(args);
}

void ImGui::textWrapped(StringView fmt, ...)
{
    verifyHaveImpl;
    va_list args;
    va_start(args, fmt);
    ::ImGui::TextWrappedV(fmt.data(), args);
    va_end(args);
}

void ImGui::labelText(StringView label, StringView fmt, ...)
{
    verifyHaveImpl;
    va_list args;
    va_start(args, fmt);
    ::ImGui::LabelTextV(label.data(), fmt.data(), args);
    va_end(args);
}

void ImGui::bulletText(StringView fmt, ...)
{
    verifyHaveImpl;
    va_list args;
    va_start(args, fmt);
    ::ImGui::BulletTextV(fmt.data(), args);
    va_end(args);
}

void ImGui::separatorWithText(StringView label)
{
    verifyHaveImpl;
    ::ImGui::SeparatorText(label.data());
}

auto ImGui::button(StringView label, Maybe<Vec2> size) -> bool
{
    verifyHaveImpl;
    return ::ImGui::Button(label.data(), convert(size.valueOr(Vec2())));
}

auto ImGui::smallButton(StringView label) -> bool
{
    verifyHaveImpl;
    return ::ImGui::SmallButton(label.data());
}

auto ImGui::invisibleButton(StringView id, Vec2 size, ImGuiButtonOpts flags) -> bool
{
    verifyHaveImpl;
    return ::ImGui::InvisibleButton(id.data(), convert(size), convert(flags));
}

auto ImGui::arrowButton(StringView id, Direction direction) -> bool
{
    verifyHaveImpl;
    return ::ImGui::ArrowButton(id.data(), convert(direction));
}

bool ImGui::checkbox(StringView label, bool& is_checked)
{
    verifyHaveImpl;
    return ::ImGui::Checkbox(label.data(), &is_checked);
}

auto ImGui::checkboxFlags(StringView label, int& flags, int flags_value) -> bool
{
    verifyHaveImpl;
    return ::ImGui::CheckboxFlags(label.data(), &flags, flags_value);
}

auto ImGui::radioButton(StringView label, bool isActive) -> bool
{
    verifyHaveImpl;
    return ::ImGui::RadioButton(label.data(), isActive);
}

bool ImGui::radioButton(StringView label, int& value, int value_button)
{
    verifyHaveImpl;
    return ::ImGui::RadioButton(label.data(), &value, value_button);
}

void ImGui::progressBar(float fraction, Maybe<Vec2> size, StringView overlay)
{
    verifyHaveImpl;
    ::ImGui::ProgressBar(fraction, convert(size.valueOr(Vec2())), overlay.data());
}

void ImGui::bullet()
{
    verifyHaveImpl;
    ::ImGui::Bullet();
}

auto ImGui::textLink(StringView label) -> bool
{
    verifyHaveImpl;
    return ::ImGui::TextLink(label.data());
}

void ImGui::textLinkOpenUrl(StringView label, StringView url)
{
    verifyHaveImpl;
    ::ImGui::TextLinkOpenURL(label.data(), url.data());
}

void ImGui::image(
    [[maybe_unused]] Image image,
    [[maybe_unused]] Vec2  image_size,
    [[maybe_unused]] Vec2  uv0,
    [[maybe_unused]] Vec2  uv1,
    [[maybe_unused]] Color tint_color,
    [[maybe_unused]] Color borderColor)
{
    notImplemented();
}

void ImGui::imageButton(
    [[maybe_unused]] StringView id,
    [[maybe_unused]] Image      image,
    [[maybe_unused]] Vec2       image_size,
    [[maybe_unused]] Vec2       uv0,
    [[maybe_unused]] Vec2       uv1,
    [[maybe_unused]] Color      background_color,
    [[maybe_unused]] Color      borderColor)
{
    notImplemented();
}

auto ImGui::beginCombo(StringView label, StringView preview_value, ImGuiComboOpts flags) -> bool
{
    verifyHaveImpl;
    return ::ImGui::BeginCombo(label.data(), preview_value.data(), convert(flags));
}

void ImGui::endCombo()
{
    verifyHaveImpl;
    ::ImGui::EndCombo();
}

auto ImGui::combo(
    StringView       label,
    int&             current_item,
    Span<StringView> items,
    Maybe<u32>       popup_max_height_in_items) -> bool
{
    declareThisImpl;

    const auto height = popup_max_height_in_items.valueOr(-1);

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
        &current_item,
        impl->tmpStringPtrLists.last().ptrs.data(),
        static_cast<int>(items.size()),
        height);
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
    verifyHaveImpl;
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
    verifyHaveImpl;
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
    verifyHaveImpl;
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
    verifyHaveImpl;
    return ::ImGui::DragFloat4(label.data(), &value.x, speed, min, max, format.data(), convert(flags));
}

auto ImGui::drag(
    StringView      label,
    int&            value,
    float           speed,
    int             min,
    int             max,
    StringView      format,
    ImGuiSliderOpts flags) const -> bool
{
    verifyHaveImpl;
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
    verifyHaveImpl;
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
    verifyHaveImpl;
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
    verifyHaveImpl;
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
    verifyHaveImpl;
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
    verifyHaveImpl;
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
    verifyHaveImpl;
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
    verifyHaveImpl;
    return ::ImGui::SliderFloat4(label.data(), &value.x, min, max, format.data(), convert(flags));
}

auto ImGui::sliderAngle(
    StringView      label,
    Degrees&        value,
    Degrees         min,
    Degrees         max,
    StringView      format,
    ImGuiSliderOpts flags) -> bool
{
    verifyHaveImpl;

    return ::ImGui::SliderAngle(
        label.data(),
        &value.value,
        min.value,
        max.value,
        format.data(),
        convert(flags));
}

auto ImGui::slider(StringView label, int& value, int min, int max, StringView format, ImGuiSliderOpts flags)
    -> bool
{
    verifyHaveImpl;
    return ::ImGui::SliderInt(label.data(), &value, min, max, format.data(), convert(flags));
}

auto ImGui::slider(StringView label, Vec2i& value, int min, int max, StringView format, ImGuiSliderOpts flags)
    -> bool
{
    verifyHaveImpl;
    return ::ImGui::SliderInt2(label.data(), &value.x, min, max, format.data(), convert(flags));
}

auto ImGui::slider(StringView label, Vec3i& value, int min, int max, StringView format, ImGuiSliderOpts flags)
    -> bool
{
    verifyHaveImpl;
    return ::ImGui::SliderInt3(label.data(), &value.x, min, max, format.data(), convert(flags));
}

auto ImGui::slider(StringView label, Vec4i& value, int min, int max, StringView format, ImGuiSliderOpts flags)
    -> bool
{
    verifyHaveImpl;
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
    verifyHaveImpl;
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
    verifyHaveImpl;
    return ::ImGui::VSliderInt(label.data(), convert(size), &value, min, max, format.data(), convert(flags));
}

bool ImGui::inputText(StringView label, String& value, imgui_input_text_opts flags)
{
    verifyHaveImpl;
    return ::ImGui::InputText(label.data(), value.data(), value.size(), convert(flags));
}

bool ImGui::inputTextMultiline(StringView label, String& value, Maybe<Vec2> size, imgui_input_text_opts flags)
{
    verifyHaveImpl;
    return ::ImGui::InputTextMultiline(
        label.data(),
        value.data(),
        value.size(),
        convert(size.valueOr(Vec2())),
        convert(flags));
}

bool ImGui::inputTextWithHint(StringView label, StringView hint, String& value, imgui_input_text_opts flags)
{
    verifyHaveImpl;
    return ::ImGui::InputTextWithHint(label.data(), hint.data(), value.data(), value.size(), convert(flags));
}

bool ImGui::input(
    StringView            label,
    float&                value,
    float                 step,
    float                 step_fast,
    StringView            format,
    imgui_input_text_opts flags)
{
    verifyHaveImpl;
    return ::ImGui::InputFloat(label.data(), &value, step, step_fast, format.data(), convert(flags));
}

bool ImGui::input(StringView label, Vec2& value, StringView format, imgui_input_text_opts flags)
{
    verifyHaveImpl;
    return ::ImGui::InputFloat2(label.data(), &value.x, format.data(), convert(flags));
}

bool ImGui::input(StringView label, Vec3& value, StringView format, imgui_input_text_opts flags)
{
    verifyHaveImpl;
    return ::ImGui::InputFloat3(label.data(), &value.x, format.data(), convert(flags));
}

bool ImGui::input(StringView label, Vec4& value, StringView format, imgui_input_text_opts flags)
{
    verifyHaveImpl;
    return ::ImGui::InputFloat4(label.data(), &value.x, format.data(), convert(flags));
}

bool ImGui::input(StringView label, int& value, int step, int step_fast, imgui_input_text_opts flags)
{
    verifyHaveImpl;
    return ::ImGui::InputInt(label.data(), &value, step, step_fast, convert(flags));
}

bool ImGui::input(StringView label, Vec2i& value, imgui_input_text_opts flags)
{
    verifyHaveImpl;
    return ::ImGui::InputInt2(label.data(), &value.x, convert(flags));
}

bool ImGui::input(StringView label, Vec3i& value, imgui_input_text_opts flags)
{
    verifyHaveImpl;
    return ::ImGui::InputInt3(label.data(), &value.x, convert(flags));
}

bool ImGui::input(StringView label, Vec4i& value, imgui_input_text_opts flags)
{
    verifyHaveImpl;
    return ::ImGui::InputInt4(label.data(), &value.x, convert(flags));
}

bool ImGui::colorEdit(StringView label, Color& value, ImGuiColorEditOpts flags)
{
    verifyHaveImpl;
    return ::ImGui::ColorEdit4(label.data(), &value.r, convert(flags));
}

bool ImGui::colorEditRgb(StringView label, Color& value, ImGuiColorEditOpts flags)
{
    verifyHaveImpl;
    return ::ImGui::ColorEdit3(label.data(), &value.r, convert(flags));
}

bool ImGui::colorPicker(StringView label, Color& value, ImGuiColorEditOpts flags)
{
    verifyHaveImpl;
    return ::ImGui::ColorPicker4(label.data(), &value.r, convert(flags));
}

bool ImGui::colorPickerRgb(StringView label, Color& value, ImGuiColorEditOpts flags)
{
    verifyHaveImpl;
    return ::ImGui::ColorPicker3(label.data(), &value.r, convert(flags));
}

bool ImGui::colorButton(StringView id, Color color, ImGuiColorEditOpts flags, Maybe<Vec2> size)
{
    verifyHaveImpl;
    return ::ImGui::ColorButton(id.data(), convert(color), convert(flags), convert(size.valueOr(Vec2())));
}

bool ImGui::treeNode(StringView label)
{
    verifyHaveImpl;
    return ::ImGui::TreeNode(label.data());
}

bool ImGui::treeNode(StringView id, StringView fmt, ...)
{
    verifyHaveImpl;

    va_list args;
    va_start(args, fmt);
    const auto result = ::ImGui::TreeNodeV(id.data(), fmt.data(), args);
    va_end(args);

    return result;
}

bool ImGui::treeNodeEx(StringView id, ImGuiTreeNodeOpts flags, StringView fmt, ...)
{
    verifyHaveImpl;

    va_list args;
    va_start(args, fmt);
    const auto result = ::ImGui::TreeNodeExV(id.data(), convert(flags), fmt.data(), args);
    va_end(args);

    return result;
}

void ImGui::treePush(const void* id)
{
    verifyHaveImpl;
    ::ImGui::TreePush(id);
}

void ImGui::treePop()
{
    verifyHaveImpl;
    ::ImGui::TreePop();
}

float ImGui::treeNodeToLabelSpacing()
{
    verifyHaveImpl;
    return ::ImGui::GetTreeNodeToLabelSpacing();
}

bool ImGui::collapsingHeader(StringView label, ImGuiTreeNodeOpts flags)
{
    verifyHaveImpl;
    return ::ImGui::CollapsingHeader(label.data(), convert(flags));
}

bool ImGui::collapsingHeader(StringView label, bool& isVisible, ImGuiTreeNodeOpts flags)
{
    verifyHaveImpl;
    return ::ImGui::CollapsingHeader(label.data(), &isVisible, convert(flags));
}

void ImGui::setNextItemOpen(bool is_open, ImGuiCondition cond)
{
    verifyHaveImpl;
    ::ImGui::SetNextItemOpen(is_open, convert(cond));
}

bool ImGui::selectable(StringView label, bool is_selected, ImGuiSelectableOpts flags, Maybe<Vec2> size)
{
    verifyHaveImpl;
    return ::ImGui::Selectable(label.data(), is_selected, convert(flags), convert(size.valueOr(Vec2())));
}

bool ImGui::selectableWithBinding(
    StringView          label,
    bool&               is_selected,
    ImGuiSelectableOpts flags,
    Maybe<Vec2>         size)
{
    verifyHaveImpl;
    return ::ImGui::Selectable(label.data(), &is_selected, convert(flags), convert(size.valueOr(Vec2())));
}

bool ImGui::beginListBox(StringView label, Maybe<Vec2> size)
{
    verifyHaveImpl;
    return ::ImGui::BeginListBox(label.data(), convert(size.valueOr(Vec2())));
}

void ImGui::endListBox()
{
    verifyHaveImpl;
    ::ImGui::EndListBox();
}

bool ImGui::listBox(StringView label, int& current_item, Span<StringView> items, Maybe<u32> height_in_items)
{
    declareThisImpl;

    const auto height = height_in_items.valueOr(-1);

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
        &current_item,
        impl->tmpStringPtrLists.last().ptrs.data(),
        static_cast<int>(items.size()),
        height);
}

void ImGui::value(StringView prefix, bool value)
{
    verifyHaveImpl;
    ::ImGui::Value(prefix.data(), value);
}

void ImGui::value(StringView prefix, int value)
{
    verifyHaveImpl;
    ::ImGui::Value(prefix.data(), value);
}

void ImGui::value(StringView prefix, unsigned int value)
{
    verifyHaveImpl;
    ::ImGui::Value(prefix.data(), value);
}

void ImGui::value(StringView prefix, float value, StringView float_format)
{
    verifyHaveImpl;
    ::ImGui::Value(prefix.data(), value, float_format.data());
}

bool ImGui::beginMenuBar()
{
    verifyHaveImpl;
    return ::ImGui::BeginMenuBar();
}

void ImGui::endMenuBar()
{
    verifyHaveImpl;
    ::ImGui::EndMenuBar();
}

bool ImGui::beginMainMenuBar()
{
    verifyHaveImpl;
    return ::ImGui::BeginMainMenuBar();
}

void ImGui::endMainMenuBar()
{
    verifyHaveImpl;
    ::ImGui::EndMainMenuBar();
}

auto ImGui::beginMenu(StringView label, bool enabled) -> bool
{
    verifyHaveImpl;
    return ::ImGui::BeginMenu(label.data(), enabled);
}

auto ImGui::endMenu() -> void
{
    verifyHaveImpl;
    ::ImGui::EndMenu();
}

bool ImGui::menuItem(StringView label, StringView shortcut, bool selected, bool enabled)
{
    verifyHaveImpl;
    return ::ImGui::MenuItem(label.data(), shortcut.data(), selected, enabled);
}

bool ImGui::menuItemWithBinding(StringView label, StringView shortcut, bool& is_selected, bool enabled)
{
    verifyHaveImpl;
    return ::ImGui::MenuItem(label.data(), shortcut.data(), &is_selected, enabled);
}

auto ImGui::beginTooltip() -> bool
{
    verifyHaveImpl;
    return ::ImGui::BeginTooltip();
}

void ImGui::endTooltip()
{
    verifyHaveImpl;
    return ::ImGui::EndTooltip();
}

void ImGui::setTooltip(StringView fmt, ...)
{
    verifyHaveImpl;

    va_list args;
    va_start(args, fmt);
    ::ImGui::SetTooltipV(fmt.data(), args);
    va_end(args);
}

auto ImGui::beginItemTooltip() -> bool
{
    verifyHaveImpl;
    return ::ImGui::BeginItemTooltip();
}

void ImGui::setItemTooltip(StringView fmt, ...)
{
    verifyHaveImpl;

    va_list args;
    va_start(args, fmt);
    ::ImGui::SetItemTooltipV(fmt.data(), args);
    va_end(args);
}

auto ImGui::beginPopup(StringView id, ImGuiWindowOpts flags) -> bool
{
    verifyHaveImpl;
    return ::ImGui::BeginPopup(id.data(), convert(flags));
}

auto ImGui::beginPopupModal(StringView name, bool& is_open, ImGuiWindowOpts flags) -> bool
{
    verifyHaveImpl;
    return ::ImGui::BeginPopupModal(name.data(), &is_open, convert(flags));
}

void ImGui::endPopup()
{
    verifyHaveImpl;
    ::ImGui::EndPopup();
}

void ImGui::openPopup(StringView id, ImGuiPopupOpts flags)
{
    verifyHaveImpl;
    ::ImGui::OpenPopup(id.data(), convert(flags));
}

void ImGui::openPopupOnItemClick(StringView id, ImGuiPopupOpts flags)
{
    verifyHaveImpl;
    ::ImGui::OpenPopupOnItemClick(id.data(), convert(flags));
}

void ImGui::closeCurrentPopup()
{
    verifyHaveImpl;
    ::ImGui::CloseCurrentPopup();
}

auto ImGui::beginPopupContextItem(StringView id, ImGuiPopupOpts flags) -> bool
{
    verifyHaveImpl;
    return ::ImGui::BeginPopupContextItem(id.data(), convert(flags));
}

auto ImGui::beginPopupContextWindow(StringView id, ImGuiPopupOpts flags) -> bool
{
    verifyHaveImpl;
    return ::ImGui::BeginPopupContextWindow(id.data(), convert(flags));
}

auto ImGui::beginPopupContextVoid(StringView id, ImGuiPopupOpts flags) -> bool
{
    verifyHaveImpl;
    return ::ImGui::BeginPopupContextVoid(id.data(), convert(flags));
}

auto ImGui::isPopupOpen(StringView id, ImGuiPopupOpts flags) const -> bool
{
    verifyHaveImpl;
    return ::ImGui::IsPopupOpen(id.data(), convert(flags));
}

auto ImGui::beginTable(
    StringView     id,
    u32            columns,
    ImGuiTableOpts flags,
    Maybe<Vec2>    outer_size,
    Maybe<float>   inner_width) -> bool
{
    verifyHaveImpl;

    return ::ImGui::BeginTable(
        id.data(),
        columns,
        convert(flags),
        convert(outer_size.valueOr(Vec2())),
        inner_width.valueOr(0.0f));
}

void ImGui::endTable()
{
    verifyHaveImpl;
    ::ImGui::EndTable();
}

void ImGui::tableNextRow(ImGuiTableRowOpts flags, Maybe<float> min_row_height)
{
    verifyHaveImpl;
    ::ImGui::TableNextRow(convert(flags), min_row_height.valueOr(0.0f));
}

auto ImGui::tableNextColumn() -> bool
{
    verifyHaveImpl;
    return ::ImGui::TableNextColumn();
}

auto ImGui::tableSetColumnIndex(int column) -> bool
{
    verifyHaveImpl;
    return ::ImGui::TableSetColumnIndex(column);
}

void ImGui::tableSetupColumn(StringView label, ImGuiTableColumnOpts flags)
{
    verifyHaveImpl;
    ::ImGui::TableSetupColumn(label.data(), convert(flags));
}

void ImGui::tableSetupScrollFreeze(u32 cols, u32 rows)
{
    verifyHaveImpl;
    ::ImGui::TableSetupScrollFreeze(cols, rows);
}

void ImGui::tableHeader(StringView label)
{
    verifyHaveImpl;
    ::ImGui::TableHeader(label.data());
}

void ImGui::tableHeadersRow()
{
    verifyHaveImpl;
    ::ImGui::TableHeadersRow();
}

void ImGui::tableAngledHeadersRow()
{
    verifyHaveImpl;
    ::ImGui::TableAngledHeadersRow();
}

auto ImGui::tableColumnCount() const -> int
{
    verifyHaveImpl;
    return ::ImGui::TableGetColumnCount();
}

auto ImGui::tableColumnIndex() const -> int
{
    verifyHaveImpl;
    return ::ImGui::TableGetColumnIndex();
}

auto ImGui::tableRowIndex() const -> int
{
    verifyHaveImpl;
    return ::ImGui::TableGetRowIndex();
}

auto ImGui::tableColumnName(Maybe<int> column) const -> StringView
{
    verifyHaveImpl;
    return ::ImGui::TableGetColumnName(column.valueOr(-1));
}

void ImGui::setItemDefaultFocus()
{
    verifyHaveImpl;
    ::ImGui::SetItemDefaultFocus();
}

void ImGui::setNavCursorVisible(bool value)
{
    verifyHaveImpl;
    ::ImGui::SetNavCursorVisible(value);
}

void ImGui::setNextItemAllowOverlap()
{
    verifyHaveImpl;
    ::ImGui::SetNextItemAllowOverlap();
}

bool ImGui::isItemHovered(ImGuiHoveredOpts flags) const
{
    verifyHaveImpl;
    return ::ImGui::IsItemHovered(convert(flags));
}

bool ImGui::isItemActive() const
{
    verifyHaveImpl;
    return ::ImGui::IsItemActive();
}

bool ImGui::isItemFocused() const
{
    verifyHaveImpl;
    return ::ImGui::IsItemFocused();
}

bool ImGui::isItemClicked(MouseButton button) const
{
    verifyHaveImpl;
    return ::ImGui::IsItemClicked(convert(button));
}

bool ImGui::isItemVisible() const
{
    verifyHaveImpl;
    return ::ImGui::IsItemVisible();
}

bool ImGui::isItemEdited() const
{
    verifyHaveImpl;
    return ::ImGui::IsItemEdited();
}

bool ImGui::isItemActivated() const
{
    verifyHaveImpl;
    return ::ImGui::IsItemActivated();
}

bool ImGui::isItemDeactivated() const
{
    verifyHaveImpl;
    return ::ImGui::IsItemDeactivated();
}

bool ImGui::isItemDeactivatedAfterEdit() const
{
    verifyHaveImpl;
    return ::ImGui::IsItemDeactivatedAfterEdit();
}

bool ImGui::isItemToggledOpen() const
{
    verifyHaveImpl;
    return ::ImGui::IsItemToggledOpen();
}

bool ImGui::isAnyItemHovered() const
{
    verifyHaveImpl;
    return ::ImGui::IsAnyItemHovered();
}

bool ImGui::isAnyItemActive() const
{
    verifyHaveImpl;
    return ::ImGui::IsAnyItemActive();
}

bool ImGui::isAnyItemFocused() const
{
    verifyHaveImpl;
    return ::ImGui::IsAnyItemFocused();
}

Rectf ImGui::itemRect() const
{
    verifyHaveImpl;

    const auto topLeft = ::ImGui::GetItemRectMin();
    const auto size    = ::ImGui::GetItemRectSize();

    return Rectf(convert(topLeft), convert(size));
}
} // namespace Polly