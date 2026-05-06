#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Sonarworks Reference 4 calibration curves (read from screenshots)
// These are the CORRECTION curves — what the EQ applies to flatten the room.
//==============================================================================

// Auto-extracted from Sonarworks Reference 4 screenshots via pixel analysis
// 145 points per channel, calibrated from axis labels (426 px/decade, 13 px/dB)
static const std::vector<RoomCorrectionProcessor::EQPoint> leftCurveData = {
    {     18.4f,  -2.95f }, {     19.3f,  -3.53f }, {     20.2f,  -4.15f },
    {     21.2f,  -4.88f }, {     22.3f,  -5.71f }, {     23.4f,  -6.65f },
    {     24.5f,  -7.67f }, {     25.6f,  -9.13f }, {     26.9f, -11.00f },
    {     28.2f, -12.93f }, {     29.5f, -14.99f }, {     30.9f, -16.93f },
    {     32.4f, -18.19f }, {     33.9f, -18.74f }, {     35.6f, -18.39f },
    {     37.4f, -17.86f }, {     39.2f, -17.49f }, {     41.0f, -17.30f },
    {     43.0f, -17.17f }, {     45.1f, -16.89f }, {     47.2f, -16.33f },
    {     49.5f, -15.38f }, {     52.0f, -14.20f }, {     54.4f, -13.17f },
    {     57.0f, -12.35f }, {     59.8f, -11.76f }, {     62.7f, -11.52f },
    {     65.6f, -11.58f }, {     68.9f, -11.86f }, {     72.3f, -12.08f },
    {     75.7f, -11.93f }, {     79.3f, -11.23f }, {     83.2f,  -9.64f },
    {     87.1f,  -7.37f }, {     91.2f,  -5.44f }, {     95.8f,  -4.60f },
    {    100.6f,  -5.45f }, {    105.3f,  -7.41f }, {    110.2f,  -8.99f },
    {    115.7f,  -8.51f }, {    121.2f,  -6.65f }, {    126.9f,  -4.65f },
    {    133.2f,  -3.20f }, {    139.4f,  -2.81f }, {    146.0f,  -2.97f },
    {    153.3f,  -2.68f }, {    160.9f,  -1.31f }, {    168.5f,   0.88f },
    {    176.4f,   2.49f }, {    185.2f,   3.26f }, {    193.9f,   4.70f },
    {    203.0f,   5.81f }, {    213.1f,   5.39f }, {    223.8f,   4.19f },
    {    234.3f,   3.17f }, {    245.3f,   2.22f }, {    257.5f,   1.18f },
    {    269.6f,   0.94f }, {    282.3f,   0.82f }, {    296.4f,   0.42f },
    {    311.2f,  -0.14f }, {    325.8f,  -1.24f }, {    341.1f,  -2.56f },
    {    358.1f,  -3.39f }, {    375.0f,  -3.49f }, {    392.6f,  -3.11f },
    {    412.2f,  -2.45f }, {    432.7f,  -1.35f }, {    453.0f,   0.09f },
    {    474.3f,   1.34f }, {    498.0f,   2.29f }, {    521.4f,   3.60f },
    {    545.9f,   4.62f }, {    573.1f,   5.17f }, {    601.7f,   4.80f },
    {    630.0f,   3.12f }, {    659.6f,   1.55f }, {    692.5f,   0.86f },
    {    725.0f,   1.11f }, {    759.1f,   1.84f }, {    797.0f,   2.69f },
    {    834.4f,   2.79f }, {    873.7f,   2.17f }, {    917.2f,   1.53f },
    {    963.0f,   0.82f }, {   1008.2f,   0.20f }, {   1055.6f,   0.18f },
    {   1108.3f,   0.98f }, {   1160.3f,   1.73f }, {   1214.9f,   2.12f },
    {   1275.5f,   2.39f }, {   1339.1f,   2.49f }, {   1402.0f,   2.29f },
    {   1467.9f,   2.12f }, {   1541.1f,   2.16f }, {   1613.5f,   2.22f },
    {   1689.4f,   1.81f }, {   1773.7f,   1.08f }, {   1862.1f,   0.46f },
    {   1949.6f,  -0.03f }, {   2041.3f,  -0.92f }, {   2143.0f,  -1.83f },
    {   2243.7f,  -2.56f }, {   2349.3f,  -3.51f }, {   2466.4f,  -4.31f },
    {   2589.3f,  -4.40f }, {   2711.0f,  -4.07f }, {   2838.5f,  -3.57f },
    {   2980.0f,  -3.25f }, {   3120.1f,  -3.19f }, {   3266.8f,  -3.12f },
    {   3429.7f,  -2.91f }, {   3590.9f,  -2.42f }, {   3759.8f,  -2.39f },
    {   3947.2f,  -2.56f }, {   4143.9f,  -2.43f }, {   4338.7f,  -2.02f },
    {   4542.8f,  -2.04f }, {   4769.2f,  -1.99f }, {   4993.3f,  -1.49f },
    {   5228.2f,  -1.48f }, {   5488.8f,  -1.87f }, {   5762.4f,  -1.98f },
    {   6033.2f,  -1.89f }, {   6317.0f,  -1.90f }, {   6631.9f,  -1.80f },
    {   6943.6f,  -1.82f }, {   7270.2f,  -1.87f }, {   7632.6f,  -1.66f },
    {   8013.1f,  -1.55f }, {   8389.6f,  -1.03f }, {   8784.2f,  -0.45f },
    {   9222.1f,   0.23f }, {   9655.5f,   0.43f }, {  10109.7f,   0.55f },
    {  10613.6f,   0.69f }, {  11142.7f,   0.91f }, {  11666.3f,   1.14f },
    {  12215.1f,   1.32f }, {  12824.0f,   1.31f }, {  13426.6f,   1.20f },
    {  14058.2f,   1.02f }, {  14759.0f,   0.89f }, {  15494.7f,   0.91f },
    {  16178.8f,   0.85f }
};

