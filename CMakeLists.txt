cmake_minimum_required(VERSION 3.24)

#Configure compiler and formats depending on OS
if(APPLE)
    set(FORMATS Standalone AU VST3)
elseif(LINUX)
    set(CMAKE_CXX_COMPILER "g++")
    set(FORMATS Standalone LV2 VST3)
else()
    set(FORMATS Standalone VST3)
endif()

# Change to your project name
project(BringYourOwnImpulse VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_XCODE_GENERATE_SCHEME OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# CPM kept failing and fetch content seems to work better
include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE
    GIT_TAG master
    GIT_SHALLOW ON
)
FetchContent_Declare(
    FFTC
    GIT_REPOSITORY https://github.com/HiFi-LoFi/FFTConvolver.git
    GIT_TAG f2cdeb04c42141d2caec19ca4f137398b2a76b85
    GIT_SHALLOW ON
)
FetchContent_Declare(
    kitik-utility
    GIT_REPOSITORY https://github.com/KiTiKDub/KiTiK-JUCE-Utils.git
    GIT_TAG master
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable( juce FFTC kitik-utility )

add_library(FFTConvolver_lib STATIC #THE libary needs to be all lower case
    ${fftc_SOURCE_DIR}/AudioFFT.cpp
    ${fftc_SOURCE_DIR}/AudioFFT.h
    ${fftc_SOURCE_DIR}/FFTConvolver.cpp
    ${fftc_SOURCE_DIR}/FFTConvolver.h
    ${fftc_SOURCE_DIR}/TwoStageFFTConvolver.cpp
    ${fftc_SOURCE_DIR}/TwoStageFFTConvolver.h
    ${fftc_SOURCE_DIR}/Utilities.cpp
    ${fftc_SOURCE_DIR}/Utilities.h
)

# Make sure you include any new source files here
set(SourceFiles
    Source/GUI/BYOILnf.cpp
    Source/GUI/BYOILnf.h
    Source/GUI/DragAndDrop.cpp
    Source/GUI/DragAndDrop.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
)

# Change these to your own preferences
juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME "KiTiK Music"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    JUCE_VST3_CAN_REPLACE_VST2 FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    NEEDS_WEB_BROWSER FALSE
    PLUGIN_MANUFACTURER_CODE
    PLUGIN_CODE
    FORMATS "${FORMATS}"
    PRODUCT_NAME "BYOI"
    LV2URI https://github.com/KiTiKDub/BYOI.git
    LV2_SHARED_LIBARARY_NAME BYOI
)

# How we want our SourceFiles to appear in our IDE
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SourceFiles})

# Make the SourceFiles buildable
target_sources(${PROJECT_NAME} PRIVATE ${SourceFiles})

# Adds a BinaryData target for embedding assets into the binary
file(GLOB_RECURSE AssetFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Assets/*")
juce_add_binary_data(Assets SOURCES ${AssetFiles})

# Make linux happy
set_target_properties(Assets PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
set_target_properties(kitik-utils PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
set_target_properties(FFTConvolver_lib PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

target_include_directories(kitik-utils PUBLIC ${kitik-utility_SOURCE_DIR})
target_include_directories(FFTConvolver_lib PUBLIC ${fftc_SOURCE_DIR})
target_link_libraries(kitik-utils PUBLIC
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_dsp
    juce::juce_gui_basics
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# These are some toggleable options from the JUCE CMake API
target_compile_definitions(kitik-utils PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

# JUCE libraries to bring into our project
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    FFTConvolver_lib
    kitik-utils
    Assets
)
