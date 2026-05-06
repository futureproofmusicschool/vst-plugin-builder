#pragma once

#include <JuceHeader.h>

class ReverbATronProcessor : public juce::AudioProcessor
{
public:
    ReverbATronProcessor();
    ~ReverbATronProcessor() override = default;

    // --- AudioProcessor interface ---
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Reverb-a-tron"; }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 10.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // --- Parameter tree ---
    juce::AudioProcessorValueTreeState apvts;

    // --- Pitch detection results (for UI access) ---
    std::atomic<float> detectedFrequency { 0.0f };
    std::atomic<float> detectedConfidence { 0.0f };
    std::atomic<int>   detectedMidiNote { -1 };

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    static juce::AudioProcessor::BusesProperties getBusesProperties();

private:
    // ─── Atomic parameter pointers (lock-free audio thread access) ───
    // Core
    std::atomic<float>* mixParam       = nullptr;
    std::atomic<float>* decayParam     = nullptr;
    std::atomic<float>* roomSizeParam  = nullptr;
    std::atomic<float>* preDelayParam  = nullptr;

    // Harmonic Resonance
    std::atomic<float>* resAmountParam     = nullptr;
    std::atomic<float>* resHarmonicsParam  = nullptr;
    std::atomic<float>* resQParam          = nullptr;
    std::atomic<float>* resTrackingParam   = nullptr;
    std::atomic<float>* resConfidenceParam = nullptr;

    // Frequency Sculpt
    std::atomic<float>* sculptLowCutParam  = nullptr;
    std::atomic<float>* sculptHighCutParam = nullptr;
    std::atomic<float>* sculptMidFreqParam = nullptr;
    std::atomic<float>* sculptMidGainParam = nullptr;
    std::atomic<float>* sculptMidQParam    = nullptr;

    // Flutter
    std::atomic<float>* flutterRateParam   = nullptr;
    std::atomic<float>* flutterDepthParam  = nullptr;
    std::atomic<float>* flutterShapeParam  = nullptr;

    // Beat Sync
    std::atomic<float>* syncEnableParam      = nullptr;
    std::atomic<float>* syncDivisionParam    = nullptr;
    std::atomic<float>* syncFlutterParam     = nullptr;

    // Output
    std::atomic<float>* stereoWidthParam = nullptr;
    std::atomic<float>* outputLevelParam = nullptr;

    // ─── DSP components ───

    // Pre-delay buffer
    juce::AudioBuffer<float> preDelayBuffer;
    int preDelayWritePos = 0;
    int maxPreDelaySamples = 0;

    // Reverb core: four parallel feedback delay lines per channel.
    static constexpr int kNumReverbLines = 4;
    using ReverbDelayLine = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>;
    std::array<ReverbDelayLine, kNumReverbLines> reverbDelayLines;
    std::array<std::array<float, 2>, kNumReverbLines> reverbDampingState {};
    std::array<float, kNumReverbLines> reverbBaseDelaySeconds { 0.0297f, 0.0371f, 0.0411f, 0.0437f };
    int maxReverbDelaySamples = 0;

    // Harmonic resonance filter bank (up to 8 harmonics, stereo)
    static constexpr int kMaxHarmonics = 8;
    juce::dsp::StateVariableTPTFilter<float> resonanceFilters[kMaxHarmonics];
    juce::AudioBuffer<float> resonanceBuffer;

    // Frequency sculpt
    juce::dsp::StateVariableTPTFilter<float> sculptHighPass;
    juce::dsp::StateVariableTPTFilter<float> sculptLowPass;
    juce::dsp::IIR::Filter<float> sculptMidFilterL;
    juce::dsp::IIR::Filter<float> sculptMidFilterR;

    // Flutter (modulated delay)
    juce::AudioBuffer<float> flutterDelayBuffer;
    int flutterDelayWritePos = 0;
    int maxFlutterDelaySamples = 0;
    float flutterPhase = 0.0f;
    float randomHoldValue = 0.0f;
    float drunkWalkValue = 0.0f;
    juce::Random flutterRandom;

    // FFT pitch detection
    static constexpr int kFFTOrder = 12;  // 4096 points
    static constexpr int kFFTSize = 1 << kFFTOrder;
    static constexpr int kHopSize = 1024;
    juce::dsp::FFT fft;
    juce::dsp::WindowingFunction<float> fftWindow;
    std::array<float, kFFTSize * 2> fftData;
    std::array<float, kFFTSize> fftInputBuffer;
    int fftInputPos = 0;
    int fftSamplesCollected = 0;
    int fftSamplesSinceLastAnalysis = 0;
    float smoothedFrequency = 0.0f;

    // Dry buffer for wet/dry mix
    juce::AudioBuffer<float> dryBuffer;
    juce::AudioBuffer<float> monoAnalysisBuffer;
    juce::AudioBuffer<float> harmonicSumBuffer;
    juce::AudioBuffer<float> harmonicTempBuffer;

    // Cached values
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;

    // Parameter smoothing
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> preDelaySmoothed;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> outputGainSmoothed;

    // ─── Internal methods ───
    void performFFTPitchDetection(const float* inputData, int numSamples);
    float getLFOValue(float phase, int shape);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbATronProcessor)
};
