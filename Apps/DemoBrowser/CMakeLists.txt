polly_add_game(
    NAME DemoBrowser
    COMPANY Polly
    VERSION ${CMAKE_PROJECT_VERSION}
    VERBOSE
)

target_include_directories(DemoBrowser PRIVATE Demos)
