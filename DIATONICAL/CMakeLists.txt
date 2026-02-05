set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/DIATONICAL")

juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME matthew_henderson
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
    PLUGIN_MANUFACTURER_CODE MHDB
    PLUGIN_CODE DIAM
    FORMATS Standalone VST3
    PRODUCT_NAME "DIATONICAL"
)

target_sources(${PROJECT_NAME}
    PRIVATE
        source/PluginEditor.cpp
        source/PluginProcessor.cpp
        ${INCLUDE_DIR}/PluginEditor.h
        ${INCLUDE_DIR}/PluginProcessor.h
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/JUCE/extras/WindowsDLL/JuceLibraryCode
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_audio_utils
        juce::juce_opengl
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)


target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        CYPER_ASSET_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/..)
