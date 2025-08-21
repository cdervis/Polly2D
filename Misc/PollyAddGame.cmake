function(polly_add_game)
    set(options VERBOSE NO_PCH)
    set(one_value_args NAME COMPANY VERSION STRICT_WARNINGS VERBOSE_LOGGING)
    set(multi_value_args)

    cmake_parse_arguments(POLLY_ADD_GAME "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    set(enable_verbose_logging ${POLLY_ADD_GAME_VERBOSE})
    set(target_name ${POLLY_ADD_GAME_NAME})
    set(company ${POLLY_ADD_GAME_COMPANY})
    set(version ${POLLY_ADD_GAME_VERSION})
    set(current_src_dir ${CMAKE_CURRENT_SOURCE_DIR})
    set(binary_dir ${CMAKE_CURRENT_BINARY_DIR})
    set(assets_dir ${current_src_dir}/Assets)
    set(misc_dir ${current_src_dir}/Misc)
    set(compiled_assets_dir ${binary_dir}/CompiledAssets)
    set(xcassets_file ${misc_dir}/Assets.xcassets)
    set(game_props_file ${binary_dir}/game_props)

    file(WRITE
        ${game_props_file}
        "${target_name}-${company}-${version}"
    )

    file(SHA256 ${game_props_file} asset_encryption_key)

    if (NOT EXISTS ${xcassets_file})
        polly_fatal_error("Assets.xcassets not found! Expected at: ${xcassets_file}")
    endif ()

    if (enable_verbose_logging)
        polly_log("Adding game target '${target_name}'")
        polly_log("  Company:    ${company}")
        polly_log("  Version:    ${version}")
        polly_log("  Source Dir: ${current_src_dir}")
        polly_log("  Binary Dir: ${binary_dir}")
        polly_log("  Assets:     ${assets_dir}")
    endif ()

    if (ANDROID)
        add_library(${target_name} SHARED)
    elseif (APPLE)
        add_executable(${target_name} MACOSX_BUNDLE)
    else ()
        if (WIN32)
            add_executable(${target_name} WIN32)
        else ()
            add_executable(${target_name})
        endif ()
    endif ()

    set_target_properties(${target_name} PROPERTIES
        USE_FOLDERS TRUE
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Lib
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin
    )

    target_link_libraries(${target_name} PRIVATE Polly)

    get_target_property(target_type Polly TYPE)

    set(script_dir ${CMAKE_CURRENT_FUNCTION_LIST_DIR})

    file(GLOB_RECURSE asset_files CONFIGURE_DEPENDS "${assets_dir}/[^.]*")

    list(LENGTH asset_files asset_count)
    target_sources(${target_name} PRIVATE ${asset_files})

    polly_log("Game has a total of ${asset_count} asset(s)")

    foreach (file ${asset_files})
        file(RELATIVE_PATH asset_name ${assets_dir} ${file})
        set(compiled_asset ${compiled_assets_dir}/${asset_name}.asset)
        list(APPEND compiled_assets ${compiled_asset})

        add_custom_command(
            OUTPUT ${compiled_asset}
            COMMAND Python3::Interpreter BuildTool compile
            --base "${assets_dir}"
            --asset "${asset_name}"
            --dst "${compiled_asset}"
            --encryptionkey "${asset_encryption_key}"
            WORKING_DIRECTORY ${polly_root_dir}
            DEPENDS ${file} ${game_props_file}
            COMMENT "Compiling ${asset_name}"
        )
    endforeach ()

    if (${asset_count} GREATER 0)
        set(asset_archive_name "data.pla")

        if (ANDROID)
            set(asset_archive_filename ${CMAKE_ANDROID_ASSETS_DIRECTORIES}/${asset_archive_name})
        else ()
            set(asset_archive_filename ${binary_dir}/${asset_archive_name})
        endif ()

        target_sources(${target_name} PRIVATE ${asset_archive_filename})

        polly_log("Asset archive: ${asset_archive_filename}")

        add_custom_command(
            OUTPUT ${asset_archive_filename}
            COMMAND Python3::Interpreter BuildTool pack
            --dst "${asset_archive_filename}"
            --encryptionkey "${asset_encryption_key}"
            ${compiled_assets}
            WORKING_DIRECTORY ${polly_root_dir}
            DEPENDS ${compiled_assets}
            COMMENT "Packing ${asset_count} asset(s)"
        )

        foreach (file ${asset_files})
            file(RELATIVE_PATH new_file ${assets_dir} ${file})
            source_group("Resources/${new_file_path}" FILES "${file}")
        endforeach ()
    endif ()

    if (APPLE)
        set(bundle_identifier "com.${company}.${target_name}")
        set(bundle_version ${version})
        set(bundle_name ${target_name})
        set(bundle_display_name ${target_name})
        set(executable_name ${target_name})
        set(metal_capture_enabled_value true)
        set(info_plist_dst_filename "${binary_dir}/Info.plist")
        configure_file("${script_dir}/Info.plist.in" ${info_plist_dst_filename})

        # TODO:
        target_sources(${target_name} PRIVATE ${info_plist_dst_filename} ${misc_dir}/Assets.xcassets)

        set_target_properties(${target_name} PROPERTIES
            MACOSX_BUNDLE_GUI_IDENTIFIER ${bundle_display_name}
            MACOSX_BUNDLE_INFO_PLIST ${info_plist_dst_filename}
            XCODE_ATTRIBUTE_PRODUCT_NAME ${bundle_display_name}
            XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME AppIcon
            XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES YES
            XCODE_ATTRIBUTE_OSX_DEPLOYMENT_TARGET "13.5"
            XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "13.5"
            XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "16.5"
            XCODE_LINK_BUILD_PHASE_MODE BUILT_ONLY
            BUILD_WITH_INSTALL_RPATH TRUE
            INSTALL_RPATH "@executable_path/"
        )

        if (IOS)
            set_target_properties(${target_name} PROPERTIES XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2")
        endif ()

        if (${asset_count} GREATER 0)
            set_property(SOURCE ${asset_archive_filename} PROPERTY MACOSX_PACKAGE_LOCATION "Resources")
        endif ()

        set_source_files_properties(${misc_dir}/Assets.xcassets PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    else ()
        if (${asset_count} GREATER 0)
            add_custom_target(${target_name}_CopyAssets ALL
                COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${target_name}>
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${asset_archive_filename} $<TARGET_FILE_DIR:${target_name}>/
                DEPENDS ${asset_archive_filename}
                COMMENT "Copying asset archive to $<TARGET_FILE_DIR:${target_name}>"
            )

            add_dependencies(${target_name} ${target_name}_CopyAssets)

            set_target_properties(${target_name}_CopyAssets
                PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${target_name}>
                FOLDER "Polly"
            )
        endif ()
    endif ()

    if (polly_FOUND)
        add_custom_command(
            POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${polly_LIBRARY_LOCATION} $<TARGET_FILE_DIR:${target_name}>
        )
    endif ()

    set(game_src_files_dir ${current_src_dir}/Src)

    if (NOT EXISTS ${game_src_files_dir})
        message(FATAL_ERROR "[Polly] The game doesn't have a 'Src' directory. Please create one.")
    endif ()

    file(GLOB_RECURSE game_src_files CONFIGURE_DEPENDS
        "${game_src_files_dir}/*.hpp"
        "${game_src_files_dir}/*.cpp"
    )

    if (NOT game_src_files)
        message(FATAL_ERROR "Your game does not have any source files. Please place some in the 'src' folder.")
    endif ()

    target_sources(${target_name} PRIVATE ${game_src_files})

    if (ANDROID)
        set(android_project_dir ${current_src_dir}/android_project)

        if (NOT EXISTS ${android_project_dir})
            polly_fatal_error("Trying to build the game for Android, but the 'android_project' folder is missing")
        endif ()

        set(main_activity_setup_file ${android_project_dir}/game/src/MainActivitySetup.cpp)

        if (NOT EXISTS ${main_activity_setup_file})
            polly_fatal_error("Trying to build the game for Android, but some files are missing. Searched for: ${main_activity_setup_file}")
        endif ()

        find_library(android_library android REQUIRED)
        target_link_libraries(${target_name} PRIVATE ${android_library})
        target_sources(${target_name} PRIVATE ${main_activity_setup_file})
    endif ()

    # Enable strict warnings
    if (${POLLY_ADD_GAME_STRICT_WARNINGS})
        if (MSVC)
            target_compile_options(${game_name} PRIVATE
                /W4 # Warning level 4
                /WX # Treat warnings as errors
            )
        else ()
            target_compile_options(${game_name} PRIVATE
                -Wall      # Enable all standard warnings
                -Wextra    # Enable extra warnings
                -Wpedantic # Pedantic warning mode
                -Werror    # Treat warnings as errors
            )
        endif ()

        # Disable common / annoying warnings
        if (MSVC)
            target_compile_options(${game_name} PRIVATE
                /wd4100 # Unreferenced formal parameter
                /wd4505 # Unreferenced function with internal linkage has been removed
                /wd4458 # Declaration of xyz hides class member
            )
        else ()
            target_compile_options(${game_name} PRIVATE
                -Wno-unused-parameter
                -Wno-dtor-name
                -Wno-unused-private-field
                -Wno-gnu-anonymous-struct
                -Wno-nested-anon-types
                -Wno-unused-function
                -Wno-psabi
            )
        endif ()
    endif ()

    if (${POLLY_ADD_GAME_DEPLOY})
        include(CheckIPOSupported)
        check_ipo_supported(RESULT lto_supported OUTPUT error)

        if (lto_supported)
            polly_log("Enabling link-time optimization (LTO)")
            set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
        endif ()

        if (MSVC)
            add_compile_options(/GL)
        else ()
            add_compile_options(-O3 -flto)
        endif ()
    elseif (NOT MSVC)
        add_compile_options(-g -gsplit-dwarf)
    endif ()

    if (MSVC)
        target_compile_options(${target_name} PRIVATE /MP /utf-8)
    endif ()

    if (${POLLY_ADD_GAME_VERBOSE_LOGGING})
        target_compile_definitions(${target_name} PRIVATE -DENABLE_VERBOSE_LOGGING)
    endif ()

    set(asset_decryption_key_cpp ${binary_dir}/AssetDecryptionKey.cpp)
    configure_file(${polly_root_dir}/Misc/AssetDecryptionKey.cpp.in ${asset_decryption_key_cpp})
    target_sources(${target_name} PRIVATE ${asset_decryption_key_cpp})

    target_include_directories(${target_name} PRIVATE ${game_src_files_dir})
endfunction()
