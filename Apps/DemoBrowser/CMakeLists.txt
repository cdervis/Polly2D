polly_add_game(
    NAME DemoBrowser
    DISPLAY_NAME "Polly Demo Browser"
    COMPANY Polly
    VERSION ${CMAKE_PROJECT_VERSION}
    VERBOSE
)

target_include_directories(DemoBrowser PRIVATE Demos)
