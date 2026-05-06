#include "PluginProcessor.h"
#include "PluginEditor.h"

// ─── Parameter Layout ────────────────────────────────────────────────────────

juce::AudioProcessorValueTreeState::ParameterLayout ReverbATronProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // === Core Parameters ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "mix", 1 }, "Dry/Wet",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 30.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "decay_time", 1 }, "Decay",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.4f), 2.0f,
        juce::AudioParameterFloatAttributes().withLabel("s")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "room_size", 1 }, "Size",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "pre_delay", 1 }, "Pre-Delay",
        juce::NormalisableRange<float>(0.0f, 250.0f, 0.1f), 20.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    // === Harmonic Resonance ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "res_amount", 1 }, "Resonance Amount",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "res_harmonics", 1 }, "Harmonics",
        juce::NormalisableRange<float>(1.0f, 8.0f, 1.0f), 4.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "res_q", 1 }, "Resonance Q",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.5f), 2.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "res_tracking", 1 }, "Tracking Speed",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 70.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "res_confidence", 1 }, "Confidence Threshold",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // === Frequency Sculpt ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "sculpt_low_cut", 1 }, "Low Cut",
        juce::NormalisableRange<float>(20.0f, 2000.0f, 1.0f, 0.3f), 80.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "sculpt_high_cut", 1 }, "High Cut",
        juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0f, 0.3f), 12000.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "sculpt_mid_freq", 1 }, "Mid Freq",
        juce::NormalisableRange<float>(200.0f, 8000.0f, 1.0f, 0.3f), 1000.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "sculpt_mid_gain", 1 }, "Mid Gain",
        juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "sculpt_mid_q", 1 }, "Mid Q",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.5f), 1.0f));

    // === Flutter ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "flutter_rate", 1 }, "Flutter Rate",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f), 2.5f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "flutter_depth", 1 }, "Flutter Depth",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 30.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "flutter_shape", 1 }, "Flutter Shape",
        juce::NormalisableRange<float>(0.0f, 3.0f, 1.0f), 0.0f));

    // === Beat Sync ===
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "sync_enable", 1 }, "Sync Enable", false));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "sync_division", 1 }, "Sync Division",
        juce::StringArray { "1/16", "1/8", "1/4", "1/2", "1 Bar", "2 Bars", "4 Bars" },
        2));  // default: 1/4

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "sync_flutter_rate", 1 }, "Sync Flutter", false));

    // === Output ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "stereo_width", 1 }, "Width",
        juce::NormalisableRange<float>(0.0f, 200.0f, 0.1f), 100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "output_level", 1 }, "Output Level",
        juce::NormalisableRange<float>(-24.0f, 6.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    return { params.begin(), params.end() };
}

// ─── Bus Layout ──────────────────────────────────────────────────────────────

juce::AudioProcessor::BusesProperties ReverbATronProcessor::getBusesProperties()
{
    return BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true);
}

// ─── Constructor ─────────────────────────────────────────────────────────────

ReverbATronProcessor::ReverbATronProcessor()
    : AudioProcessor(getBusesProperties()),
      apvts(*this, nullptr, juce::Identifier("REVERBATRON_PARAMS"), createParameterLayout()),
      fft(kFFTOrder),
      fftWindow(kFFTSize, juce::dsp::WindowingFunction<float>::hann)
{
    fftData.fill(0.0f);
    fftInputBuffer.fill(0.0f);
}

// ─── Prepare / Release ───────────────────────────────────────────────────────

void ReverbATronProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;

    const auto numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    // Cache parameter pointers
    mixParam            = apvts.getRawParameterValue("mix");
    decayParam          = apvts.getRawParameterValue("decay_time");
    roomSizeParam       = apvts.getRawParameterValue("room_size");
    preDelayParam       = apvts.getRawParameterValue("pre_delay");
    resAmountParam      = apvts.getRawParameterValue("res_amount");
    resHarmonicsParam   = apvts.getRawParameterValue("res_harmonics");
    resQParam           = apvts.getRawParameterValue("res_q");
    resTrackingParam    = apvts.getRawParameterValue("res_tracking");
    resConfidenceParam  = apvts.getRawParameterValue("res_confidence");
    sculptLowCutParam   = apvts.getRawParameterValue("sculpt_low_cut");
    sculptHighCutParam  = apvts.getRawParameterValue("sculpt_high_cut");
    sculptMidFreqParam  = apvts.getRawParameterValue("sculpt_mid_freq");
    sculptMidGainParam  = apvts.getRawParameterValue("sculpt_mid_gain");
    sculptMidQParam     = apvts.getRawParameterValue("sculpt_mid_q");
    flutterRateParam    = apvts.getRawParameterValue("flutter_rate");
    flutterDepthParam   = apvts.getRawParameterValue("flutter_depth");
    flutterShapeParam   = apvts.getRawParameterValue("flutter_shape");
    syncEnableParam     = apvts.getRawParameterValue("sync_enable");
    syncDivisionParam   = apvts.getRawParameterValue("sync_division");
    syncFlutterParam    = apvts.getRawParameterValue("sync_flutter_rate");
    stereoWidthParam    = apvts.getRawParameterValue("stereo_width");
    outputLevelParam    = apvts.getRawParameterValue("output_level");

    // DSP spec
    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels      = numChannels;

    // Pre-delay buffer (max 250ms)
    maxPreDelaySamples = static_cast<int>(sampleRate * 0.25) + 1;
    preDelayBuffer.setSize(static_cast<int>(numChannels), maxPreDelaySamples);
    preDelayBuffer.clear();
    preDelayWritePos = 0;

    // Reverb delay network
    maxReverbDelaySamples = static_cast<int>(sampleRate * 0.12) + 1;
    for (auto& delayLine : reverbDelayLines)
    {
        delayLine.setMaximumDelayInSamples(maxReverbDelaySamples);
        delayLine.prepare(spec);
        delayLine.reset();
    }
    for (auto& states : reverbDampingState)
        states = { 0.0f, 0.0f };

    // Harmonic resonance filters
    for (int i = 0; i < kMaxHarmonics; ++i)
    {
        resonanceFilters[i].prepare(spec);
        resonanceFilters[i].setType(juce::dsp::StateVariableTPTFilter<float>::Type::bandpass);
        resonanceFilters[i].setCutoffFrequency(440.0f * (i + 1));
        resonanceFilters[i].setResonance(2.0f);
    }
    // Frequency sculpt
    sculptHighPass.prepare(spec);
    sculptHighPass.setType(juce::dsp::StateVariableTPTFilter<float>::Type::highpass);
    sculptHighPass.setCutoffFrequency(80.0f);

    sculptLowPass.prepare(spec);
    sculptLowPass.setType(juce::dsp::StateVariableTPTFilter<float>::Type::lowpass);
    sculptLowPass.setCutoffFrequency(12000.0f);

    juce::dsp::ProcessSpec monoSpec = spec;
    monoSpec.numChannels = 1;
    sculptMidFilterL.prepare(monoSpec);
    sculptMidFilterR.prepare(monoSpec);

    // Flutter delay buffer (max 20ms modulation depth)
    maxFlutterDelaySamples = static_cast<int>(sampleRate * 0.02) + 1;
    flutterDelayBuffer.setSize(static_cast<int>(numChannels), maxFlutterDelaySamples);
    flutterDelayBuffer.clear();
    flutterDelayWritePos = 0;
    flutterPhase = 0.0f;
    randomHoldValue = flutterRandom.nextFloat() * 2.0f - 1.0f;
    drunkWalkValue = 0.0f;

    // FFT
    fftData.fill(0.0f);
    fftInputBuffer.fill(0.0f);
    fftInputPos = 0;
    fftSamplesCollected = 0;
    fftSamplesSinceLastAnalysis = 0;
    smoothedFrequency = 0.0f;

    // Working buffers
    dryBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);
    monoAnalysisBuffer.setSize(1, samplesPerBlock);
    harmonicSumBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);
    harmonicTempBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);
    resonanceBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);

    // Smoothed parameters
    preDelaySmoothed.reset(sampleRate, 0.02);
    preDelaySmoothed.setCurrentAndTargetValue(preDelayParam->load());
    outputGainSmoothed.reset(sampleRate, 0.02);
    outputGainSmoothed.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(outputLevelParam->load()));
}

void ReverbATronProcessor::releaseResources()
{
    for (auto& delayLine : reverbDelayLines)
        delayLine.reset();
    for (int i = 0; i < kMaxHarmonics; ++i)
        resonanceFilters[i].reset();
    sculptHighPass.reset();
    sculptLowPass.reset();
    sculptMidFilterL.reset();
    sculptMidFilterR.reset();
}

// ─── FFT Pitch Detection ────────────────────────────────────────────────────

