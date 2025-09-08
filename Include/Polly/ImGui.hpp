// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Color.hpp"
#include "Polly/Direction.hpp"
#include "Polly/Image.hpp"
#include "Polly/Linalg.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/Rectangle.hpp"

namespace Polly
{
enum class MouseButton;

enum class ImGuiTableRowOpts
{
    None    = 0,
    Headers = 1 << 0,
};

enum class ImGuiColorEditOpts
{
    None             = 0,
    NoAlpha          = 1 << 1,
    NoPicker         = 1 << 2,
    NoOptions        = 1 << 3,
    NoSmallPreview   = 1 << 4,
    NoInputs         = 1 << 5,
    NoTooltip        = 1 << 6,
    NoLabel          = 1 << 7,
    NoSidePreview    = 1 << 8,
    NoDragDrop       = 1 << 9,
    NoBorder         = 1 << 10,
    AlphaBar         = 1 << 16,
    AlphaPreview     = 1 << 17,
    AlphaPreviewHalf = 1 << 18,
    Hdr              = 1 << 19,
    DisplayRgb       = 1 << 20,
    DisplayHsv       = 1 << 21,
    DisplayHex       = 1 << 22,
    U8               = 1 << 23,
    Float            = 1 << 24,
    PickerHueBar     = 1 << 25,
    PickerHueWheel   = 1 << 26,
    InputRgb         = 1 << 27,
    InputHsv         = 1 << 28,
};

enum class ImGuiComboOpts
{
    None            = 0,
    PopupAlignLeft  = 1 << 0,
    HeightSmall     = 1 << 1,
    HeightRegular   = 1 << 2,
    HeightLarge     = 1 << 3,
    HeightLargest   = 1 << 4,
    NoArrowButton   = 1 << 5,
    NoPreview       = 1 << 6,
    WidthFitPreview = 1 << 7,
};

enum class ImGuiButtonOpts
{
    None = 0,
};

enum class ImGuiTreeNodeOpts
{
    None = 0,
};

enum class ImGuiWindowOpts
{
    None                      = 0,
    NoTitleBar                = 1 << 0,
    NoResize                  = 1 << 1,
    NoMove                    = 1 << 2,
    NoScrollBar               = 1 << 3,
    NoScrollWithMouse         = 1 << 4,
    NoCollapse                = 1 << 5,
    AlwaysAutoResize          = 1 << 6,
    NoBackground              = 1 << 7,
    NoSavedSettings           = 1 << 8,
    NoMouseInputs             = 1 << 9,
    MenuBar                   = 1 << 10,
    HorizontalScrollBar       = 1 << 11,
    NoFocusOnAppearing        = 1 << 12,
    NoBringToFrontOnFocus     = 1 << 13,
    AlwaysVerticalScrollBar   = 1 << 14,
    AlwaysHorizontalScrollBar = 1 << 15,
    NoNavInputs               = 1 << 16,
    NoNavFocus                = 1 << 17,
    UnsavedDocument           = 1 << 18,
};

enum class ImGuiChildOpts
{
    None = 0,
};

enum class ImGuiFocusedOpts
{
    None = 0,
};

enum class ImGuiHoveredOpts
{
    None = 0,
};

enum class ImGuiSliderOpts
{
    None = 0,
};

enum class ImGuiInputTextOpts
{
    None = 0,
};

enum class ImGuiPopupOpts
{
    None = 0,
    One  = 1,
};

enum class ImGuiTableOpts
{
    None = 0,
};

enum class ImGuiCondition
{
    None         = 0,
    Always       = 1 << 0,
    Once         = 1 << 1,
    FirstUseEver = 1 << 2,
    Appearing    = 1 << 3,
};

enum class ImGuiSelectableOpts
{
    None = 0,
};

enum class ImGuiTableColumnOpts
{
    None = 0,
};

/// Represents an interface to the Dear ImGui integration of Polly.
///
/// This mirrors the Dear ImGui API with C++ idiomatic modernizations,
/// tailored to be used with Polly exclusively.
///
/// @note You can't create a valid ImGui object directly.
///       Instead, you have to attach a callback function to the game's
///       ImGui event using `Game::OnImGui()`.
///
///       The callback function then receives a valid ImGui object that
///       you can use to perform ImGui drawing.
///
/// @tip For detailed tutorials, visit https://github.com/ocornut/imgui/wiki.
class ImGui
{
    PollyObject(ImGui);

