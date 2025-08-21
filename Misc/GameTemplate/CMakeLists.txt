# CMake script for the game
cmake_minimum_required(VERSION 3.22)

# CMake options for the game:
option(GAME_STRICT_WARNINGS "Enable strict compiler warnings?" ON)
option(GAME_VERBOSE_LOGGING "Enable verbose logging (LogVerbose())?" OFF)

# Set general properties of the game.
set(game_name "MyGame")
set(game_display_name "My Game")
set(company_name "MyCompany")
set(game_version 0.0.1)

# Set the version of Polly to fetch and use.
set(polly_version 0.9.0)

# Declare the game project.
project(
    ${game_name}
    VERSION ${game_version}
    DESCRIPTION ${game_display_name}
    LANGUAGES CXX
)

include(Misc/PackageManagement.cmake)

# Ensure that Polly is downloaded and set up.
CPMAddPackage(
    NAME Polly
    URL https://github.com/cdervis/Polly/archive/refs/tags/v${polly_version}.zip
)

# Add our game as a CMake target.
polly_add_game(
    NAME ${game_name}
    COMPANY ${company_name}
    VERSION ${game_version}
    STRICT_WARNINGS ON
    DEPLOY ${GAME_DEPLOY}
    VERBOSE_LOGGING ${GAME_VERBOSE_LOGGING}
)
