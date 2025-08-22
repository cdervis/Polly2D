# Spine runtime
CPMAddPackage(
    NAME Spine
    GIT_REPOSITORY https://github.com/cdervis/spine
    GIT_TAG v1.0
    GIT_SHALLOW
)

target_link_libraries(Polly PRIVATE Spine)

# TODO: this should be part of the Spine project, but we'll do it here for now.
target_compile_features(Spine PRIVATE cxx_std_20)

set_target_properties(Spine PROPERTIES FOLDER "Polly")