  public:
    void beginWindow(StringView name, bool* isOpen = nullptr, ImGuiWindowOpts flags = ImGuiWindowOpts::None);

    void endWindow();

    void beginChildWindow(
        StringView      id,
        Maybe<Vec2>     size        = {},
        ImGuiChildOpts  childFlags  = ImGuiChildOpts::None,
        ImGuiWindowOpts windowFlags = ImGuiWindowOpts::None);

    void endChildWindow();

    [[nodiscard]]
    bool isWindowAppearing();

    [[nodiscard]]
    bool isWindowCollapsed();

    [[nodiscard]]
    bool isWindowFocused(ImGuiFocusedOpts flags = ImGuiFocusedOpts::None);

    [[nodiscard]]
    bool isWindowHovered(ImGuiHoveredOpts flags = ImGuiHoveredOpts::None);

    [[nodiscard]]
    Vec2 windowPosition();

    [[nodiscard]]
    Vec2 windowSize();

    [[nodiscard]]
    float windowWidth();

    [[nodiscard]]
    float windowHeight();

    void setNextWindowPosition(Vec2 position, ImGuiCondition cond = ImGuiCondition::None);

    void setNextWindowSize(Vec2 size, ImGuiCondition cond = ImGuiCondition::None);

    void setNextWindowCollapsed(bool collapsed, ImGuiCondition cond = ImGuiCondition::None);

    void setWindowPosition(StringView name, Vec2 position, ImGuiCondition cond = ImGuiCondition::None);

    void setWindowSize(StringView name, Vec2 size, ImGuiCondition cond = ImGuiCondition::None);

    void setWindowCollapsed(StringView name, bool collapsed, ImGuiCondition cond = ImGuiCondition::None);

    void setWindowFocus(StringView name);

    void separator();

    void sameLine(float offsetFromStartX = 0.0f, float spacing = -1.0f);

    void newLine();

    void spacing();

    void dummy(Vec2 size);

    void indent(float indentW = 0.0f);

    void unindent(float indentW = 0.0f);

    void beginGroup();

    void endGroup();

    void alignTextToFramePadding();

    float textLineHeight();

    float textLineHeightWithSpacing();

    float frameHeight();

    float frameHeightWithSpacing();

    void textUnformatted(StringView text);

    void text(StringView fmt, ...);

    void textColored(Color color, StringView fmt, ...);

    void textDisabled(StringView fmt, ...);

    void textWrapped(StringView fmt, ...);

    void labelText(StringView label, StringView fmt, ...);

    void bulletText(StringView fmt, ...);

    void separatorWithText(StringView label);

    [[nodiscard]]
    bool button(StringView label, Maybe<Vec2> size = {});

    [[nodiscard]]
    bool smallButton(StringView label);

    [[nodiscard]]
    bool invisibleButton(StringView id, Vec2 size, ImGuiButtonOpts flags = ImGuiButtonOpts::None);

    [[nodiscard]]
    bool arrowButton(StringView id, Direction direction);

    bool checkbox(StringView label, bool& isChecked);

    bool checkboxFlags(StringView label, int& flags, int flagsValue);

    bool radioButton(StringView label, bool isActive);

    bool radioButton(StringView label, int& value, int valueButton);

    void progressBar(float fraction, Maybe<Vec2> size = {}, StringView overlay = {});

    void bullet();

    bool textLink(StringView label);

    void textLinkOpenUrl(StringView label, StringView url);

    void image(
        Image image,
        Vec2  imageSize,
        Vec2  uv0         = Vec2(0, 0),
        Vec2  uv1         = Vec2(1, 1),
        Color tintColor   = white,
        Color borderColor = transparent);

    void imageButton(
        StringView   id,
        Polly::Image image,
        Vec2         imageSize,
        Vec2         uv0             = Vec2(0, 0),
        Vec2         uv1             = Vec2(1, 1),
        Color        backgroundColor = transparent,
        Color        borderColor     = white);

    [[nodiscard]]
    bool beginCombo(StringView label, StringView previewValue, ImGuiComboOpts flags = ImGuiComboOpts::None);

    void endCombo();

    [[nodiscard]]
    bool combo(
        StringView       label,
        int&             currentItem,
        Span<StringView> items,
        Maybe<u32>       popupMaxHeightInItems = none);

