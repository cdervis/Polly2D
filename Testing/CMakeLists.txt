if (BUILD_TESTING)
    polly_log("Adding Polly tests")
    
    enable_testing()
    
    CPMAddPackage(
        NAME Catch2
        GIT_REPOSITORY https://github.com/snitch-org/snitch
        VERSION 1.3.2
        OPTIONS
        "SNITCH_INSTALL_DOCS OFF"
        GIT_SHALLOW
    )

    file(GLOB_RECURSE test_files CONFIGURE_DEPENDS "*.hpp" "*.cpp" "Tests/*.cpp")
    add_executable(PollyTests ${test_files})
    target_link_libraries(PollyTests PRIVATE Polly snitch::snitch)

    add_test(NAME PollyTests COMMAND PollyTests)
endif()
