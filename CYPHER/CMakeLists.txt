set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/cypher")

juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME MRH
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    PLUGIN_MANUFACTURER_CODE MCMP
    PLUGIN_CODE EXPL
    FORMATS Standalone VST3
    PRODUCT_NAME "cypher"
)

target_sources(${PROJECT_NAME}
    PRIVATE
        source/main.cpp
        source/PluginEditor.cpp
        source/PluginProcessor.cpp
        source/Cypher.cpp
        source/CypherControls.cpp
        source/Oscilloscope.cpp
        source/OpenGL.cpp
        source/VertexBuffer.cpp

        ${INCLUDE_DIR}/PluginEditor.h
        ${INCLUDE_DIR}/PluginProcessor.h
        ${INCLUDE_DIR}/Cypher.h
        ${INCLUDE_DIR}/CypherControls.h
        ${INCLUDE_DIR}/Oscilloscope.h
        ${INCLUDE_DIR}/OpenGL.h
        ${INCLUDE_DIR}/VertexBuffer.h
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
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
