// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/ImGui/ImGuiImpl.hpp"

#include "Polly/Logging.hpp"

#include <backends/imgui_impl_sdl3.h>

namespace Polly
{
ImGui::Impl::~Impl() noexcept
{
    if (imGuiContext)
    {
        logVerbose("Destroying ImGui");
        ImGui_ImplSDL3_Shutdown();
        ::ImGui::DestroyContext(imGuiContext);
    }
}
} // namespace Polly