static const std::vector<RoomCorrectionProcessor::EQPoint> rightCurveData = {
    {     18.4f,  -2.96f }, {     19.3f,  -3.55f }, {     20.2f,  -4.19f },
    {     21.2f,  -4.91f }, {     22.3f,  -5.76f }, {     23.4f,  -6.69f },
    {     24.5f,  -7.73f }, {     25.6f,  -9.21f }, {     26.9f, -11.11f },
    {     28.2f, -13.06f }, {     29.5f, -15.08f }, {     30.9f, -17.01f },
    {     32.4f, -18.26f }, {     33.9f, -18.78f }, {     35.6f, -18.45f },
    {     37.4f, -17.96f }, {     39.2f, -17.60f }, {     41.0f, -17.41f },
    {     43.0f, -17.23f }, {     45.1f, -16.91f }, {     47.2f, -16.32f },
    {     49.5f, -15.38f }, {     52.0f, -14.24f }, {     54.4f, -13.21f },
    {     57.0f, -12.37f }, {     59.8f, -11.69f }, {     62.7f, -11.37f },
    {     65.6f, -11.37f }, {     68.9f, -11.60f }, {     72.3f, -11.78f },
    {     75.7f, -11.55f }, {     79.3f, -10.77f }, {     83.2f,  -9.14f },
    {     87.1f,  -6.84f }, {     91.2f,  -4.74f }, {     95.8f,  -3.60f },
    {    100.6f,  -4.06f }, {    105.3f,  -5.67f }, {    110.2f,  -7.00f },
    {    115.7f,  -6.30f }, {    121.2f,  -4.00f }, {    126.9f,  -1.46f },
    {    133.2f,   0.39f }, {    139.4f,   1.01f }, {    146.0f,   0.81f },
    {    153.3f,   0.82f }, {    160.9f,   1.77f }, {    168.5f,   2.84f },
    {    176.4f,   2.23f }, {    185.2f,   0.52f }, {    193.9f,   0.53f },
    {    203.0f,   1.77f }, {    213.1f,   2.47f }, {    223.8f,   2.23f },
    {    234.3f,   2.10f }, {    245.3f,   1.65f }, {    257.5f,   0.07f },
    {    269.6f,  -1.40f }, {    282.3f,  -1.98f }, {    296.4f,  -1.98f },
    {    311.2f,  -1.87f }, {    325.8f,  -1.87f }, {    341.1f,  -2.35f },
    {    358.1f,  -2.96f }, {    375.0f,  -3.00f }, {    392.6f,  -2.65f },
    {    412.2f,  -2.16f }, {    432.7f,  -1.49f }, {    453.0f,  -0.58f },
    {    474.3f,   0.26f }, {    498.0f,   0.96f }, {    521.4f,   1.45f },
    {    545.9f,   1.92f }, {    573.1f,   2.49f }, {    601.7f,   3.41f },
    {    630.0f,   4.09f }, {    659.6f,   4.07f }, {    692.5f,   3.62f },
    {    725.0f,   3.00f }, {    759.1f,   2.16f }, {    797.0f,   1.65f },
    {    834.4f,   2.09f }, {    873.7f,   2.15f }, {    917.2f,   1.75f },
    {    963.0f,   1.19f }, {   1008.2f,   0.92f }, {   1055.6f,   0.66f },
    {   1108.3f,   0.81f }, {   1160.3f,   1.57f }, {   1214.9f,   2.19f },
    {   1275.5f,   2.47f }, {   1339.1f,   2.78f }, {   1402.0f,   2.96f },
    {   1467.9f,   2.84f }, {   1541.1f,   2.66f }, {   1613.5f,   2.55f },
    {   1689.4f,   2.06f }, {   1773.7f,   1.40f }, {   1862.1f,   0.87f },
    {   1949.6f,   0.61f }, {   2041.3f,  -0.14f }, {   2143.0f,  -1.05f },
    {   2243.7f,  -1.93f }, {   2349.3f,  -2.82f }, {   2466.4f,  -3.65f },
    {   2589.3f,  -3.84f }, {   2711.0f,  -3.67f }, {   2838.5f,  -3.65f },
    {   2980.0f,  -3.47f }, {   3120.1f,  -3.26f }, {   3266.8f,  -3.02f },
    {   3429.7f,  -2.64f }, {   3590.9f,  -2.77f }, {   3759.8f,  -3.07f },
    {   3947.2f,  -2.82f }, {   4143.9f,  -2.34f }, {   4338.7f,  -2.36f },
    {   4542.8f,  -2.85f }, {   4769.2f,  -2.63f }, {   4993.3f,  -1.99f },
    {   5228.2f,  -1.97f }, {   5488.8f,  -2.21f }, {   5762.4f,  -2.42f },
    {   6033.2f,  -2.49f }, {   6317.0f,  -2.24f }, {   6631.9f,  -2.44f },
    {   6943.6f,  -2.49f }, {   7270.2f,  -2.40f }, {   7632.6f,  -2.52f },
    {   8013.1f,  -2.36f }, {   8389.6f,  -1.75f }, {   8784.2f,  -0.67f },
    {   9222.1f,   0.21f }, {   9655.5f,   0.57f }, {  10109.7f,   0.60f },
    {  10613.6f,   0.78f }, {  11142.7f,   0.83f }, {  11666.3f,   0.88f },
    {  12215.1f,   0.90f }, {  12824.0f,   0.83f }, {  13426.6f,   0.74f },
    {  14058.2f,   0.63f }, {  14759.0f,   0.56f }, {  15494.7f,   0.53f },
    {  16178.8f,   0.47f }
};

