if (polly_have_gfx_metal)
    polly_log("Enabling Metal support")
    add_subdirectory(Metal)
endif ()

if (polly_have_gfx_d3d11)
    polly_log("Enabling D3D11 support")
    add_subdirectory(D3D11)
endif()

if (polly_have_gfx_opengl)
    polly_log("Enabling OpenGL support")
    add_subdirectory(OpenGL)
endif()

if (polly_have_gfx_vulkan)
    polly_log("Enabling Vulkan support")
    add_subdirectory(Vulkan)
endif ()

target_sources(Polly PRIVATE Resources/Noto.ttf)

