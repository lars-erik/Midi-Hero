cmake_minimum_required(VERSION 3.15)

project(MidiHero VERSION 0.0.4)

# Set JUCE path
set(JUCE_DIR ${CMAKE_SOURCE_DIR}/JUCE)

# Add JUCE
add_subdirectory(${JUCE_DIR} JUCE)

# Define plugin target
juce_add_plugin(MidiHero
    COMPANY_NAME "Aabech Consulting"
    PLUGIN_NAME "Midi Hero"
    PLUGIN_MANUFACTURER_CODE "AaCo"
    PLUGIN_CODE "MHr0"
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Midi Hero"
)

# Link JUCE modules
target_link_libraries(MidiHero PRIVATE
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
    juce::juce_gui_extra
)

juce_generate_juce_header(MidiHero)

# Set build options
set_target_properties(MidiHero PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

set(MODEL_FILES
    Source/MidiListModel.h
    Source/MidiQueue.h
    Source/Observer.h
    Source/TimedMidiMessage.h
)

set(GUI_FILES
    Source/DivisionLevelComponent.h
    Source/HeroPage.cpp
    Source/HeroPage.h
    Source/JudgementLabel.cpp
    Source/JudgementLabel.h
    Source/LogTablePage.cpp
    Source/LogTablePage.h
    Source/MidiTable.h
    Source/ScoreCountsComponent.h
    Source/StatisticsPage.cpp
    Source/StatisticsPage.h
    Source/TabsComponent.cpp
    Source/TabsComponent.h
    Source/TotalScoreComponent.h
)

set(GLOBAL_FILES
    Source/Global.h
    Source/MidiHeroSettings.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
)

target_sources(MidiHero PRIVATE ${MODEL_FILES} ${GUI_FILES} ${GLOBAL_FILES})

source_group("Model" FILES ${MODEL_FILES})
source_group("GUI" FILES ${GUI_FILES})