const std::vector<RoomCorrectionProcessor::EQPoint>& RoomCorrectionProcessor::getLeftCurve()  { return leftCurveData; }
const std::vector<RoomCorrectionProcessor::EQPoint>& RoomCorrectionProcessor::getRightCurve() { return rightCurveData; }

//==============================================================================
RoomCorrectionProcessor::RoomCorrectionProcessor()
    : AudioProcessor (BusesProperties()
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
}

RoomCorrectionProcessor::~RoomCorrectionProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout RoomCorrectionProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "bypass", 1 }, "Bypass", false));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "output_gain", 1 }, "Output Gain",
        juce::NormalisableRange<float> (-12.f, 6.f, 0.1f), 0.f));

    return { params.begin(), params.end() };
}

//==============================================================================
float RoomCorrectionProcessor::interpolateEQ (const std::vector<EQPoint>& points, float freq)
{
    if (freq <= 0.f) return 0.f;
    if (freq <= points.front().freq) return points.front().gainDB;
    if (freq >= points.back().freq)  return points.back().gainDB;

    for (size_t i = 0; i < points.size() - 1; ++i)
    {
        if (freq >= points[i].freq && freq <= points[i + 1].freq)
        {
            float logF  = std::log (freq);
            float logF0 = std::log (points[i].freq);
            float logF1 = std::log (points[i + 1].freq);
            float t = (logF - logF0) / (logF1 - logF0);
            return points[i].gainDB + t * (points[i + 1].gainDB - points[i].gainDB);
        }
    }
    return 0.f;
}

