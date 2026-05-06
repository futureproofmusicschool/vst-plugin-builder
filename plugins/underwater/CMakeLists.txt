cmake_minimum_required(VERSION 3.22)
project(Underwater VERSION 1.0.0)

# Add JUCE from the shared submodule at repo root
add_subdirectory(../../JUCE JUCE)

# Define the plugin target
juce_add_plugin(Underwater
    COMPANY_NAME             "Futureproof"
    PLUGIN_MANUFACTURER_CODE Fpaf
    PLUGIN_CODE              Udwt
    FORMATS                  AU VST3 Standalone
    PRODUCT_NAME             "Underwater"
    BUNDLE_ID                "com.futureproof.underwater"
    IS_SYNTH                 FALSE
    NEEDS_MIDI_INPUT         FALSE
    NEEDS_MIDI_OUTPUT        FALSE
    IS_MIDI_EFFECT           FALSE
    COPY_PLUGIN_AFTER_BUILD  TRUE
)

# Generate JuceHeader.h
juce_generate_juce_header(Underwater)

# Add source files
target_sources(Underwater
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

# Compile definitions
target_compile_definitions(Underwater
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# Link JUCE modules
target_link_libraries(Underwater
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
