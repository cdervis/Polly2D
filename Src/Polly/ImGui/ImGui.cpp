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
    verify_have_impl;
    ::ImGui::Begin(name.data(), is_open, convert(flags));
}

void ImGui::endWindow()
{
    verify_have_impl;
    ::ImGui::End();
}

void ImGui::beginChildWindow(
    StringView      id,
    Maybe<Vec2>     size,
    ImGuiChildOpts  child_flags,
    ImGuiWindowOpts window_flags)
{
    verify_have_impl;
    ::ImGui::BeginChild(
        id.data(),
        convert(size.valueOr(Vec2())),
        convert(child_flags),
        convert(window_flags));
}

void ImGui::endChildWindow()
{
    verify_have_impl;
    ::ImGui::EndChild();
}

bool ImGui::isWindowAppearing()
{
    verify_have_impl;
    return ::ImGui::IsWindowAppearing();
}

bool ImGui::isWindowCollapsed()
{
    verify_have_impl;
    return ::ImGui::IsWindowCollapsed();
}

bool ImGui::isWindowFocused(ImGuiFocusedOpts flags)
{
    verify_have_impl;
    return ::ImGui::IsWindowFocused(convert(flags));
}

bool ImGui::isWindowHovered(ImGuiHoveredOpts flags)
{
    verify_have_impl;
    return ::ImGui::IsWindowHovered(convert(flags));
}

Vec2 ImGui::windowPosition()
{
    verify_have_impl;
    return convert(::ImGui::GetWindowPos());
}

Vec2 ImGui::windowSize()
{
    verify_have_impl;
    return convert(::ImGui::GetWindowSize());
}

float ImGui::windowWidth()
{
    verify_have_impl;
    return ::ImGui::GetWindowWidth();
}

float ImGui::windowHeight()
{
    verify_have_impl;
    return ::ImGui::GetWindowHeight();
}

void ImGui::setNextWindowPosition(Vec2 position, ImGuiCondition cond)
{
    verify_have_impl;
    ::ImGui::SetNextWindowPos(convert(position), convert(cond));
}

void ImGui::setNextWindowSize(Vec2 size, ImGuiCondition cond)
{
    verify_have_impl;
    ::ImGui::SetNextWindowSize(convert(size), convert(cond));
}

void ImGui::setNextWindowCollapsed(bool collapsed, ImGuiCondition cond)
{
    verify_have_impl;
    ::ImGui::SetNextWindowCollapsed(collapsed, convert(cond));
}

void ImGui::setWindowPosition(StringView name, Vec2 position, ImGuiCondition cond)
{
    verify_have_impl;
    ::ImGui::SetWindowPos(name.data(), convert(position), convert(cond));
}

void ImGui::setWindowSize(StringView name, Vec2 size, ImGuiCondition cond)
{
    verify_have_impl;
    ::ImGui::SetWindowSize(name.data(), convert(size), convert(cond));
}

void ImGui::setWindowCollapsed(StringView name, bool collapsed, ImGuiCondition cond)
{
    verify_have_impl;
    ::ImGui::SetWindowCollapsed(name.data(), collapsed, convert(cond));
}

void ImGui::setWindowFocus(StringView name)
{
    verify_have_impl;
    ::ImGui::SetWindowFocus(name.data());
}

void ImGui::separator()
{
    verify_have_impl;
    ::ImGui::Separator();
}

void ImGui::sameLine(float offset_from_start_x, float spacing)
{
    verify_have_impl;
    ::ImGui::SameLine(offset_from_start_x, spacing);
}

void ImGui::newLine()
{
    verify_have_impl;
    ::ImGui::NewLine();
}

void ImGui::spacing()
{
    verify_have_impl;
    ::ImGui::Spacing();
}

void ImGui::dummy(Vec2 size)
{
    verify_have_impl;
    ::ImGui::Dummy(convert(size));
}

void ImGui::indent(float indent_w)
{
    verify_have_impl;
    ::ImGui::Indent(indent_w);
}

void ImGui::unindent(float indent_w)
{
    verify_have_impl;
    ::ImGui::Unindent(indent_w);
}

void ImGui::beginGroup()
{
    verify_have_impl;
    ::ImGui::BeginGroup();
}

void ImGui::endGroup()
{
    verify_have_impl;
    ::ImGui::EndGroup();
}

