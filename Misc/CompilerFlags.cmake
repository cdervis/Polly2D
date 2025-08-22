# General build flags
if (NOT MSVC)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fvisibility=hidden>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fvisibility-inlines-hidden>)
endif ()

# Production build settings
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    polly_log("Enabling optimizations for Release mode")

    include(CheckIPOSupported)
    check_ipo_supported(RESULT lto_supported OUTPUT error)

    if (lto_supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ${ltoSupported})
    endif ()

    if (NOT MSVC)
        add_compile_options(-O3)
    endif ()

    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

# Address sanitizer
if (POLLY_ENABLE_ADDRESS_SANITIZER AND NOT MSVC)
    polly_log("Enabling address sanitizer")

    set(CMAKE_XCODE_SCHEME_ADDRESS_SANITIZER ON)
    set(CMAKE_XCODE_SCHEME_ADDRESS_SANITIZER_USE_AFTER_RETURN ON)

    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif ()

function(enable_default_cpp_flags target_name)
    set_target_properties(
        ${target_name}
        PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${polly_binary_dir}/bin
        LIBRARY_OUTPUT_DIRECTORY ${polly_binary_dir}/bin
        RUNTIME_OUTPUT_DIRECTORY ${polly_binary_dir}/bin)

    if (MSVC)
        # Warning level 4, warnings as errors etc.
        target_compile_options(${target_name} PRIVATE /W4 /WX /MP /utf-8)

        # Ignore warnings about sprintf etc.
        target_compile_definitions(${target_name} PRIVATE -D_CRT_SECURE_NO_WARNINGS)
    else ()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic -Werror -Wfatal-errors)

        # Disable some of the warnings
        target_compile_options(${target_name} PRIVATE -Wno-nonnull)
    endif ()

    if (WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_definitions(${target_name} PRIVATE -D_CRT_SECURE_NO_WARNINGS)
    endif ()

    # Disable common warnings
    if (MSVC)
        target_compile_options(
            ${target_name}
            PRIVATE /wd4100 # Unreferenced formal parameter
            /wd4505 # Unreferenced function with internal linkage has been removed
            /wd4251 # DLL export warnings
            /wd4458 # Declaration of xyz hides class member
        )
    else ()
        target_compile_options(${target_name} PRIVATE
            -Wno-unused-parameter
            -Wno-comment
            -Wno-unused-function
            -Wno-psabi
            -Wno-missing-braces
            -fvisibility=hidden
        )

        if (GCC)
            target_compile_options(${target_name} PRIVATE -Wnrvo)
        endif ()

        if (APPLE)
            target_compile_options(${target_name} PRIVATE
                -Wno-dtor-name
                -Wno-unused-private-field
                -Wno-gnu-anonymous-struct
                -Wno-nested-anon-types
                -Wno-nontrivial-memaccess
            )
        endif ()
    endif ()

    set(CMAKE_XCODE_GENERATE_SCHEME ON)

    if (ANDROID)
        # Non-standard type char_traits<...> is not supported beginning with LLVM 19. But we need it right now, so
        # disable those warnings.
        target_compile_options(${target_name} PRIVATE -Wno-deprecated-declarations)
    endif ()

    if (NOT APPLE AND POLLY_DEPLOY)
        # TODO: static link libstdc++/libc++/msvc
        #-static-libstdc++ and -static-libgcc
    endif()
endfunction()

function(enable_clang_tidy target_name)
    if (POLLY_ENABLE_CLANG_TIDY AND NOT MSVC AND NOT APPLE)
        polly_log("Enabling clang-tidy checks for the build")

        find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
        if (CLANG_TIDY_EXE)
            set(CLANG_TIDY_COMMAND
                "${CLANG_TIDY_EXE}" "-p" "${CMAKE_BINARY_DIR}" "--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy")

            set_target_properties(${target_name} PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
        else ()
            polly_warn("clang-tidy executable not found; ignoring")
        endif ()
    endif ()
endfunction()
