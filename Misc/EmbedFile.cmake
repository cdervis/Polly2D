function(polly_embed_file target_name absolute_src_filename)
    if (NOT EXISTS ${absolute_src_filename})
        polly_fatal_error("File '${absolute_src_filename}' does not exist")
    endif ()

    get_filename_component(short_filename ${absolute_src_filename} NAME)
    set(output_dir ${CMAKE_BINARY_DIR}/EmbeddedFiles/${target_name})

    polly_log("Embedding '${short_filename}' into ${target_name}")

    if (NOT EXISTS ${output_dir})
        file(MAKE_DIRECTORY ${output_dir})
    endif ()

    target_include_directories(${target_name} PRIVATE ${output_dir})

    set(output_filename_hpp "${output_dir}/${short_filename}.hpp")
    set(output_filename_cpp "${output_dir}/${short_filename}.cpp")

    string(MAKE_C_IDENTIFIER ${short_filename} c_name)

    set(script_dir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}")

    add_custom_command(
        OUTPUT
        ${output_filename_hpp}
        ${output_filename_cpp}
        COMMAND Python3::Interpreter
        BuildTool embed
        --filename "${absolute_src_filename}"
        --dst_filename_hpp "${output_filename_hpp}"
        --dst_filename_cpp "${output_filename_cpp}"
        --c_name "${c_name}"
        WORKING_DIRECTORY ${polly_root_dir}
        MAIN_DEPENDENCY ${absolute_src_filename}
        COMMENT "Bin2Header for ${short_filename}")

    target_sources(${target_name} PRIVATE
        ${output_filename_hpp}
        ${output_filename_cpp})

    source_group("Embedded" FILES ${absolute_src_filename})
    source_group("Embedded\\Generated" FILES ${output_filename_hpp} ${output_filename_cpp})
endfunction()
