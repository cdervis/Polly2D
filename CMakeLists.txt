# Copyright (C) 2025 Cemalettin Dervis
# This file is part of Polly, a lightweight 2D Game Framework for C++.
# For conditions of distribution and use, see copyright notice in LICENSE.

cmake_minimum_required(VERSION 3.22)

file(READ "Version.txt" polly_version)

if (polly_version STREQUAL "")
    message(FATAL_ERROR "No version found in Version.txt!")
endif ()

if (NOT polly_version MATCHES "[0-9]+\.[0-9]+\.[0-9]+$")
    message(FATAL_ERROR "Invalid version '${polly_version}' specified in Version.txt!")
endif ()

message(STATUS "Configuring Polly version ${polly_version}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(BUILD_SHARED_LIBS OFF)

#set(CMAKE_OSX_ARCHITECTURES "arm64")
#set(CMAKE_XCODE_ATTRIBUTE_OSX_DEPLOYMENT_TARGET "13.5")
#set(CMAKE_XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "13.5")
#set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "16.5")
#set(CMAKE_XCODE_GENERATE_SCHEME ON)

set(polly_languages C CXX)

if (APPLE)
    list(APPEND polly_languages OBJC OBJCXX)
endif()

project(
    Polly
    DESCRIPTION "Polly is a 2D game framework for C++"
    HOMEPAGE_URL "https://polly2d.org"
    LANGUAGES ${polly_languages}
    VERSION ${polly_version})

if (APPLE)
    include(CheckOBJCSourceCompiles)
    enable_language(OBJC)
endif ()

include(Misc/Util.cmake)

if (CMAKE_BUILD_TYPE STREQUAL "")
    polly_warn("CMAKE_BUILD_TYPE is not set. This is OK, but certain compiler flags and optimizations will not be enabled.")
endif ()

# Get CPM
file(
    DOWNLOAD
    https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.0/CPM.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/CMake/CPM.cmake
    EXPECTED_HASH SHA256=2020b4fc42dba44817983e06342e682ecfc3d2f484a581f11cc5731fbe4dce8a
)

include(${CMAKE_CURRENT_BINARY_DIR}/CMake/CPM.cmake)

find_package(Python3 COMPONENTS Interpreter REQUIRED)

# Set up some base directories first.
set(polly_root_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(polly_binary_dir ${CMAKE_CURRENT_BINARY_DIR})
set(polly_generated_public_headers_dir ${polly_binary_dir}/GeneratedPublicHeaders)
set(polly_generated_private_headers_dir ${polly_binary_dir}/GeneratedPrivateHeaders)
set(build_tool_dir ${CMAKE_CURRENT_SOURCE_DIR}/BuildTool)

include(Misc/Options.cmake)
include(Misc/CompilerFlags.cmake)
include(Misc/EmbedFile.cmake)
include(Misc/PollyAddGame.cmake)

configure_file(
    ${polly_root_dir}/Misc/Version.hpp.in
    ${polly_generated_public_headers_dir}/Polly/Version.hpp)

add_subdirectory(Src)
add_subdirectory(Apps)

# TODO: natvis support (issue #32)
#if (MSVC)
#    set(natvis_file ${CMAKE_CURRENT_SOURCE_DIR}/Misc/DebugVis/VS/Polly.natvis)
#    target_link_options(Polly PRIVATE /NATVIS:${natvis_file})
#    target_sources(Polly PRIVATE ${natvis_file})
#endif ()

target_sources(Polly PRIVATE ${misc_cmake_files_in_sources})
source_group("Misc" FILES ${misc_cmake_files_in_sources})

if (ANDROID)
    target_compile_options(Polly PRIVATE -Wno-nullability-extension -Wno-nullability-completeness)
endif ()
