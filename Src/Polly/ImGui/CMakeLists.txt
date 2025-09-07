CPMAddPackage(
    NAME imgui
    URL https://github.com/cdervis/imgui/archive/refs/tags/v1.92.2.zip
    DOWNLOAD_ONLY
)

set(base_dir ${imgui_SOURCE_DIR})

target_include_directories(Polly PRIVATE ${base_dir})

set(imgui_files
    ${base_dir}/imgui.cpp
    ${base_dir}/imgui_draw.cpp
    ${base_dir}/imgui_tables.cpp
    ${base_dir}/imgui_widgets.cpp
    ${base_dir}/backends/imgui_impl_sdl3.h
    ${base_dir}/backends/imgui_impl_sdl3.cpp
)

target_compile_definitions(Polly PRIVATE
    -DIMGUI_DEFINE_MATH_OPERATORS
    -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS
)

if (polly_have_gfx_d3d11)
    list(APPEND imgui_files
        ${base_dir}/backends/imgui_impl_dx11.h
        ${base_dir}/backends/imgui_impl_dx11.cpp
    )
endif()

if (polly_have_gfx_metal)
    list(APPEND imgui_files
        ${base_dir}/backends/imgui_impl_metal.h
        ${base_dir}/backends/imgui_impl_metal.mm
    )

    set_source_files_properties(
        ${base_dir}/backends/imgui_impl_metal.mm
        PROPERTIES SKIP_PRECOMPILE_HEADERS ON
    )

    target_compile_definitions(Polly PRIVATE -DIMGUI_IMPL_METAL_CPP)

    target_compile_options(Polly PRIVATE -Wno-arc-bridge-casts-disallowed-in-nonarc)
endif ()

if (polly_have_gfx_opengl)
    list(APPEND imgui_files
        ${base_dir}/backends/imgui_impl_opengl3.h
        ${base_dir}/backends/imgui_impl_opengl3.cpp
    )
endif ()

if (polly_have_gfx_vulkan)
    list(APPEND imgui_files
        ${base_dir}/backends/imgui_impl_vulkan.h
        ${base_dir}/backends/imgui_impl_vulkan.cpp
    )

    target_compile_definitions(Polly PRIVATE
        -DIMGUI_IMPL_VULKAN_NO_PROTOTYPE
        -DIMGUI_IMPL_VULKAN_USE_VOLK
    )
endif ()

target_sources(Polly PRIVATE ${imgui_files})
