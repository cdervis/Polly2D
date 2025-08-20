# stb
CPMAddPackage(
    NAME stb
    GIT_REPOSITORY https://github.com/cdervis/stb
    GIT_TAG v1.0
    GIT_SHALLOW
)

target_link_libraries(Polly PRIVATE stb)
list(APPEND targets_in_polly_group stb)

# SDL
set(sdl_x11 OFF)
set(sdl_wayland OFF)

if (NOT APPLE AND NOT WIN32 AND NOT ANDROID)
    set(sdl_x11 ON)
    set(sdl_wayland ON)
endif ()

CPMAddPackage(
    NAME SDL
    VERSION v3.2.20
    URL https://github.com/cdervis/SDL/archive/refs/tags/v3.2.20.zip
    OPTIONS
    "SDL_STATIC ON"
    "SDL_CLOCK_GETTIME OFF"
    "SDL_DISABLE_INSTALL ON"
    "SDL_DISABLE_UNINSTALL ON"
    "SDL_DIALOG OFF"
    "SDL_OPENGL OFF"
    "SDL_VULKAN ${polly_have_gfx_vulkan}"
    "SDL_METAL ${polly_have_gfx_metal}"
    "SDL_OFFSCREEN OFF"
)

target_link_libraries(Polly PRIVATE SDL3-static)

if (TARGET SDL3-shared)
    list(APPEND targets_in_polly_group SDL3-shared)
endif ()

if (TARGET SDL3-static)
    list(APPEND targets_in_polly_group SDL3-static)
endif ()

if (TARGET SDL_uclibc)
    list(APPEND targets_in_polly_group SDL_uclibc)
endif ()

# zlib
set(zlib_ng_with_optim OFF)

if (IOS)
    set(zlib_ng_with_optim OFF)
endif()

CPMAddPackage(
    NAME zlib-ng
    GIT_REPOSITORY https://github.com/zlib-ng/zlib-ng
    GIT_TAG 2.2.4
    OPTIONS
    "ZLIB_COMPAT OFF"
    "ZLIB_ENABLE_TESTS OFF"
    "ZLIBNG_ENABLE_TESTS OFF"
    "WITH_OPTIM ${zlib_ng_with_optim}"
    GIT_SHALLOW)

target_link_libraries(Polly PRIVATE zlib)
list(APPEND targets_in_polly_group zlib)

foreach (targetName ${targets_in_polly_group})
    set_target_properties(${targetName} PROPERTIES FOLDER "Third-Party")
endforeach ()