void ImGui::alignTextToFramePadding()
{
    verify_have_impl;
    ::ImGui::AlignTextToFramePadding();
}

float ImGui::textLineHeight()
{
    verify_have_impl;
    return ::ImGui::GetTextLineHeight();
}

float ImGui::textLineHeightWithSpacing()
{
    verify_have_impl;
    return ::ImGui::GetTextLineHeightWithSpacing();
}

float ImGui::frameHeight()
{
    verify_have_impl;
    return ::ImGui::GetFrameHeight();
}

float ImGui::frameHeightWithSpacing()
{
    verify_have_impl;
    return ::ImGui::GetFrameHeightWithSpacing();
}

void ImGui::textUnformatted(StringView text)
{
    verify_have_impl;
    ::ImGui::TextUnformatted(text.data());
}

void ImGui::text(StringView fmt, ...)
{
    verify_have_impl;
    va_list args;
    va_start(args, fmt);
    ::ImGui::TextV(fmt.data(), args);
    va_end(args);
}

void ImGui::textColored(Color color, StringView fmt, ...)
{
    verify_have_impl;
    va_list args;
    va_start(args, fmt);
    ::ImGui::TextColoredV(convert(color), fmt.data(), args);
    va_end(args);
}

void ImGui::textDisabled(StringView fmt, ...)
{
    verify_have_impl;
    va_list args;
    va_start(args, fmt);
    ::ImGui::TextDisabledV(fmt.data(), args);
    va_end(args);
}

void ImGui::textWrapped(StringView fmt, ...)
{
    verify_have_impl;
    va_list args;
    va_start(args, fmt);
    ::ImGui::TextWrappedV(fmt.data(), args);
    va_end(args);
}

void ImGui::labelText(StringView label, StringView fmt, ...)
{
    verify_have_impl;
    va_list args;
    va_start(args, fmt);
    ::ImGui::LabelTextV(label.data(), fmt.data(), args);
    va_end(args);
}

void ImGui::bulletText(StringView fmt, ...)
{
    verify_have_impl;
    va_list args;
    va_start(args, fmt);
    ::ImGui::BulletTextV(fmt.data(), args);
    va_end(args);
}

void ImGui::separatorWithText(StringView label)
{
    verify_have_impl;
    ::ImGui::SeparatorText(label.data());
}

auto ImGui::button(StringView label, Maybe<Vec2> size) -> bool
{
    verify_have_impl;
    return ::ImGui::Button(label.data(), convert(size.valueOr(Vec2())));
}

auto ImGui::smallButton(StringView label) -> bool
{
    verify_have_impl;
    return ::ImGui::SmallButton(label.data());
}

auto ImGui::invisibleButton(StringView id, Vec2 size, ImGuiButtonOpts flags) -> bool
{
    verify_have_impl;
    return ::ImGui::InvisibleButton(id.data(), convert(size), convert(flags));
}

auto ImGui::arrowButton(StringView id, Direction direction) -> bool
{
    verify_have_impl;
    return ::ImGui::ArrowButton(id.data(), convert(direction));
}

bool ImGui::checkbox(StringView label, bool& is_checked)
{
    verify_have_impl;
    return ::ImGui::Checkbox(label.data(), &is_checked);
}

auto ImGui::checkboxFlags(StringView label, int& flags, int flags_value) -> bool
{
    verify_have_impl;
    return ::ImGui::CheckboxFlags(label.data(), &flags, flags_value);
}

auto ImGui::radioButton(StringView label, bool isActive) -> bool
{
    verify_have_impl;
    return ::ImGui::RadioButton(label.data(), isActive);
}

bool ImGui::radioButton(StringView label, int& value, int value_button)
{
    verify_have_impl;
    return ::ImGui::RadioButton(label.data(), &value, value_button);
}

void ImGui::progressBar(float fraction, Maybe<Vec2> size, StringView overlay)
{
    verify_have_impl;
    ::ImGui::ProgressBar(fraction, convert(size.valueOr(Vec2())), overlay.data());
}

void ImGui::bullet()
{
    verify_have_impl;
    ::ImGui::Bullet();
}

auto ImGui::textLink(StringView label) -> bool
{
    verify_have_impl;
    return ::ImGui::TextLink(label.data());
}

