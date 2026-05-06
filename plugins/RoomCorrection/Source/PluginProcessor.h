#pragma once

#include <JuceHeader.h>

class RoomCorrectionProcessor : public juce::AudioProcessor
{
public:
    RoomCorrectionProcessor();
    ~RoomCorrectionProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    // EQ curve data for UI display
    struct EQPoint { float freq; float gainDB; };
    static const std::vector<EQPoint>& getLeftCurve();
    static const std::vector<EQPoint>& getRightCurve();
    static float interpolateEQ (const std::vector<EQPoint>& points, float freq);

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::dsp::Convolution convolution;

    void generateAndLoadIR (double sampleRate);
    std::vector<float> generateFIR (const std::vector<EQPoint>& points,
                                    double sampleRate, int firLength);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RoomCorrectionProcessor)
};
