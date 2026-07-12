#include "PluginProcessor.h"
#include "PluginEditor.h"

// ─── Parameter Layout ────────────────────────────────────────────────────────
//
// Stage-grouped parameters, mirrored from .ideas/parameter-spec.md.
// Wells (the gravity wells painted on the Constellation) are stored separately
// as a ValueTree child — see getStateInformation / setStateInformation.

juce::AudioProcessorValueTreeState::ParameterLayout AetherweaveProcessor::createParameterLayout()
{
    using P = juce::AudioParameterFloat;
    using B = juce::AudioParameterBool;
    using I = juce::AudioParameterInt;
    using Range = juce::NormalisableRange<float>;
    using Attr = juce::AudioParameterFloatAttributes;

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // ── Stage 1: Capture ────────────────────────────────────────────────────
    params.push_back(std::make_unique<B>(juce::ParameterID { "capture_enable", 1 }, "Capture Enable", true));
    params.push_back(std::make_unique<P>(juce::ParameterID { "capture_length", 1 }, "Buffer Length",
        Range(0.25f, 8.0f, 0.01f), 2.0f, Attr().withLabel("s")));
    params.push_back(std::make_unique<B>(juce::ParameterID { "capture_freeze", 1 }, "Freeze", false));

    // ── Stage 2: Shatter ────────────────────────────────────────────────────
    params.push_back(std::make_unique<B>(juce::ParameterID { "shatter_enable", 1 }, "Shatter Enable", true));
    params.push_back(std::make_unique<P>(juce::ParameterID { "grain_size", 1 }, "Grain Size",
        Range(5.0f, 500.0f, 0.1f, 0.4f), 80.0f, Attr().withLabel("ms")));
    params.push_back(std::make_unique<P>(juce::ParameterID { "grain_density", 1 }, "Density",
        Range(1.0f, 200.0f, 0.1f, 0.5f), 40.0f, Attr().withLabel("/s")));
    params.push_back(std::make_unique<P>(juce::ParameterID { "pos_jitter", 1 }, "Position Jitter",
        Range(0.0f, 100.0f, 0.1f), 25.0f, Attr().withLabel("%")));
    params.push_back(std::make_unique<P>(juce::ParameterID { "pitch_jitter", 1 }, "Pitch Jitter",
        Range(0.0f, 24.0f, 0.01f), 0.0f, Attr().withLabel("st")));
    params.push_back(std::make_unique<P>(juce::ParameterID { "window_tilt", 1 }, "Window Tilt",
        Range(0.0f, 100.0f, 0.1f), 50.0f, Attr().withLabel("%")));

    // ── Stage 3: Drift ──────────────────────────────────────────────────────
    params.push_back(std::make_unique<B>(juce::ParameterID { "drift_enable", 1 }, "Drift Enable", true));
    params.push_back(std::make_unique<P>(juce::ParameterID { "swarm_temp", 1 }, "Swarm Temperature",
        Range(0.0f, 100.0f, 0.1f), 30.0f, Attr().withLabel("%")));
    params.push_back(std::make_unique<P>(juce::ParameterID { "well_strength", 1 }, "Well Strength",
        Range(0.0f, 100.0f, 0.1f), 60.0f, Attr().withLabel("%")));
    params.push_back(std::make_unique<P>(juce::ParameterID { "motion_speed", 1 }, "Motion Speed",
        Range(0.1f, 10.0f, 0.01f, 0.4f), 1.0f, Attr().withLabel("x")));

    // ── Stage 4: Bloom ──────────────────────────────────────────────────────
    params.push_back(std::make_unique<B>(juce::ParameterID { "bloom_enable", 1 }, "Bloom Enable", true));
    params.push_back(std::make_unique<I>(juce::ParameterID { "shimmer_interval", 1 }, "Shimmer Interval",
        -24, 24, 12));
    params.push_back(std::make_unique<P>(juce::ParameterID { "bloom_diffusion", 1 }, "Diffusion",
        Range(0.0f, 100.0f, 0.1f), 70.0f, Attr().withLabel("%")));
    params.push_back(std::make_unique<P>(juce::ParameterID { "bloom_mix", 1 }, "Bloom Mix",
        Range(0.0f, 100.0f, 0.1f), 35.0f, Attr().withLabel("%")));

    // ── Output ──────────────────────────────────────────────────────────────
    params.push_back(std::make_unique<P>(juce::ParameterID { "mix", 1 }, "Dry/Wet",
        Range(0.0f, 100.0f, 0.1f), 100.0f, Attr().withLabel("%")));
    params.push_back(std::make_unique<P>(juce::ParameterID { "output_gain", 1 }, "Output Gain",
        Range(-24.0f, 12.0f, 0.1f), 0.0f, Attr().withLabel("dB")));

    // ── Secondary ───────────────────────────────────────────────────────────
    params.push_back(std::make_unique<I>(juce::ParameterID { "voice_limit", 1 }, "Grain Voices", 8, 256, 64));
    params.push_back(std::make_unique<P>(juce::ParameterID { "stereo_spread", 1 }, "Stereo Spread",
        Range(0.0f, 100.0f, 0.1f), 70.0f, Attr().withLabel("%")));
    params.push_back(std::make_unique<P>(juce::ParameterID { "brightness_bias", 1 }, "Brightness Bias",
        Range(-100.0f, 100.0f, 0.1f), 0.0f, Attr().withLabel("%")));
    params.push_back(std::make_unique<B>(juce::ParameterID { "tempo_sync", 1 }, "Tempo Sync", false));
    params.push_back(std::make_unique<P>(juce::ParameterID { "vis_trail", 1 }, "Visual Trail",
        Range(0.0f, 100.0f, 0.1f), 60.0f, Attr().withLabel("%")));

    return { params.begin(), params.end() };
}

// ─── Bus Layout ──────────────────────────────────────────────────────────────

juce::AudioProcessor::BusesProperties AetherweaveProcessor::getBusesProperties()
{
    return BusesProperties()
        .withInput("Input",   juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true);
}

// ─── Constructor ─────────────────────────────────────────────────────────────

AetherweaveProcessor::AetherweaveProcessor()
    : AudioProcessor(getBusesProperties()),
      apvts(*this, nullptr, juce::Identifier("PARAMETERS"), createParameterLayout())
{
}

// ─── Prepare / Release ───────────────────────────────────────────────────────

void AetherweaveProcessor::prepareToPlay(double /*sampleRate*/, int /*samplesPerBlock*/)
{
    // Stage 1: no DSP yet — DSP modules will be prepared here in Stage 2.
}

void AetherweaveProcessor::releaseResources()
{
}

// ─── Process Block ───────────────────────────────────────────────────────────

void AetherweaveProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                         juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;

    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that don't have a corresponding input.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Stage 1: pass-through. DSP comes in Stage 2.
}

// ─── State Serialization ─────────────────────────────────────────────────────
//
// APVTS handles all standard parameters. Wells (the gravity wells painted on
// the Constellation) are appended as a child ValueTree named "wells". Stage 1
// has no wells yet — the round-trip still works since the child is just absent.

void AetherweaveProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AetherweaveProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

// ─── Editor ──────────────────────────────────────────────────────────────────

juce::AudioProcessorEditor* AetherweaveProcessor::createEditor()
{
    return new AetherweaveEditor(*this);
}

// ─── Plugin Instantiation ────────────────────────────────────────────────────

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AetherweaveProcessor();
}
