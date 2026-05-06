#pragma once

#include <JuceHeader.h>

class UnderwaterProcessor : public juce::AudioProcessor
{
public:
    UnderwaterProcessor();
    ~UnderwaterProcessor() override = default;

    // --- AudioProcessor interface ---
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Underwater"; }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // --- Parameter tree ---
    juce::AudioProcessorValueTreeState apvts;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    static juce::AudioProcessor::BusesProperties getBusesProperties();

private:
    // Atomic parameter pointers (lock-free audio thread access)
    std::atomic<float>* depthParam  = nullptr;
    std::atomic<float>* motionParam = nullptr;
    std::atomic<float>* spaceParam  = nullptr;
    std::atomic<float>* mixParam    = nullptr;

    // DSP processors
    juce::dsp::StateVariableTPTFilter<float> lowPassFilter;
    juce::dsp::Chorus<float> chorus;
    juce::dsp::Reverb reverb;

    // Dry buffer for wet/dry mix
    juce::AudioBuffer<float> dryBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UnderwaterProcessor)
};