    bool drag(
        StringView      label,
        float&          value,
        float           speed  = 1.0f,
        float           min    = 0.0f,
        float           max    = 0.0f,
        StringView      format = "%.3f",
        ImGuiSliderOpts flags  = ImGuiSliderOpts::None);

    bool drag(
        StringView      label,
        Vec2&           value,
        float           speed  = 1.0f,
        float           min    = 0.0f,
        float           max    = 0.0f,
        StringView      format = "%.3f",
        ImGuiSliderOpts flags  = ImGuiSliderOpts::None);

    bool drag(
        StringView      label,
        Vec3&           value,
        float           speed  = 1.0f,
        float           min    = 0.0f,
        float           max    = 0.0f,
        StringView      format = "%.3f",
        ImGuiSliderOpts flags  = ImGuiSliderOpts::None);

    bool drag(
        StringView      label,
        Vec4&           value,
        float           speed  = 1.0f,
        float           min    = 0.0f,
        float           max    = 0.0f,
        StringView      format = "%.3f",
        ImGuiSliderOpts flags  = ImGuiSliderOpts::None);

    bool drag(
        StringView      label,
        int&            value,
        float           speed  = 1.0f,
        int             min    = 0,
        int             max    = 0,
        StringView      format = "%d",
        ImGuiSliderOpts flags  = ImGuiSliderOpts::None) const;

    bool drag(
        StringView      label,
        Vec2i&          value,
        float           speed  = 1.0f,
        int             min    = 0,
        int             max    = 0,
        StringView      format = "%d",
        ImGuiSliderOpts flags  = ImGuiSliderOpts::None);

    bool drag(
        StringView      label,
        Vec3i&          value,
        float           speed  = 1.0f,
        int             min    = 0,
        int             max    = 0,
        StringView      format = "%d",
        ImGuiSliderOpts flags  = ImGuiSliderOpts::None);

    bool drag(
        StringView      label,
        Vec4i&          value,
        float           speed  = 1.0f,
        int             min    = 0,
        int             max    = 0,
        StringView      format = "%d",
        ImGuiSliderOpts flags  = ImGuiSliderOpts::None);

    bool slider(
        StringView      label,
        float&          value,
        float           min,
        float           max,
        StringView      format = "%.3f",
        ImGuiSliderOpts flags  = ImGuiSliderOpts::None);

    bool slider(
        StringView      label,
        Vec2&           value,
        float           min,
        float           max,
        StringView      format = "%.3f",
        ImGuiSliderOpts flags  = ImGuiSliderOpts::None);

    bool slider(
        StringView      label,
        Vec3&           value,
        float           min,
        float           max,
        StringView      format = "%.3f",
        ImGuiSliderOpts flags  = ImGuiSliderOpts::None);

    bool slider(
        StringView      label,
        Vec4&           value,
        float           min,
        float           max,
        StringView      format = "%.3f",
        ImGuiSliderOpts flags  = ImGuiSliderOpts::None);

    bool sliderAngle(
        StringView      label,
        Degrees&        value,
        Degrees         min    = Degrees(-360),
        Degrees         max    = Degrees(360),
        StringView      format = "%.0f deg",
        ImGuiSliderOpts flags  = ImGuiSliderOpts::None);

    bool slider(
        StringView      label,
        int&            value,
        int             min,
        int             max,
        StringView      format = "%d",
        ImGuiSliderOpts flags  = ImGuiSliderOpts::None);

    bool slider(
        StringView      label,
        Vec2i&          value,
        int             min,
        int             max,
        StringView      format = "%d",
        ImGuiSliderOpts flags  = ImGuiSliderOpts::None);

    bool slider(
        StringView      label,
        Vec3i&          value,
        int             min,
        int             max,
        StringView      format = "%d",
        ImGuiSliderOpts flags  = ImGuiSliderOpts::None);

    bool slider(
        StringView      label,
        Vec4i&          value,
        int             min,
        int             max,
        StringView      format = "%d",
        ImGuiSliderOpts flags  = ImGuiSliderOpts::None);

    bool sliderVertical(
        StringView      label,
        Vec2            size,
        float&          value,
        float           min,
        float           max,
        StringView      format = "%.3f",
        ImGuiSliderOpts flags  = ImGuiSliderOpts::None);

    bool sliderVertical(
        StringView      label,
        Vec2            size,
        int&            value,
        int             min,
        int             max,
        StringView      format = "%d",
        ImGuiSliderOpts flags  = ImGuiSliderOpts::None);