void ReverbATronProcessor::performFFTPitchDetection(const float* inputData, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        fftInputBuffer[static_cast<size_t>(fftInputPos)] = inputData[i];
        fftInputPos = (fftInputPos + 1) % kFFTSize;
        fftSamplesCollected = juce::jmin(fftSamplesCollected + 1, kFFTSize);
        ++fftSamplesSinceLastAnalysis;

        if (fftSamplesCollected == kFFTSize && fftSamplesSinceLastAnalysis >= kHopSize)
        {
            fftSamplesSinceLastAnalysis = 0;

            // Copy the latest kFFTSize samples from the ring buffer into a linear FFT buffer.
            for (int j = 0; j < kFFTSize; ++j)
            {
                const int sourceIndex = (fftInputPos + j) % kFFTSize;
                fftData[static_cast<size_t>(j)] = fftInputBuffer[static_cast<size_t>(sourceIndex)];
            }
            std::fill(fftData.begin() + kFFTSize, fftData.end(), 0.0f);

            fftWindow.multiplyWithWindowingTable(fftData.data(), static_cast<size_t>(kFFTSize));
            fft.performRealOnlyForwardTransform(fftData.data());

            float maxMag = 0.0f;
            int maxBin = 0;
            float totalMag = 0.0f;
            int numBins = kFFTSize / 2;

            for (int j = 2; j < numBins; ++j)
            {
                float real = fftData[static_cast<size_t>(j * 2)];
                float imag = fftData[static_cast<size_t>(j * 2 + 1)];
                float mag = real * real + imag * imag;
                totalMag += mag;

                if (mag > maxMag)
                {
                    maxMag = mag;
                    maxBin = j;
                }
            }

            float avgMag = totalMag / static_cast<float>(numBins - 2);
            float confidence = (avgMag > 0.0f) ? (maxMag / (avgMag * static_cast<float>(numBins))) : 0.0f;
            confidence = juce::jlimit(0.0f, 1.0f, confidence * 5.0f);

            float freq = static_cast<float>(maxBin) * static_cast<float>(currentSampleRate) / static_cast<float>(kFFTSize);

            if (freq < 20.0f || freq > 10000.0f)
            {
                confidence = 0.0f;
                freq = smoothedFrequency;
            }

            float trackingSpeed = resTrackingParam->load() / 100.0f;
            float alpha = 0.01f + trackingSpeed * 0.29f;
            smoothedFrequency = smoothedFrequency + alpha * (freq - smoothedFrequency);

            detectedFrequency.store(smoothedFrequency);
            detectedConfidence.store(confidence);

            if (smoothedFrequency > 20.0f)
            {
                float midiNote = 69.0f + 12.0f * std::log2(smoothedFrequency / 440.0f);
                detectedMidiNote.store(static_cast<int>(std::round(midiNote)));
            }
            else
            {
                detectedMidiNote.store(-1);
            }
        }
    }
}

// ─── LFO ─────────────────────────────────────────────────────────────────────

float ReverbATronProcessor::getLFOValue(float phase, int shape)
{
    switch (shape)
    {
        case 0:  // Sine
            return std::sin(phase * juce::MathConstants<float>::twoPi);

        case 1:  // Triangle
        {
            float t = std::fmod(phase, 1.0f);
            return (t < 0.5f) ? (4.0f * t - 1.0f) : (3.0f - 4.0f * t);
        }

        case 2:  // Random (sample & hold)
            return randomHoldValue;

        case 3:  // Drunk walk
            return drunkWalkValue;

        default:
            return 0.0f;
    }
}

// ─── Process Block ───────────────────────────────────────────────────────────

void ReverbATronProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                         juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;

    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    // Clear unused output channels
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    // Read parameters
    const float mix           = mixParam->load() / 100.0f;
    const float decayTime     = decayParam->load();
    const float roomSize      = roomSizeParam->load() / 100.0f;
    const float preDelayMs    = preDelayParam->load();
    const float resAmount     = resAmountParam->load() / 100.0f;
    const int   numHarmonics  = static_cast<int>(resHarmonicsParam->load());
    const float resQ          = resQParam->load();
    const float confidenceThreshold = resConfidenceParam->load() / 100.0f;
    const float sculptLowCut  = sculptLowCutParam->load();
    const float sculptHighCut = sculptHighCutParam->load();
    const float sculptMidFreq = sculptMidFreqParam->load();
    const float sculptMidGain = sculptMidGainParam->load();
    const float sculptMidQ    = sculptMidQParam->load();
    const float flutterRate   = flutterRateParam->load();
    const float flutterDepth  = flutterDepthParam->load() / 100.0f;
    const int   flutterShape  = static_cast<int>(flutterShapeParam->load());
    const bool  syncEnable    = syncEnableParam->load() > 0.5f;
    const int   syncDivision  = static_cast<int>(syncDivisionParam->load());
    const bool  syncFlutter   = syncFlutterParam->load() > 0.5f;
    const float stereoWidth   = stereoWidthParam->load() / 100.0f;
    const float outputLevel   = juce::Decibels::decibelsToGain(outputLevelParam->load());

    // Beat sync: calculate tempo-locked decay if enabled
    float effectiveDecay = decayTime;
    float effectiveFlutterRate = flutterRate;

    if (syncEnable)
    {
        auto* playHead = getPlayHead();
        if (playHead != nullptr)
        {
            auto posInfo = playHead->getPosition();
            if (posInfo.hasValue() && posInfo->getBpm().hasValue())
            {
                double bpm = *posInfo->getBpm();
                if (bpm > 0.0)
                {
                    // Beat divisions: 1/16, 1/8, 1/4, 1/2, 1bar, 2bar, 4bar
                    static const double divisions[] = { 0.25, 0.5, 1.0, 2.0, 4.0, 8.0, 16.0 };
                    double beats = divisions[juce::jlimit(0, 6, syncDivision)];
                    effectiveDecay = static_cast<float>((60.0 / bpm) * beats);
                    effectiveDecay = juce::jlimit(0.1f, 10.0f, effectiveDecay);

                    if (syncFlutter)
                    {
                        // Sync flutter to 1/4 of the beat division
                        effectiveFlutterRate = static_cast<float>(bpm / (60.0 * beats * 0.25));
                        effectiveFlutterRate = juce::jlimit(0.1f, 10.0f, effectiveFlutterRate);
                    }
                }
            }
        }
    }

    jassert(numSamples <= currentBlockSize);

    // === Save dry copy ===
    for (int ch = 0; ch < totalNumOutputChannels; ++ch)
        dryBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);

    // === FFT Pitch Detection (mono sum for analysis) ===
    {
        auto* monoData = monoAnalysisBuffer.getWritePointer(0);
        if (totalNumInputChannels >= 2)
        {
            const auto* left = buffer.getReadPointer(0);
            const auto* right = buffer.getReadPointer(1);
            for (int n = 0; n < numSamples; ++n)
                monoData[n] = 0.5f * (left[n] + right[n]);
        }
        else if (totalNumInputChannels == 1)
        {
            monoAnalysisBuffer.copyFrom(0, 0, buffer, 0, 0, numSamples);
        }
        else
        {
            monoAnalysisBuffer.clear();
        }

        performFFTPitchDetection(monoData, numSamples);
    }

    // === Pre-Delay ===
    {
        preDelaySmoothed.setTargetValue(preDelayMs);
        for (int n = 0; n < numSamples; ++n)
        {
            const int delaySamples = juce::jlimit(0, maxPreDelaySamples - 1,
                static_cast<int>(preDelaySmoothed.getNextValue() * 0.001f * static_cast<float>(currentSampleRate)));
            int readPos = (preDelayWritePos - delaySamples + maxPreDelaySamples) % maxPreDelaySamples;

            for (int ch = 0; ch < totalNumOutputChannels; ++ch)
            {
                auto* data = buffer.getWritePointer(ch);
                auto* delayData = preDelayBuffer.getWritePointer(ch);

                delayData[preDelayWritePos] = data[n];
                data[n] = delayData[readPos];
            }

            preDelayWritePos = (preDelayWritePos + 1) % maxPreDelaySamples;
        }
    }

    // === Reverb ===
    {
        const float roomScale = 0.6f + roomSize * 0.9f;
        const float dampingCoeff = 0.2f + roomSize * 0.5f;

        for (int line = 0; line < kNumReverbLines; ++line)
        {
            const float baseDelaySamples = reverbBaseDelaySeconds[static_cast<size_t>(line)]
                                           * roomScale
                                           * static_cast<float>(currentSampleRate);
            reverbDelayLines[static_cast<size_t>(line)].setDelay(baseDelaySamples);
        }

        for (int ch = 0; ch < totalNumOutputChannels; ++ch)
        {
            auto* channelData = buffer.getWritePointer(ch);

            for (int n = 0; n < numSamples; ++n)
            {
                const float input = channelData[n];
                float wet = 0.0f;

                for (int line = 0; line < kNumReverbLines; ++line)
                {
                    const float delaySamples = reverbDelayLines[static_cast<size_t>(line)].getDelay();
                    const float delaySeconds = delaySamples / static_cast<float>(currentSampleRate);
                    const float feedbackGain = std::pow(0.001f, delaySeconds / juce::jmax(0.1f, effectiveDecay));
                    const float delayed = reverbDelayLines[static_cast<size_t>(line)].popSample(ch, delaySamples);

                    auto& dampingState = reverbDampingState[static_cast<size_t>(line)][static_cast<size_t>(ch)];
                    const float dampedFeedback = dampingCoeff * dampingState + (1.0f - dampingCoeff) * delayed;
                    dampingState = dampedFeedback;

                    const float excitation = input + dampedFeedback * feedbackGain;
                    reverbDelayLines[static_cast<size_t>(line)].pushSample(ch, excitation);
                    wet += delayed;
                }

                channelData[n] = wet / static_cast<float>(kNumReverbLines);
            }
        }
    }

    // === Harmonic Resonance ===
    {
        float currentFreq = detectedFrequency.load();
        float currentConf = detectedConfidence.load();

        // Blend factor: how much resonance to apply
        float confBlend = (currentConf >= confidenceThreshold) ?
            juce::jlimit(0.0f, 1.0f, (currentConf - confidenceThreshold) / (1.0f - confidenceThreshold + 0.001f)) : 0.0f;
        float resonanceBlend = resAmount * confBlend;

        if (resonanceBlend > 0.001f && currentFreq > 20.0f)
        {
            for (int ch = 0; ch < totalNumOutputChannels; ++ch)
                resonanceBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);

            harmonicSumBuffer.clear();

            for (int h = 0; h < juce::jmin(numHarmonics, kMaxHarmonics); ++h)
            {
                float harmonicFreq = currentFreq * static_cast<float>(h + 1);
                if (harmonicFreq > 20000.0f) break;

                resonanceFilters[h].setCutoffFrequency(harmonicFreq);
                resonanceFilters[h].setResonance(resQ);

                for (int ch = 0; ch < totalNumOutputChannels; ++ch)
                    harmonicTempBuffer.copyFrom(ch, 0, resonanceBuffer, ch, 0, numSamples);

                juce::dsp::AudioBlock<float> tempBlock(harmonicTempBuffer);
                juce::dsp::ProcessContextReplacing<float> tempContext(tempBlock);
                resonanceFilters[h].process(tempContext);

                for (int ch = 0; ch < totalNumOutputChannels; ++ch)
                    harmonicSumBuffer.addFrom(ch, 0, harmonicTempBuffer, ch, 0, numSamples);
            }

            for (int ch = 0; ch < totalNumOutputChannels; ++ch)
            {
                auto* out = buffer.getWritePointer(ch);
                const auto* harmData = harmonicSumBuffer.getReadPointer(ch);

                for (int n = 0; n < numSamples; ++n)
                    out[n] = (1.0f - resonanceBlend) * out[n] + resonanceBlend * harmData[n];
            }
        }
    }

    // === Frequency Sculpt ===
    {
        sculptHighPass.setCutoffFrequency(sculptLowCut);
        sculptLowPass.setCutoffFrequency(sculptHighCut);

        juce::dsp::AudioBlock<float> sculptBlock(buffer);
        juce::dsp::ProcessContextReplacing<float> sculptContext(sculptBlock);
        sculptHighPass.process(sculptContext);
        sculptLowPass.process(sculptContext);

        // Parametric mid EQ
        if (std::abs(sculptMidGain) > 0.05f)
        {
            auto coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                currentSampleRate, sculptMidFreq, sculptMidQ,
                juce::Decibels::decibelsToGain(sculptMidGain));

            *sculptMidFilterL.coefficients = *coeffs;
            *sculptMidFilterR.coefficients = *coeffs;

            if (totalNumOutputChannels >= 1)
            {
                auto leftBlock = sculptBlock.getSingleChannelBlock(0);
                juce::dsp::ProcessContextReplacing<float> leftCtx(leftBlock);
                sculptMidFilterL.process(leftCtx);
            }
            if (totalNumOutputChannels >= 2)
            {
                auto rightBlock = sculptBlock.getSingleChannelBlock(1);
                juce::dsp::ProcessContextReplacing<float> rightCtx(rightBlock);
                sculptMidFilterR.process(rightCtx);
            }
        }
    }

    // === Flutter ===
    if (flutterDepth > 0.001f)
    {
        float maxDelaySec = 0.005f * flutterDepth;  // up to 5ms modulation
        float phaseInc = effectiveFlutterRate / static_cast<float>(currentSampleRate);

        for (int n = 0; n < numSamples; ++n)
        {
            // Calculate modulated delay (same for all channels)
            float lfoVal = getLFOValue(flutterPhase, flutterShape);
            float delaySec = maxDelaySec * (0.5f + 0.5f * lfoVal);
            float delaySamplesF = delaySec * static_cast<float>(currentSampleRate);
            delaySamplesF = juce::jlimit(0.0f, static_cast<float>(maxFlutterDelaySamples - 2), delaySamplesF);

            // Linear interpolation for fractional delay
            int delayInt = static_cast<int>(delaySamplesF);
            float frac = delaySamplesF - static_cast<float>(delayInt);

            int readPos0 = (flutterDelayWritePos - delayInt + maxFlutterDelaySamples) % maxFlutterDelaySamples;
            int readPos1 = (readPos0 - 1 + maxFlutterDelaySamples) % maxFlutterDelaySamples;

            for (int ch = 0; ch < totalNumOutputChannels; ++ch)
            {
                auto* data = buffer.getWritePointer(ch);
                auto* delayData = flutterDelayBuffer.getWritePointer(ch);

                // Write to delay buffer
                delayData[flutterDelayWritePos] = data[n];

                // Read with interpolation
                data[n] = delayData[readPos0] * (1.0f - frac) + delayData[readPos1] * frac;
            }

            // Advance write position once per sample
            flutterDelayWritePos = (flutterDelayWritePos + 1) % maxFlutterDelaySamples;

            // Advance LFO phase once per sample
            flutterPhase += phaseInc;
            if (flutterPhase >= 1.0f)
            {
                flutterPhase -= 1.0f;
                if (flutterShape == 2)
                    randomHoldValue = flutterRandom.nextFloat() * 2.0f - 1.0f;

                if (flutterShape == 3)
                {
                    drunkWalkValue += (flutterRandom.nextFloat() * 2.0f - 1.0f) * 0.3f;
                    drunkWalkValue = juce::jlimit(-1.0f, 1.0f, drunkWalkValue);
                }
            }
        }
    }

    // === Stereo Width (M/S Processing) ===
    if (totalNumOutputChannels >= 2 && std::abs(stereoWidth - 1.0f) > 0.001f)
    {
        auto* leftData  = buffer.getWritePointer(0);
        auto* rightData = buffer.getWritePointer(1);

        for (int n = 0; n < numSamples; ++n)
        {
            float mid  = (leftData[n] + rightData[n]) * 0.5f;
            float side = (leftData[n] - rightData[n]) * 0.5f;
            side *= stereoWidth;
            leftData[n]  = mid + side;
            rightData[n] = mid - side;
        }
    }

    // === Output Level ===
    outputGainSmoothed.setTargetValue(outputLevel);
    float* outputChannels[2] {};
    for (int ch = 0; ch < juce::jmin(totalNumOutputChannels, 2); ++ch)
        outputChannels[ch] = buffer.getWritePointer(ch);

    for (int n = 0; n < numSamples; ++n)
    {
        const float gain = outputGainSmoothed.getNextValue();
        for (int ch = 0; ch < totalNumOutputChannels; ++ch)
            outputChannels[ch][n] *= gain;
    }

    // === Wet/Dry Mix ===
    for (int ch = 0; ch < totalNumOutputChannels; ++ch)
    {
        auto* wetData = buffer.getWritePointer(ch);
        const auto* dryData = dryBuffer.getReadPointer(ch);

        for (int n = 0; n < numSamples; ++n)
            wetData[n] = mix * wetData[n] + (1.0f - mix) * dryData[n];
    }
}

// ─── State Serialization ─────────────────────────────────────────────────────

void ReverbATronProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ReverbATronProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

// ─── Editor ──────────────────────────────────────────────────────────────────

juce::AudioProcessorEditor* ReverbATronProcessor::createEditor()
{
    return new ReverbATronEditor(*this);
}

// ─── Plugin Instantiation ────────────────────────────────────────────────────

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReverbATronProcessor();
}
