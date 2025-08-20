if (polly_have_gfx_metal)
    polly_log("Enabling Metal support")
    add_subdirectory(Metal)
endif ()

if (polly_have_gfx_vulkan)
    polly_log("Enabling Vulkan support")
    add_subdirectory(Vulkan)
endif ()

target_sources(Polly PRIVATE Resources/Noto.ttf)

