file(READ "Version.txt" polly_version)

if (polly_version STREQUAL "")
    message(FATAL_ERROR "No version found in Version.txt!")
endif ()

if (NOT polly_version MATCHES "[0-9]+\.[0-9]+\.[0-9]+$")
    message(FATAL_ERROR "Invalid version '${polly_version}' specified in Version.txt!")
endif ()

string(REPLACE "." ";" polly_version_split ${polly_version})
list(GET polly_version_split 0 polly_version_major)
list(GET polly_version_split 1 polly_version_minor)
list(GET polly_version_split 2 polly_version_revision)
