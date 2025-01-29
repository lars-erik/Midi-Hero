cmake_minimum_required(VERSION 3.15)

project(MidiHeroTests)

include(FetchContent)

# Fetch Catch2 from GitHub
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(Catch2)

# Define test executable
add_executable(MidiHeroTests
    "Midi Hero_Tests/main.cpp"
    "Midi Hero_Tests/Midi Hero_Tests.cpp"
    "Midi Hero_Tests/resource.h"
    "Midi Hero_Tests/TestData.h"
    "Midi Hero_Tests/TestUtils.h"
    "Midi Hero_Tests/Midi Hero_Tests.rc"
)

# Ensure the correct include directories are set
target_include_directories(MidiHeroTests PRIVATE
    "${CMAKE_SOURCE_DIR}/Midi Hero_Tests"   # Your test source files
    "${CMAKE_SOURCE_DIR}/../JUCE/modules"      # JUCE headers
    "${CMAKE_BINARY_DIR}/_deps/catch2-src/single_include"  # Correct Catch2 headers
    "${CMAKE_CURRENT_BINARY_DIR}"  # Ensure JuceHeader.h is available
)

# Link resource files on Windows
if(WIN32)
    target_sources(MidiHeroTests PRIVATE "Midi Hero_Tests/Midi Hero_Tests.rc")
endif()

# Link the test executable to Catch2 and JUCE
target_link_libraries(MidiHeroTests PRIVATE
    Catch2::Catch2WithMain
    MidiHero  # Ensure the main project is linked
)

# Enable CTest for test discovery
enable_testing()
include(CTest)
include(Catch)
catch_discover_tests(MidiHeroTests)
