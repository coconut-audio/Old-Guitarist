cmake_minimum_required(VERSION 3.22)

project(OLD_GUITARIST VERSION 0.0.1)

set(PROJECT_NAME Old.Guitarist)
set(CURRENT_VERSION 0.0.1)
set(CURRENT_PLUGIN_CODE GMOD)

message(STATUS "Plugin Name: ${PROJECT_NAME}")
message(STATUS "Version: ${CURRENT_VERSION}")
message(STATUS "Plugin Code: ${CURRENT_PLUGIN_CODE}")

# set JUCE path
add_subdirectory(modules/JUCE)

# set plugin formats
set(FORMATS VST3 Standalone)

# JUCE specific settings
juce_add_plugin(${PROJECT_NAME}
    VERSION ${CURRENT_VERSION}
    COMPANY_NAME "CoconutAudio"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE COAU
    FORMATS "${FORMATS}"
    PRODUCT_NAME "${PROJECT_NAME}"
    PLUGIN_CODE "${CURRENT_PLUGIN_CODE}")

# platform specific settings
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum macOS version" FORCE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
endif()

# add source files
target_sources(${PROJECT_NAME}
    PRIVATE
        "source/Processor.cpp"
        "source/Editor.cpp"
        "source/synth/Voice.cpp"
        "source/ui/StringVisualizer.cpp"
        "source/ui/Keyboard.cpp"
        "source/model/WaveSolver.cpp")

# generate JUCE header
juce_generate_juce_header(${PROJECT_NAME})

# set C++ standard
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# add binary data
juce_add_binary_data(Data
    SOURCES
        "resources/mic.wav")

# link libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Data
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# set platform specific compile definitions
if(MSVC)
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
    )
endif()
