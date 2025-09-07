function(get_home_dir output_var)
    if (WIN32)
        set(home_dir "$ENV{USERPROFILE}")
    else ()
        set(home_dir "$ENV{HOME}")
    endif ()

    if (EXISTS "${home_dir}")
        set("${output_var}" "${home_dir}" PARENT_SCOPE)
    else ()
        polly_fatal_error("Failed to determine the home directory.")
    endif ()
endfunction()

if (NOT POLLY_NO_DEBUG_SUPPORT)
    if (MSVC)
        set(natvis_file ${polly_root_dir}/Misc/DebugVis/PollyMSVC.natvis)
        target_sources(Polly PRIVATE ${natvis_file})
        target_link_options(Polly PRIVATE /NATVIS:${natvis_file})
    else ()
        polly_log("Checking LLDB support")

        get_home_dir(user_home_dir)
        set(lldb_dir ${user_home_dir}/.lldb)

        if (NOT EXISTS ${lldb_dir})
            file(MAKE_DIRECTORY ${lldb_dir})
        endif ()

        file(COPY ${polly_root_dir}/Misc/DebugVis/PollyLLDB.py DESTINATION ${lldb_dir})

        set(lldbinit_filename ${user_home_dir}/.lldbinit)
        set(lldb_command_str "command script import ${lldb_dir}/PollyLLDB.py")

        set(is_polly_lldb_registered -1)
        set(lldbinit_contents "")

        if (EXISTS ${lldbinit_filename})
            file(READ ${lldbinit_filename} lldbinit_contents)
        endif ()

        if (NOT lldbinit_contents STREQUAL "")
            string(FIND ${lldbinit_contents} "${lldb_command_str}" is_polly_lldb_registered)
        endif ()

        if (is_polly_lldb_registered EQUAL -1)
            polly_log("Registering Polly LLDB visualizers")
            file(APPEND ${lldbinit_filename} "\n${lldb_command_str}\n")
        endif ()
    endif ()
endif ()