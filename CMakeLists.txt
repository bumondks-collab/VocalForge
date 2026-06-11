cmake_minimum_required(VERSION 3.22)
project(VocalForge VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(DEFINED JUCE_PATH AND EXISTS "${JUCE_PATH}")
    message(STATUS "Using local JUCE at: ${JUCE_PATH}")
    add_subdirectory("${JUCE_PATH}" juce)
else()
    message(STATUS "JUCE_PATH not set -- downloading JUCE 8.0.4 via FetchContent")
    include(FetchContent)
    FetchContent_Declare(JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG        8.0.4
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
    )
    FetchContent_MakeAvailable(JUCE)
endif()

juce_add_plugin(VocalForge
    PLUGIN_CODE                  VcFg
    PLUGIN_MANUFACTURER_CODE     Bmix
    PRODUCT_NAME                 "Vocal Factory"
    COMPANY_NAME                 "Bumon.mix"
    COMPANY_COPYRIGHT            "2024 Bumon.mix"
    COMPANY_WEBSITE              "www.bumon.mix"
    BUNDLE_ID                    com.bumonmix.vocalfactory
    FORMATS                      VST3 Standalone
    IS_SYNTH                     FALSE
    NEEDS_MIDI_INPUT             FALSE
    NEEDS_MIDI_OUTPUT            FALSE
    IS_MIDI_EFFECT               FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS  FALSE
    VST3_CATEGORIES              "Fx|Vocal"
    COPY_PLUGIN_AFTER_BUILD      FALSE
)

juce_generate_juce_header(VocalForge)

target_sources(VocalForge PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)

target_compile_definitions(VocalForge PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(VocalForge
    PRIVATE
        juce::juce_audio_basics juce::juce_audio_devices juce::juce_audio_formats
        juce::juce_audio_plugin_client juce::juce_audio_processors juce::juce_audio_utils
        juce::juce_core juce::juce_data_structures juce::juce_dsp juce::juce_events
        juce::juce_graphics juce::juce_gui_basics juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
