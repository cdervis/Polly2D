// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Core/Object.hpp"
#include "Polly/ImGui.hpp"
#include "Polly/LinkedList.hpp"
#include "Polly/List.hpp"

#include <imgui.h>

namespace Polly
{
class ImGui::Impl final : public Object
{
  public:
    ~Impl() noexcept override;

    struct StringPtrList
    {
        List<const char*, 32> ptrs;
    };

    ImGuiContext*           imGuiContext = nullptr;
    LinkedList<String>      tmpStrings;
    List<StringPtrList, 32> tmpStringPtrLists;
};
} // namespace Polly