void ImGui::textLinkOpenUrl(StringView label, StringView url)
{
    verify_have_impl;
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
    verify_have_impl;
    return ::ImGui::BeginCombo(label.data(), preview_value.data(), convert(flags));
}

void ImGui::endCombo()
{
    verify_have_impl;
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
    verify_have_impl;
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
    verify_have_impl;
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
    verify_have_impl;
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
    verify_have_impl;
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
    verify_have_impl;
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
    verify_have_impl;
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
    verify_have_impl;
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
    verify_have_impl;
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
    verify_have_impl;
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
    verify_have_impl;
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
    verify_have_impl;
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
    verify_have_impl;
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
    verify_have_impl;

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
    verify_have_impl;
    return ::ImGui::SliderInt(label.data(), &value, min, max, format.data(), convert(flags));
}

auto ImGui::slider(StringView label, Vec2i& value, int min, int max, StringView format, ImGuiSliderOpts flags)
    -> bool
{
    verify_have_impl;
    return ::ImGui::SliderInt2(label.data(), &value.x, min, max, format.data(), convert(flags));
}

auto ImGui::slider(StringView label, Vec3i& value, int min, int max, StringView format, ImGuiSliderOpts flags)
    -> bool
{
    verify_have_impl;
    return ::ImGui::SliderInt3(label.data(), &value.x, min, max, format.data(), convert(flags));
}

auto ImGui::slider(StringView label, Vec4i& value, int min, int max, StringView format, ImGuiSliderOpts flags)
    -> bool
{
    verify_have_impl;
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
    verify_have_impl;
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
    verify_have_impl;
    return ::ImGui::VSliderInt(label.data(), convert(size), &value, min, max, format.data(), convert(flags));
}

bool ImGui::inputText(StringView label, String& value, imgui_input_text_opts flags)
{
    verify_have_impl;
    return ::ImGui::InputText(label.data(), value.data(), value.size(), convert(flags));
}

bool ImGui::inputTextMultiline(StringView label, String& value, Maybe<Vec2> size, imgui_input_text_opts flags)
{
    verify_have_impl;
    return ::ImGui::InputTextMultiline(
        label.data(),
        value.data(),
        value.size(),
        convert(size.valueOr(Vec2())),
        convert(flags));
}

bool ImGui::inputTextWithHint(StringView label, StringView hint, String& value, imgui_input_text_opts flags)
{
    verify_have_impl;
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
    verify_have_impl;
    return ::ImGui::InputFloat(label.data(), &value, step, step_fast, format.data(), convert(flags));
}

bool ImGui::input(StringView label, Vec2& value, StringView format, imgui_input_text_opts flags)
{
    verify_have_impl;
    return ::ImGui::InputFloat2(label.data(), &value.x, format.data(), convert(flags));
}

bool ImGui::input(StringView label, Vec3& value, StringView format, imgui_input_text_opts flags)
{
    verify_have_impl;
    return ::ImGui::InputFloat3(label.data(), &value.x, format.data(), convert(flags));
}

bool ImGui::input(StringView label, Vec4& value, StringView format, imgui_input_text_opts flags)
{
    verify_have_impl;
    return ::ImGui::InputFloat4(label.data(), &value.x, format.data(), convert(flags));
}

bool ImGui::input(StringView label, int& value, int step, int step_fast, imgui_input_text_opts flags)
{
    verify_have_impl;
    return ::ImGui::InputInt(label.data(), &value, step, step_fast, convert(flags));
}

bool ImGui::input(StringView label, Vec2i& value, imgui_input_text_opts flags)
{
    verify_have_impl;
    return ::ImGui::InputInt2(label.data(), &value.x, convert(flags));
}

bool ImGui::input(StringView label, Vec3i& value, imgui_input_text_opts flags)
{
    verify_have_impl;
    return ::ImGui::InputInt3(label.data(), &value.x, convert(flags));
}

bool ImGui::input(StringView label, Vec4i& value, imgui_input_text_opts flags)
{
    verify_have_impl;
    return ::ImGui::InputInt4(label.data(), &value.x, convert(flags));
}

bool ImGui::colorEdit(StringView label, Color& value, ImGuiColorEditOpts flags)
{
    verify_have_impl;
    return ::ImGui::ColorEdit4(label.data(), &value.r, convert(flags));
}