    bool inputText(StringView label, String& value, ImGuiInputTextOpts flags = ImGuiInputTextOpts::None);

    bool inputTextMultiline(
        StringView         label,
        String&            value,
        Maybe<Vec2>        size  = {},
        ImGuiInputTextOpts flags = ImGuiInputTextOpts::None);

    bool inputTextWithHint(
        StringView         label,
        StringView         hint,
        String&            value,
        ImGuiInputTextOpts flags = ImGuiInputTextOpts::None);

    bool input(
        StringView         label,
        float&             value,
        float              step     = 0.0f,
        float              stepFast = 0.0f,
        StringView         format   = "%.3f",
        ImGuiInputTextOpts flags    = ImGuiInputTextOpts::None);

    bool input(
        StringView         label,
        Vec2&              value,
        StringView         format = "%.3f",
        ImGuiInputTextOpts flags  = ImGuiInputTextOpts::None);

    bool input(
        StringView         label,
        Vec3&              value,
        StringView         format = "%.3f",
        ImGuiInputTextOpts flags  = ImGuiInputTextOpts::None);

    bool input(
        StringView         label,
        Vec4&              value,
        StringView         format = "%.3f",
        ImGuiInputTextOpts flags  = ImGuiInputTextOpts::None);

    bool input(
        StringView         label,
        int&               value,
        int                step     = 1,
        int                stepFast = 100,
        ImGuiInputTextOpts flags    = ImGuiInputTextOpts::None);

    bool input(StringView label, Vec2i& value, ImGuiInputTextOpts flags = ImGuiInputTextOpts::None);

    bool input(StringView label, Vec3i& value, ImGuiInputTextOpts flags = ImGuiInputTextOpts::None);

    bool input(StringView label, Vec4i& value, ImGuiInputTextOpts flags = ImGuiInputTextOpts::None);

    bool colorEdit(StringView label, Color& value, ImGuiColorEditOpts flags = ImGuiColorEditOpts::None);

    bool colorEditRgb(StringView label, Color& value, ImGuiColorEditOpts flags = ImGuiColorEditOpts::None);

    bool colorPicker(StringView label, Color& value, ImGuiColorEditOpts flags = ImGuiColorEditOpts::None);

    bool colorPickerRgb(StringView label, Color& value, ImGuiColorEditOpts flags = ImGuiColorEditOpts::None);

    bool colorButton(
        StringView         id,
        Color              color,
        ImGuiColorEditOpts flags = ImGuiColorEditOpts::None,
        Maybe<Vec2>        size  = {});

    bool treeNode(StringView label);

    bool treeNode(StringView id, StringView fmt, ...);

    bool treeNodeEx(StringView id, ImGuiTreeNodeOpts flags, StringView fmt, ...);

    void treePush(const void* id);

    void treePop();

    float treeNodeToLabelSpacing();

    bool collapsingHeader(StringView label, ImGuiTreeNodeOpts flags = ImGuiTreeNodeOpts::None);

    bool collapsingHeader(
        StringView        label,
        bool&             isVisible,
        ImGuiTreeNodeOpts flags = ImGuiTreeNodeOpts::None);

    void setNextItemOpen(bool isOpen, ImGuiCondition cond = ImGuiCondition::None);

    bool selectable(
        StringView          label,
        bool                isSelected = false,
        ImGuiSelectableOpts flags      = ImGuiSelectableOpts::None,
        Maybe<Vec2>         size       = {});

    bool selectableWithBinding(
        StringView          label,
        bool&               isSelected,
        ImGuiSelectableOpts flags = ImGuiSelectableOpts::None,
        Maybe<Vec2>         size  = {});

    bool beginListBox(StringView label, Maybe<Vec2> size = {});

    void endListBox();

    bool listBox(StringView label, int& currentItem, Span<StringView> items, Maybe<u32> heightInItems = {});

    void value(StringView prefix, bool value);

    void value(StringView prefix, int value);

    void value(StringView prefix, unsigned int value);

    void value(StringView prefix, float value, StringView floatFormat = "");

    bool beginMenuBar();

    void endMenuBar();

    bool beginMainMenuBar();

    void endMainMenuBar();

    bool beginMenu(StringView label, bool enabled = true);

    void endMenu();

    bool menuItem(StringView label, StringView shortcut = "", bool selected = false, bool enabled = true);

