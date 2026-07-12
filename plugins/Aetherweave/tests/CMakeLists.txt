# Catch2 + CTest setup for Aetherweave.
#
# This file lives at plugins/Aetherweave/tests/CMakeLists.txt and is auto-included
# by the plugin's main CMakeLists.txt when this file exists (see the conditional
# add_subdirectory at the bottom of the plugin's CMakeLists.txt).

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.0
)
FetchContent_MakeAvailable(Catch2)

# Test executable. Add new test files here as you write them.
add_executable(AetherweaveTests
    sample-test.cpp
)

target_link_libraries(AetherweaveTests
    PRIVATE
        Catch2::Catch2WithMain
        juce::juce_dsp
        juce::juce_audio_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

target_compile_definitions(AetherweaveTests
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)

# Test code can include DSP headers from ../Source/.
# Extract algorithms into standalone classes (e.g. Source/dsp/CaptureBuffer.h)
# that don't depend on juce::AudioProcessor — those are what we test here.
target_include_directories(AetherweaveTests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Source
)

# Discover tests for CTest. Run with `ctest --output-on-failure` from the build dir.
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(AetherweaveTests)