bool ImGui::colorEditRgb(StringView label, Color& value, ImGuiColorEditOpts flags)
{
    verify_have_impl;
    return ::ImGui::ColorEdit3(label.data(), &value.r, convert(flags));
}

bool ImGui::colorPicker(StringView label, Color& value, ImGuiColorEditOpts flags)
{
    verify_have_impl;
    return ::ImGui::ColorPicker4(label.data(), &value.r, convert(flags));
}

bool ImGui::colorPickerRgb(StringView label, Color& value, ImGuiColorEditOpts flags)
{
    verify_have_impl;
    return ::ImGui::ColorPicker3(label.data(), &value.r, convert(flags));
}

bool ImGui::colorButton(StringView id, Color color, ImGuiColorEditOpts flags, Maybe<Vec2> size)
{
    verify_have_impl;
    return ::ImGui::ColorButton(id.data(), convert(color), convert(flags), convert(size.valueOr(Vec2())));
}

bool ImGui::treeNode(StringView label)
{
    verify_have_impl;
    return ::ImGui::TreeNode(label.data());
}

bool ImGui::treeNode(StringView id, StringView fmt, ...)
{
    verify_have_impl;

    va_list args;
    va_start(args, fmt);
    const auto result = ::ImGui::TreeNodeV(id.data(), fmt.data(), args);
    va_end(args);

    return result;
}

bool ImGui::treeNodeEx(StringView id, ImGuiTreeNodeOpts flags, StringView fmt, ...)
{
    verify_have_impl;

    va_list args;
    va_start(args, fmt);
    const auto result = ::ImGui::TreeNodeExV(id.data(), convert(flags), fmt.data(), args);
    va_end(args);

    return result;
}

void ImGui::treePush(const void* id)
{
    verify_have_impl;
    ::ImGui::TreePush(id);
}

void ImGui::treePop()
{
    verify_have_impl;
    ::ImGui::TreePop();
}

float ImGui::treeNodeToLabelSpacing()
{
    verify_have_impl;
    return ::ImGui::GetTreeNodeToLabelSpacing();
}

bool ImGui::collapsingHeader(StringView label, ImGuiTreeNodeOpts flags)
{
    verify_have_impl;
    return ::ImGui::CollapsingHeader(label.data(), convert(flags));
}

bool ImGui::collapsingHeader(StringView label, bool& isVisible, ImGuiTreeNodeOpts flags)
{
    verify_have_impl;
    return ::ImGui::CollapsingHeader(label.data(), &isVisible, convert(flags));
}

void ImGui::setNextItemOpen(bool is_open, ImGuiCondition cond)
{
    verify_have_impl;
    ::ImGui::SetNextItemOpen(is_open, convert(cond));
}

bool ImGui::selectable(StringView label, bool is_selected, ImGuiSelectableOpts flags, Maybe<Vec2> size)
{
    verify_have_impl;
    return ::ImGui::Selectable(label.data(), is_selected, convert(flags), convert(size.valueOr(Vec2())));
}

bool ImGui::selectableWithBinding(
    StringView          label,
    bool&               is_selected,
    ImGuiSelectableOpts flags,
    Maybe<Vec2>         size)
{
    verify_have_impl;
    return ::ImGui::Selectable(label.data(), &is_selected, convert(flags), convert(size.valueOr(Vec2())));
}

bool ImGui::beginListBox(StringView label, Maybe<Vec2> size)
{
    verify_have_impl;
    return ::ImGui::BeginListBox(label.data(), convert(size.valueOr(Vec2())));
}

void ImGui::endListBox()
{
    verify_have_impl;
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
    verify_have_impl;
    ::ImGui::Value(prefix.data(), value);
}

void ImGui::value(StringView prefix, int value)
{
    verify_have_impl;
    ::ImGui::Value(prefix.data(), value);
}

void ImGui::value(StringView prefix, unsigned int value)
{
    verify_have_impl;
    ::ImGui::Value(prefix.data(), value);
}

void ImGui::value(StringView prefix, float value, StringView float_format)
{
    verify_have_impl;
    ::ImGui::Value(prefix.data(), value, float_format.data());
}

bool ImGui::beginMenuBar()
{
    verify_have_impl;
    return ::ImGui::BeginMenuBar();
}

void ImGui::endMenuBar()
{
    verify_have_impl;
    ::ImGui::EndMenuBar();
}

