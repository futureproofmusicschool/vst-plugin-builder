#include "PluginProcessor.h"
#include "PluginEditor.h"

// ─── Parameter Layout ────────────────────────────────────────────────────────

juce::AudioProcessorValueTreeState::ParameterLayout UnderwaterProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // DEPTH — LPF cutoff (200–2000 Hz), skewed toward low end
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "depth", 1 },
        "Depth",
        juce::NormalisableRange<float>(200.0f, 2000.0f, 1.0f, 0.5f),
        600.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")
    ));

    // MOTION — chorus rate + depth (0–100%)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "motion", 1 },
        "Motion",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        40.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")
    ));

    // SPACE — reverb amount (0–100%)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "space", 1 },
        "Space",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")
    ));

    // MIX — wet/dry blend (0–100%)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "mix", 1 },
        "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")
    ));

    return { params.begin(), params.end() };
}

// ─── Bus Layout ──────────────────────────────────────────────────────────────

juce::AudioProcessor::BusesProperties UnderwaterProcessor::getBusesProperties()
{
    return BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true);
}

// ─── Constructor ─────────────────────────────────────────────────────────────

UnderwaterProcessor::UnderwaterProcessor()
    : AudioProcessor(getBusesProperties()),
      apvts(*this, nullptr, juce::Identifier("PARAMETERS"), createParameterLayout())
{
}

// ─── Prepare / Release ───────────────────────────────────────────────────────

void UnderwaterProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Cache parameter pointers for lock-free reads
    depthParam  = apvts.getRawParameterValue("depth");
    motionParam = apvts.getRawParameterValue("motion");
    spaceParam  = apvts.getRawParameterValue("space");
    mixParam    = apvts.getRawParameterValue("mix");

    // Build DSP spec
    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels      = static_cast<juce::uint32>(getTotalNumOutputChannels());

    // Prepare low-pass filter
    lowPassFilter.prepare(spec);
    lowPassFilter.setType(juce::dsp::StateVariableTPTFilter<float>::Type::lowpass);
    lowPassFilter.setCutoffFrequency(600.0f);
    lowPassFilter.setResonance(1.0f / std::sqrt(2.0f));  // Butterworth Q

    // Prepare chorus
    chorus.prepare(spec);
    chorus.setCentreDelay(7.0f);
    chorus.setFeedback(0.3f);
    chorus.setMix(1.0f);
    chorus.setRate(0.8f);
    chorus.setDepth(0.2f);

    // Prepare reverb
    reverb.prepare(spec);

    // Allocate dry buffer (2x headroom for safety)
    dryBuffer.setSize(static_cast<int>(spec.numChannels),
                      samplesPerBlock * 2);
}

void UnderwaterProcessor::releaseResources()
{
    lowPassFilter.reset();
    chorus.reset();
    reverb.reset();
}

// ─── Process Block ───────────────────────────────────────────────────────────

void UnderwaterProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                        juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;

    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    // Clear unused output channels
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    // Read parameters (lock-free)
    const float depth  = depthParam->load();
    const float motion = motionParam->load() / 100.0f;
    const float space  = spaceParam->load()  / 100.0f;
    const float mix    = mixParam->load()    / 100.0f;

    // Resize dry buffer if needed (some hosts send larger blocks)
    if (dryBuffer.getNumSamples() < numSamples)
        dryBuffer.setSize(totalNumOutputChannels, numSamples, false, false, true);

    // Save dry copy for wet/dry mix
    for (int ch = 0; ch < totalNumOutputChannels; ++ch)
        dryBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);

    // --- Stage 1: Low-pass filter ---
    lowPassFilter.setCutoffFrequency(depth);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    lowPassFilter.process(context);

    // --- Stage 2: Chorus (modulation) ---
    chorus.setRate(0.2f + motion * 1.8f);   // 0.2–2.0 Hz
    chorus.setDepth(motion * 0.5f);          // 0.0–0.5

    chorus.process(context);

    // --- Stage 3: Reverb ---
    juce::Reverb::Parameters reverbParams;
    reverbParams.roomSize   = 0.3f + space * 0.6f;  // 0.3–0.9
    reverbParams.damping    = 0.7f;
    reverbParams.wetLevel   = space * 0.6f;          // 0.0–0.6
    reverbParams.dryLevel   = 1.0f - reverbParams.wetLevel;
    reverbParams.width      = 0.8f;
    reverbParams.freezeMode = 0.0f;
    reverb.setParameters(reverbParams);

    reverb.process(context);

    // --- Stage 4: Wet/dry mix ---
    for (int ch = 0; ch < totalNumOutputChannels; ++ch)
    {
        auto* wetData = buffer.getWritePointer(ch);
        const auto* dryData = dryBuffer.getReadPointer(ch);

        for (int n = 0; n < numSamples; ++n)
            wetData[n] = mix * wetData[n] + (1.0f - mix) * dryData[n];
    }
}

// ─── State Serialization ─────────────────────────────────────────────────────

void UnderwaterProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void UnderwaterProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

// ─── Editor ──────────────────────────────────────────────────────────────────

juce::AudioProcessorEditor* UnderwaterProcessor::createEditor()
{
    return new UnderwaterEditor(*this);
}

// ─── Plugin Instantiation ────────────────────────────────────────────────────

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new UnderwaterProcessor();
}
