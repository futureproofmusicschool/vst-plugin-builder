cmake_minimum_required(VERSION 3.22)
project(Aetherweave VERSION 1.0.0)

# Add JUCE from the shared submodule at repo root
add_subdirectory(../../JUCE JUCE)

# Define the plugin target
juce_add_plugin(Aetherweave
    COMPANY_NAME             "Futureproof"
    PLUGIN_MANUFACTURER_CODE Fpms
    PLUGIN_CODE              Aetw
    FORMATS                  AU VST3 Standalone
    # CLAP omitted: bundled JUCE 8.0.12 doesn't ship native CLAP in its CMake.
    # Re-add via clap-juce-extensions if CLAP becomes a requirement.
    PRODUCT_NAME             "Aetherweave"
    BUNDLE_ID                "com.futureproof.aetherweave"
    IS_SYNTH                 FALSE
    NEEDS_MIDI_INPUT         FALSE
    NEEDS_MIDI_OUTPUT        FALSE
    IS_MIDI_EFFECT           FALSE
    COPY_PLUGIN_AFTER_BUILD  TRUE
)

# Generate JuceHeader.h
juce_generate_juce_header(Aetherweave)

# Add source files
target_sources(Aetherweave
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

# Compile definitions
target_compile_definitions(Aetherweave
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# Link JUCE modules
target_link_libraries(Aetherweave
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_opengl
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Optional unit tests — auto-included if tests/CMakeLists.txt exists.
# See templates/tests/ for the scaffold and rules/juce-patterns.md "Testing".
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt)
    enable_testing()
    add_subdirectory(tests)
endif()
