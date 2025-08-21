# Copyright (C) 2025 Cemalettin Dervis This file is part of Polly.
# For conditions of distribution and use, see copyright notice in LICENSE.

if (APPLE)
    set(polly_have_gfx_metal TRUE)
else ()
    set(polly_have_gfx_vulkan TRUE)
endif ()

add_library(Polly STATIC)
target_compile_features(Polly PUBLIC cxx_std_20)

set(polly_include_dir ${polly_root_dir}/Include)
set(polly_src_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(polly_deps_headers_dir ${FETCHCONTENT_BASE_DIR}/DepsHeaders)

include(../Misc/DownloadDepFile.cmake)
include(ThirdParty.cmake)

file(GLOB_RECURSE polly_public_files CONFIGURE_DEPENDS "../Include/Polly/*.hpp")
file(GLOB_RECURSE polly_inline_files CONFIGURE_DEPENDS "../Include/Polly/Details/*.inl")
target_sources(Polly PRIVATE ${polly_public_files} ${polly_inline_files})

target_include_directories(Polly
    PUBLIC ${polly_include_dir} ${polly_generated_public_headers_dir}
    PRIVATE ${polly_src_dir} ${polly_generated_private_headers_dir} ${polly_deps_headers_dir}
)

source_group("Public" FILES ${polly_public_files})
source_group("Public\\Inline" FILES ${polly_inline_files})

set(base_dir "Polly")
file(GLOB sources_list CONFIGURE_DEPENDS LIST_DIRECTORIES TRUE "${base_dir}/*")

foreach (dir ${sources_list})
    if (IS_DIRECTORY ${dir})
        get_filename_component(dir_name ${dir} NAME_WE)
        list(APPEND src_sub_dirs ${dir_name})
    endif ()
endforeach ()

foreach (dir ${src_sub_dirs})
    set(module_dir "${base_dir}/${dir}")
    file(GLOB module_header_files CONFIGURE_DEPENDS "${module_dir}/*.hpp")
    file(GLOB module_source_files CONFIGURE_DEPENDS "${module_dir}/*.cpp")

    target_sources(Polly
        PRIVATE FILE_SET HEADERS
        BASE_DIRS ${module_dir}
        FILES ${module_header_files}
        PRIVATE ${module_header_files} ${module_source_files}
    )

    if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/${module_dir}/CMakeLists.txt")
        add_subdirectory(${module_dir})
    endif ()

    source_group("Private\\${dir}" FILES ${module_header_files} ${module_source_files})
endforeach ()

set_target_properties(Polly PROPERTIES FOLDER "Polly")

if (BUILD_TESTING)
    target_compile_definitions(Polly PUBLIC -DPOLLY_ENABLE_TESTS)
endif ()

if (ANDROID)
    target_link_libraries(Polly PUBLIC dl m atomic stdc++ android log)

    if (ANDROID_ABI STREQUAL armeabi-v7a)
        set_target_properties(Polly PROPERTIES COMPILE_FLAGS -mfpu=neon)
    endif ()
endif ()

if (IOS)
    set_target_properties(
        Polly
        PROPERTIES FRAMEWORK FALSE
        MACOSX_FRAMEWORK_IDENTIFIER com.cdervis.Polly
        VERSION ${CMAKE_PROJECT_VERSION}
        SOVERSION ${CMAKE_PROJECT_VERSION}
    )
endif ()

enable_default_cpp_flags(Polly)
enable_clang_tidy(Polly)

if (POLLY_ENABLE_VERBOSE_LOGGING)
    target_compile_definitions(Polly PRIVATE -DENABLE_VERBOSE_LOGGING)
endif ()

# Embed non-code files directly into the binary.
get_target_property(polly_sources Polly SOURCES)

foreach (file ${polly_sources})
    get_filename_component(ext ${file} EXT)

    if (NOT ext STREQUAL ".cpp"
        AND NOT ext STREQUAL ".hpp"
        AND NOT ext STREQUAL ".h"
        AND NOT ext STREQUAL ".c"
        AND NOT ext STREQUAL ".m"
        AND NOT ext STREQUAL ".inl"
        AND NOT ext STREQUAL ".mm")
        # Embed the resource as-is.
        polly_embed_file(Polly ${file})
    endif ()
endforeach ()

# Precompiled headers
target_precompile_headers(Polly PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:<Polly/List.hpp$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<Polly/Details/ListBase.hpp$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<Polly/Details/ListData.hpp$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<Polly/String.hpp$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<Polly/StringView.hpp$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<Polly/Algorithm.hpp$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<Polly/Maybe.hpp$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<Polly/Details/magic_enum.hpp$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<memory$<ANGLE-R>>"
)

target_precompile_headers(Spine PRIVATE
    <spine/SpineObject.h>
    <spine/Vector.h>
)

if (WIN32)
    target_compile_definitions(Polly PRIVATE -DNOMINMAX -DWIN32_LEAN_AND_MEAN)
    target_precompile_headers(Polly PRIVATE <windows.h>)
endif()