std::vector<float> RoomCorrectionProcessor::generateFIR (
    const std::vector<EQPoint>& points, double sampleRate, int N)
{
    int order = static_cast<int> (std::round (std::log2 (N)));
    juce::dsp::FFT fft (order);

    // Frequency-domain buffer: 2*N floats for real-only inverse transform
    std::vector<float> fftData (static_cast<size_t> (2 * N), 0.f);

    // Fill frequency bins with desired magnitude response (zero phase)
    for (int k = 0; k <= N / 2; ++k)
    {
        float freq = static_cast<float> (k) * static_cast<float> (sampleRate) / static_cast<float> (N);
        float gainDB = interpolateEQ (points, freq);
        float magnitude = juce::Decibels::decibelsToGain (gainDB);

        fftData[static_cast<size_t> (2 * k)]     = magnitude;  // real
        fftData[static_cast<size_t> (2 * k + 1)] = 0.f;        // imag (zero phase)
    }

    // Inverse FFT -> zero-phase impulse response
    fft.performRealOnlyInverseTransform (fftData.data());

    // Circular shift by N/2 to make causal, then apply Hann window
    std::vector<float> coeffs (static_cast<size_t> (N));
    for (int i = 0; i < N; ++i)
    {
        int srcIdx = (i + N / 2) % N;
        float window = 0.5f * (1.f - std::cos (2.f * juce::MathConstants<float>::pi
                                                * static_cast<float> (i)
                                                / static_cast<float> (N - 1)));
        coeffs[static_cast<size_t> (i)] = fftData[static_cast<size_t> (srcIdx)] * window;
    }

    return coeffs;
}

void RoomCorrectionProcessor::generateAndLoadIR (double sampleRate)
{
    // Choose FIR length based on sample rate for consistent frequency resolution
    int firLength = (sampleRate > 48000.0) ? 8192 : 4096;

    auto firL = generateFIR (leftCurveData,  sampleRate, firLength);
    auto firR = generateFIR (rightCurveData, sampleRate, firLength);

    juce::AudioBuffer<float> ir (2, firLength);
    ir.copyFrom (0, 0, firL.data(), firLength);
    ir.copyFrom (1, 0, firR.data(), firLength);

    convolution.loadImpulseResponse (std::move (ir),
                                     sampleRate,
                                     juce::dsp::Convolution::Stereo::yes,
                                     juce::dsp::Convolution::Trim::no,
                                     juce::dsp::Convolution::Normalise::no);
}

//==============================================================================
void RoomCorrectionProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Load IR before prepare so it's active on first process() call
    generateAndLoadIR (sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = 2;
    convolution.prepare (spec);

    setLatencySamples (convolution.getLatency());
}

void RoomCorrectionProcessor::releaseResources() {}

void RoomCorrectionProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                            juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    auto bypass = apvts.getRawParameterValue ("bypass")->load() > 0.5f;
    if (bypass)
        return;

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    convolution.process (context);

    // Apply output gain
    float gainDB = apvts.getRawParameterValue ("output_gain")->load();
    if (std::abs (gainDB) > 0.05f)
        buffer.applyGain (juce::Decibels::decibelsToGain (gainDB));
}

//==============================================================================
void RoomCorrectionProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void RoomCorrectionProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

//==============================================================================
juce::AudioProcessorEditor* RoomCorrectionProcessor::createEditor()
{
    return new RoomCorrectionEditor (*this);
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RoomCorrectionProcessor();
}
