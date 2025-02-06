cmake_minimum_required(VERSION 3.15)

# This tells cmake we have goodies in the /cmake folder
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

# Modern concise way to add dependencies to your project
include (CPM)

# Allow overriding the version from GitVersion via -DVERSION
if(NOT DEFINED VERSION)
    set(VERSION "0.0.7")  # Default fallback
endif()

project(MidiHero VERSION ${VERSION})

message(STATUS "Building MidiHero version: ${PROJECT_VERSION}")

# Set JUCE path
set(JUCE_DIR ${CMAKE_SOURCE_DIR}/JUCE)

# Add JUCE
add_subdirectory(${JUCE_DIR} JUCE)

# Define plugin target
juce_add_plugin(MidiHero
    COMPANY_NAME "Aabech Consulting"
    BUNDLE_ID "com.AabechConsulting.MidiHero"
    PLUGIN_NAME "Midi Hero"
    PLUGIN_MANUFACTURER_CODE "AaCo"
    PLUGIN_CODE "MHr0"
    FORMATS VST3 AU AUv3 Standalone
    PRODUCT_NAME "Midi Hero"
)


# This lets us use our code in both the JUCE targets and our Test target
# Without running into ODR violations
add_library(SharedCode INTERFACE)

target_compile_definitions(SharedCode
    INTERFACE
    # JUCE_WEB_BROWSER and JUCE_USE_CURL off by default
    JUCE_WEB_BROWSER=0  # If you set this to 1, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
    JUCE_USE_CURL=0     # If you set this to 1, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
)

# Link JUCE modules
target_link_libraries(SharedCode 
    INTERFACE
    juce::juce_animation
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
)

target_link_libraries(MidiHero PRIVATE SharedCode)

juce_generate_juce_header(MidiHero)

# Set build options
set_target_properties(SharedCode PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# Recursively find all source and header files
file(GLOB_RECURSE MODEL_FILES "Source/Model/*.*")
file(GLOB_RECURSE GUI_FILES "Source/GUI/*.*")
file(GLOB_RECURSE GLOBAL_FILES "Source/Plugin/*.*")

# Add sources to the target
target_sources(SharedCode INTERFACE ${MODEL_FILES} ${GUI_FILES} ${GLOBAL_FILES})

target_include_directories(MidiHero PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source/Model)
target_include_directories(MidiHero PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source/GUI)
target_include_directories(MidiHero PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source/Plugin)

# Automatically reflect folder structure in VS Solution Explorer
source_group(TREE ${CMAKE_SOURCE_DIR}/Source PREFIX "Source Files" FILES ${MODEL_FILES} ${GUI_FILES} ${GLOBAL_FILES})

message(${JUCE_MODULES_DIR})

include(Tests)
