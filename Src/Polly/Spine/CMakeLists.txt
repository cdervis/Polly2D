# Spine runtime
CPMAddPackage(
    NAME Spine
    GIT_REPOSITORY https://github.com/cdervis/spine
    GIT_TAG v1.0
    GIT_SHALLOW
)

target_link_libraries(Polly PRIVATE Spine)