    bool menuItemWithBinding(StringView label, StringView shortcut, bool& isSelected, bool enabled = true);

    bool beginTooltip();

    void endTooltip();

    void setTooltip(StringView fmt, ...);

    bool beginItemTooltip();

    void setItemTooltip(StringView fmt, ...);

    bool beginPopup(StringView id, ImGuiWindowOpts flags = ImGuiWindowOpts::None);

    bool beginPopupModal(StringView name, bool& isOpen, ImGuiWindowOpts flags = ImGuiWindowOpts::None);

    void endPopup();

    void openPopup(StringView id, ImGuiPopupOpts flags = ImGuiPopupOpts::None);

    void openPopupOnItemClick(StringView id = "", ImGuiPopupOpts flags = ImGuiPopupOpts::One);

    void closeCurrentPopup();

    bool beginPopupContextItem(StringView id = "", ImGuiPopupOpts flags = ImGuiPopupOpts::One);

    bool beginPopupContextWindow(StringView id = "", ImGuiPopupOpts flags = ImGuiPopupOpts::One);

    bool beginPopupContextVoid(StringView id = "", ImGuiPopupOpts flags = ImGuiPopupOpts::One);

    bool isPopupOpen(StringView id, ImGuiPopupOpts flags = ImGuiPopupOpts::One) const;

    [[nodiscard]]
    bool beginTable(
        StringView     id,
        u32            columns,
        ImGuiTableOpts flags      = ImGuiTableOpts::None,
        Maybe<Vec2>    outerSize  = none,
        Maybe<float>   innerWidth = none);

    void endTable();

    void tableNextRow(ImGuiTableRowOpts flags = ImGuiTableRowOpts::None, Maybe<float> minRowHeight = {});

    bool tableNextColumn();

    bool tableSetColumnIndex(int column);

    void tableSetupColumn(StringView label, ImGuiTableColumnOpts flags = ImGuiTableColumnOpts::None);

    void tableSetupScrollFreeze(u32 cols, u32 rows);

    void tableHeader(StringView label);

    void tableHeadersRow();

    void tableAngledHeadersRow();

    [[nodiscard]]
    int tableColumnCount() const;

    [[nodiscard]]
    int tableColumnIndex() const;

    [[nodiscard]]
    int tableRowIndex() const;

    [[nodiscard]]
    StringView tableColumnName(Maybe<int> column) const;

    void setItemDefaultFocus();

    void setNavCursorVisible(bool value);

    void setNextItemAllowOverlap();

    [[nodiscard]]
    bool isItemHovered(ImGuiHoveredOpts flags = ImGuiHoveredOpts::None) const;

    [[nodiscard]]
    bool isItemActive() const;

    [[nodiscard]]
    bool isItemFocused() const;

    [[nodiscard]]
    bool isItemClicked(MouseButton button) const;

    [[nodiscard]]
    bool isItemVisible() const;

    [[nodiscard]]
    bool isItemEdited() const;

    [[nodiscard]]
    bool isItemActivated() const;

    [[nodiscard]]
    bool isItemDeactivated() const;

    [[nodiscard]]
    bool isItemDeactivatedAfterEdit() const;

    [[nodiscard]]
    bool isItemToggledOpen() const;

    [[nodiscard]]
    bool isAnyItemHovered() const;

    [[nodiscard]]
    bool isAnyItemActive() const;

    [[nodiscard]]
    bool isAnyItemFocused() const;

    [[nodiscard]]
    Rectangle itemRect() const;
};

PollyDefineEnumFlagOperations(ImGuiWindowOpts);
PollyDefineEnumFlagOperations(ImGuiTableRowOpts);
PollyDefineEnumFlagOperations(ImGuiColorEditOpts);
PollyDefineEnumFlagOperations(ImGuiComboOpts);
PollyDefineEnumFlagOperations(ImGuiButtonOpts);
PollyDefineEnumFlagOperations(ImGuiTreeNodeOpts);
PollyDefineEnumFlagOperations(ImGuiChildOpts);
PollyDefineEnumFlagOperations(ImGuiFocusedOpts);
PollyDefineEnumFlagOperations(ImGuiHoveredOpts);
PollyDefineEnumFlagOperations(ImGuiSliderOpts);
PollyDefineEnumFlagOperations(ImGuiInputTextOpts);
} // namespace Polly