bool ImGui::beginMainMenuBar()
{
    verify_have_impl;
    return ::ImGui::BeginMainMenuBar();
}

void ImGui::endMainMenuBar()
{
    verify_have_impl;
    ::ImGui::EndMainMenuBar();
}

auto ImGui::beginMenu(StringView label, bool enabled) -> bool
{
    verify_have_impl;
    return ::ImGui::BeginMenu(label.data(), enabled);
}

auto ImGui::endMenu() -> void
{
    verify_have_impl;
    ::ImGui::EndMenu();
}

bool ImGui::menuItem(StringView label, StringView shortcut, bool selected, bool enabled)
{
    verify_have_impl;
    return ::ImGui::MenuItem(label.data(), shortcut.data(), selected, enabled);
}

bool ImGui::menuItemWithBinding(StringView label, StringView shortcut, bool& is_selected, bool enabled)
{
    verify_have_impl;
    return ::ImGui::MenuItem(label.data(), shortcut.data(), &is_selected, enabled);
}

auto ImGui::beginTooltip() -> bool
{
    verify_have_impl;
    return ::ImGui::BeginTooltip();
}

void ImGui::endTooltip()
{
    verify_have_impl;
    return ::ImGui::EndTooltip();
}

void ImGui::setTooltip(StringView fmt, ...)
{
    verify_have_impl;

    va_list args;
    va_start(args, fmt);
    ::ImGui::SetTooltipV(fmt.data(), args);
    va_end(args);
}

auto ImGui::beginItemTooltip() -> bool
{
    verify_have_impl;
    return ::ImGui::BeginItemTooltip();
}

void ImGui::setItemTooltip(StringView fmt, ...)
{
    verify_have_impl;

    va_list args;
    va_start(args, fmt);
    ::ImGui::SetItemTooltipV(fmt.data(), args);
    va_end(args);
}

auto ImGui::beginPopup(StringView id, ImGuiWindowOpts flags) -> bool
{
    verify_have_impl;
    return ::ImGui::BeginPopup(id.data(), convert(flags));
}

auto ImGui::beginPopupModal(StringView name, bool& is_open, ImGuiWindowOpts flags) -> bool
{
    verify_have_impl;
    return ::ImGui::BeginPopupModal(name.data(), &is_open, convert(flags));
}

void ImGui::endPopup()
{
    verify_have_impl;
    ::ImGui::EndPopup();
}

void ImGui::openPopup(StringView id, ImGuiPopupOpts flags)
{
    verify_have_impl;
    ::ImGui::OpenPopup(id.data(), convert(flags));
}

void ImGui::openPopupOnItemClick(StringView id, ImGuiPopupOpts flags)
{
    verify_have_impl;
    ::ImGui::OpenPopupOnItemClick(id.data(), convert(flags));
}

void ImGui::closeCurrentPopup()
{
    verify_have_impl;
    ::ImGui::CloseCurrentPopup();
}

auto ImGui::beginPopupContextItem(StringView id, ImGuiPopupOpts flags) -> bool
{
    verify_have_impl;
    return ::ImGui::BeginPopupContextItem(id.data(), convert(flags));
}

auto ImGui::beginPopupContextWindow(StringView id, ImGuiPopupOpts flags) -> bool
{
    verify_have_impl;
    return ::ImGui::BeginPopupContextWindow(id.data(), convert(flags));
}

auto ImGui::beginPopupContextVoid(StringView id, ImGuiPopupOpts flags) -> bool
{
    verify_have_impl;
    return ::ImGui::BeginPopupContextVoid(id.data(), convert(flags));
}

auto ImGui::isPopupOpen(StringView id, ImGuiPopupOpts flags) const -> bool
{
    verify_have_impl;
    return ::ImGui::IsPopupOpen(id.data(), convert(flags));
}

auto ImGui::beginTable(
    StringView     id,
    u32            columns,
    ImGuiTableOpts flags,
    Maybe<Vec2>    outer_size,
    Maybe<float>   inner_width) -> bool
{
    verify_have_impl;

    return ::ImGui::BeginTable(
        id.data(),
        columns,
        convert(flags),
        convert(outer_size.valueOr(Vec2())),
        inner_width.valueOr(0.0f));
}

void ImGui::endTable()
{
    verify_have_impl;
    ::ImGui::EndTable();
}

