// Sample DSP test for Aetherweave.
//
// Pattern (filled in during /implement Stage 2):
//   1. Extract DSP algorithms into standalone classes under Source/dsp/ that
//      don't depend on juce::AudioProcessor:
//        - Source/dsp/CaptureBuffer.h    (Stage 2 step 1)
//        - Source/dsp/GrainEngine.h      (Stage 2 step 2)
//        - Source/dsp/DriftField.h       (Stage 2 step 3)
//        - Source/dsp/BloomTail.h        (Stage 2 step 4)
//   2. Use them from PluginProcessor::processBlock.
//   3. Test them here by instantiating directly and feeding synthetic buffers.
//
// Run with:
//   cd plugins/Aetherweave && cmake -B build && cmake --build build && (cd build && ctest --output-on-failure)

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <vector>

// Example: include a DSP class once you've extracted one.
// #include "dsp/CaptureBuffer.h"

TEST_CASE("Catch2 is wired up correctly", "[sanity]")
{
    REQUIRE(2 + 2 == 4);
}

// Example DSP tests to write in Stage 2:
//
// TEST_CASE("CaptureBuffer reads back what it wrote", "[capture]")
// {
//     CaptureBuffer buf;
//     buf.prepare(48000.0, 512, 2, 8.0);
//
//     std::vector<float> in(512);
//     for (int i = 0; i < 512; ++i) in[i] = std::sin(i * 0.1f);
//     buf.write(in.data(), in.data(), 512);
//
//     // Read back from position 0, channel 0
//     for (int i = 0; i < 512; ++i)
//         REQUIRE(buf.read(double(i), 0) == Catch::Approx(in[i]).margin(1e-3));
// }
//
// TEST_CASE("GrainEngine is stable under impulse", "[grain]") { /* … */ }
// TEST_CASE("DriftField converges grain to single attractor", "[drift]") { /* … */ }
// TEST_CASE("BloomTail decays to silence", "[bloom]") { /* … */ }
