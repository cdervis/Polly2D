set(vulkan_sdk_version 1.4.321)

file(GLOB vulkan_header_files CONFIGURE_DEPENDS "*.hpp")
file(GLOB vulkan_source_files CONFIGURE_DEPENDS "*.cpp")

target_sources(Polly PRIVATE
    FILE_SET HEADERS
    FILES ${vulkan_header_files}
    PRIVATE ${vulkan_source_files}
)

# Fetch Vulkan memory allocator library.
polly_download_dep_file(
    vk_mem_alloc.h
    https://raw.githubusercontent.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/refs/tags/v3.3.0/include/vk_mem_alloc.h
    SHA256=90ce12fc4a2466235a09ae02905dd0c13aee80c1bbf11b331ab61230c2ceb112
)

# Vulkan headers
CPMAddPackage(
    NAME VulkanHeaders
    URL https://github.com/KhronosGroup/Vulkan-Headers/archive/refs/tags/vulkan-sdk-${vulkan_sdk_version}.0.zip
    DOWNLOAD_ONLY
)

target_include_directories(Polly PRIVATE ${VulkanHeaders_SOURCE_DIR}/include)

# volk
polly_download_dep_file(
    volk.h
    https://raw.githubusercontent.com/zeux/volk/refs/heads/vulkan-sdk-${vulkan_sdk_version}/volk.h
    SHA256=4bb1510156da18c2ad5e2b9b25cd89cbc181726237674731b3f328804b07795b
)

polly_download_dep_file(
    volk.c
    https://raw.githubusercontent.com/zeux/volk/refs/heads/vulkan-sdk-${vulkan_sdk_version}/volk.c
    SHA256=5aaf2a8f3c7da950aa0c05abdb7af472c433b079d14f087974565ac9fa2694a4
)

target_sources(Polly PRIVATE ${polly_deps_headers_dir}/volk.c)

set_source_files_properties(${polly_deps_headers_dir}/volk.c
    PROPERTIES SKIP_PRECOMPILE_HEADERS ON
)

set_source_files_properties(${polly_deps_headers_dir}/volk.c PROPERTIES LANGUAGE CXX)

# GLSlang for compiling our GLSL shaders to SPIR-V at runtime.
# We only need the standalone library, no executables.
CPMAddPackage(
    NAME glslang
    URL https://github.com/KhronosGroup/glslang/archive/refs/tags/15.4.0.zip
    OPTIONS
    "ENABLE_OPT OFF"
    "ENABLE_EXCEPTIONS ON"
    "BUILD_WERROR ON"
    "BUILD_SHARED_LIBS OFF"
    "ENABLE_PCH ON"
    "ENABLE_HLSL OFF"
    "GLSLANG_TESTS OFF"
    "GLSLANG_ENABLE_INSTALL OFF"
    "ENABLE_GLSLANG_BINARIES OFF"
    "ALLOW_EXTERNAL_SPIRV_TOOLS OFF"
    "ENABLE_SPVREMAPPER OFF"
    "ENABLE_GLSLANG_JS OFF"
)

if (WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Clang on Windows!")
    target_compile_definitions(glslang PRIVATE -D_CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(glslang-default-resource-limits PRIVATE -D_CRT_SECURE_NO_WARNINGS)
endif ()

target_link_libraries(Polly PRIVATE
    glslang::glslang
    glslang::glslang-default-resource-limits
)

# We're using Vulkan 1.0.
target_compile_definitions(Polly PRIVATE
    -Dpolly_have_gfx_vulkan
    -DVMA_VULKAN_VERSION=1000000
)

# Embed GLSL shader files into the Polly library.
# They're compiled at runtime using glslang instead of offline during build time.
# This provides some flexibility depending on the target system and configuration.
# The trade-off is that we won't be able to catch shader errors during build time (unless we implement such a step).
target_sources(Polly PRIVATE
    Resources/SpriteBatchVs.vert
    Resources/SpriteBatchPsDefault.frag
    Resources/SpriteBatchPsMonochromatic.frag
    Resources/PolyVs.vert
    Resources/PolyPs.frag
    Resources/MeshVs.vert
    Resources/MeshPs.frag
)