void ImGui::tableNextRow(ImGuiTableRowOpts flags, Maybe<float> min_row_height)
{
    verify_have_impl;
    ::ImGui::TableNextRow(convert(flags), min_row_height.valueOr(0.0f));
}

auto ImGui::tableNextColumn() -> bool
{
    verify_have_impl;
    return ::ImGui::TableNextColumn();
}

auto ImGui::tableSetColumnIndex(int column) -> bool
{
    verify_have_impl;
    return ::ImGui::TableSetColumnIndex(column);
}

void ImGui::tableSetupColumn(StringView label, ImGuiTableColumnOpts flags)
{
    verify_have_impl;
    ::ImGui::TableSetupColumn(label.data(), convert(flags));
}

void ImGui::tableSetupScrollFreeze(u32 cols, u32 rows)
{
    verify_have_impl;
    ::ImGui::TableSetupScrollFreeze(cols, rows);
}

void ImGui::tableHeader(StringView label)
{
    verify_have_impl;
    ::ImGui::TableHeader(label.data());
}

void ImGui::tableHeadersRow()
{
    verify_have_impl;
    ::ImGui::TableHeadersRow();
}

void ImGui::tableAngledHeadersRow()
{
    verify_have_impl;
    ::ImGui::TableAngledHeadersRow();
}

auto ImGui::tableColumnCount() const -> int
{
    verify_have_impl;
    return ::ImGui::TableGetColumnCount();
}

auto ImGui::tableColumnIndex() const -> int
{
    verify_have_impl;
    return ::ImGui::TableGetColumnIndex();
}

auto ImGui::tableRowIndex() const -> int
{
    verify_have_impl;
    return ::ImGui::TableGetRowIndex();
}

auto ImGui::tableColumnName(Maybe<int> column) const -> StringView
{
    verify_have_impl;
    return ::ImGui::TableGetColumnName(column.valueOr(-1));
}

void ImGui::setItemDefaultFocus()
{
    verify_have_impl;
    ::ImGui::SetItemDefaultFocus();
}

void ImGui::setNavCursorVisible(bool value)
{
    verify_have_impl;
    ::ImGui::SetNavCursorVisible(value);
}

void ImGui::setNextItemAllowOverlap()
{
    verify_have_impl;
    ::ImGui::SetNextItemAllowOverlap();
}

bool ImGui::isItemHovered(ImGuiHoveredOpts flags) const
{
    verify_have_impl;
    return ::ImGui::IsItemHovered(convert(flags));
}

bool ImGui::isItemActive() const
{
    verify_have_impl;
    return ::ImGui::IsItemActive();
}

bool ImGui::isItemFocused() const
{
    verify_have_impl;
    return ::ImGui::IsItemFocused();
}

bool ImGui::isItemClicked(MouseButton button) const
{
    verify_have_impl;
    return ::ImGui::IsItemClicked(convert(button));
}

bool ImGui::isItemVisible() const
{
    verify_have_impl;
    return ::ImGui::IsItemVisible();
}

bool ImGui::isItemEdited() const
{
    verify_have_impl;
    return ::ImGui::IsItemEdited();
}

bool ImGui::isItemActivated() const
{
    verify_have_impl;
    return ::ImGui::IsItemActivated();
}

bool ImGui::isItemDeactivated() const
{
    verify_have_impl;
    return ::ImGui::IsItemDeactivated();
}

bool ImGui::isItemDeactivatedAfterEdit() const
{
    verify_have_impl;
    return ::ImGui::IsItemDeactivatedAfterEdit();
}

bool ImGui::isItemToggledOpen() const
{
    verify_have_impl;
    return ::ImGui::IsItemToggledOpen();
}

bool ImGui::isAnyItemHovered() const
{
    verify_have_impl;
    return ::ImGui::IsAnyItemHovered();
}

bool ImGui::isAnyItemActive() const
{
    verify_have_impl;
    return ::ImGui::IsAnyItemActive();
}

bool ImGui::isAnyItemFocused() const
{
    verify_have_impl;
    return ::ImGui::IsAnyItemFocused();
}

Rectf ImGui::itemRect() const
{
    verify_have_impl;

    const auto topLeft = ::ImGui::GetItemRectMin();
    const auto size    = ::ImGui::GetItemRectSize();

    return Rectf(convert(topLeft), convert(size));
}
} // namespace Polly