set(dr_libs_url "https://raw.githubusercontent.com/mackron/dr_libs/refs/tags/wav-0.14.0")

# dr_wav
polly_download_dep_file(
    dr_wav.h ${dr_libs_url}/dr_wav.h
    SHA256=856593362de0deebd457f710b69a808a6eb5fe9c09d0239aae6bf047c6ef90d1
)

# dr_mp3
polly_download_dep_file(
    dr_mp3.h ${dr_libs_url}/dr_mp3.h
    SHA256=e9f220aa2d47bd6c8d6d98ff4390277cbb2dfd988190c0509f70035eca357613
)

# dr_flac
polly_download_dep_file(
    dr_flac.h ${dr_libs_url}/dr_flac.h
    SHA256=cb8ac090f53f1a988b2a1bc724bd881a6bf171b02c2e70632c872294431960c7
)

# miniaudio
polly_download_dep_file(
    miniaudio.h
    https://github.com/mackron/miniaudio/raw/refs/tags/0.11.22/miniaudio.h
    SHA256=9019743287e443c55e5737a7297f38e5e358561701d6db2d905afb114390c410
)

if (APPLE)
    find_library(AUDIOTOOLBOX_FRAMEWORK AudioToolbox REQUIRED)
    find_library(COREAUDIO_FRAMEWORK CoreAudio REQUIRED)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)
    find_library(AVFOUNDATION_FRAMEWORK AVFoundation REQUIRED)

    polly_log("AudioToolbox: ${AUDIOTOOLBOX_FRAMEWORK}")
    polly_log("CoreAudio: ${COREAUDIO_FRAMEWORK}")
    polly_log("CoreFoundation: ${COREFOUNDATION_FRAMEWORK}")
    polly_log("AVFoundation: ${AVFOUNDATION_FRAMEWORK}")

    target_link_libraries(Polly PRIVATE
        ${AUDIOTOOLBOX_FRAMEWORK}
        ${COREAUDIO_FRAMEWORK}
        ${COREFOUNDATION_FRAMEWORK}
        ${AVFOUNDATION_FRAMEWORK}
    )

    target_sources(Polly PRIVATE miniaudio_impl_objc.m)
endif ()

target_compile_definitions(Polly PRIVATE
    -DWITH_NULL
    -DWITH_NOSOUND
    -DWITH_MINIAUDIO
    -DSTB_VORBIS_NO_STDIO
)
