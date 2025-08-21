if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(is_master_project TRUE)
endif ()

option(POLLY_ENABLE_ADDRESS_SANITIZER "Enable clang address sanitizer" OFF)
option(POLLY_ENABLE_VERBOSE_LOGGING "Enable verbose logging during debug mode" OFF)
option(POLLY_BUILD_APPS "Build the Polly testbed and sample games" ${is_master_project})

if (ANDROID)
    if (POLLY_BUILD_TESTBED)
        polly_log("Disabling testbed implicitly due to Android")
        set(POLLY_BUILD_TESTBED OFF)
    endif ()

    if (POLLY_BUILD_DEMO_BROWSER)
        polly_log("Disabling demo browser implicitly due to Android")
        set(POLLY_BUILD_DEMO_BROWSER OFF)
    endif ()
endif ()
