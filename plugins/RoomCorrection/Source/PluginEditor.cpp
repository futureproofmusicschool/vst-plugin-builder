#include "PluginEditor.h"
#include "BinaryData.h"

//==============================================================================
// Fonts
//==============================================================================

static juce::Typeface::Ptr getPanchang()
{
    static auto tf = juce::Typeface::createSystemTypefaceFor (
        BinaryData::PanchangMedium_otf, BinaryData::PanchangMedium_otfSize);
    return tf;
}

static juce::Typeface::Ptr getSatoshi()
{
    static auto tf = juce::Typeface::createSystemTypefaceFor (
        BinaryData::SatoshiRegular_otf, BinaryData::SatoshiRegular_otfSize);
    return tf;
}

static juce::Typeface::Ptr getSatoshiBold()
{
    static auto tf = juce::Typeface::createSystemTypefaceFor (
        BinaryData::SatoshiBold_otf, BinaryData::SatoshiBold_otfSize);
    return tf;
}

juce::Font RoomCorrectionEditor::getTitleFont (float height)
{
    return juce::Font (juce::FontOptions().withTypeface (getPanchang()).withHeight (height));
}

juce::Font RoomCorrectionEditor::getHeadingFont (float height)
{
    return juce::Font (juce::FontOptions().withTypeface (getSatoshiBold()).withHeight (height));
}

juce::Font RoomCorrectionEditor::getBodyFont (float height)
{
    return juce::Font (juce::FontOptions().withTypeface (getSatoshi()).withHeight (height));
}

//==============================================================================
// LookAndFeel
//==============================================================================

RoomCorrectionEditor::LookAndFeel::LookAndFeel()
{
    using B = RoomCorrectionEditor::Brand;

    setColour (juce::Slider::rotarySliderFillColourId,    B::purple);
    setColour (juce::Slider::rotarySliderOutlineColourId, B::panel);
    setColour (juce::Slider::thumbColourId,               B::textPrimary);
    setColour (juce::Slider::textBoxTextColourId,         B::textPrimary);
    setColour (juce::Slider::textBoxOutlineColourId,      juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxBackgroundColourId,   B::glassFill);
    setColour (juce::Slider::trackColourId,               B::panel);

    setColour (juce::Label::textColourId, B::textPrimary);

    setColour (juce::TextButton::buttonColourId,   B::panel);
    setColour (juce::TextButton::buttonOnColourId,  B::purple);
    setColour (juce::TextButton::textColourOnId,    B::textPrimary);
    setColour (juce::TextButton::textColourOffId,   B::textPrimary);
}

//==============================================================================
// Editor
//==============================================================================

RoomCorrectionEditor::RoomCorrectionEditor (RoomCorrectionProcessor& p)
    : AudioProcessorEditor (p), processorRef (p)
{
    setLookAndFeel (&lnf);
    setSize (600, 400);

    // Bypass toggle
    bypassButton.setClickingTogglesState (true);
    addAndMakeVisible (bypassButton);
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        processorRef.apvts, "bypass", bypassButton);

    // Output gain knob
    gainSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    gainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 18);
    gainSlider.setTextValueSuffix (" dB");
    addAndMakeVisible (gainSlider);
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "output_gain", gainSlider);

    gainLabel.setText ("Output", juce::dontSendNotification);
    gainLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (gainLabel);
}

RoomCorrectionEditor::~RoomCorrectionEditor()
{
    setLookAndFeel (nullptr);
}

//==============================================================================
void RoomCorrectionEditor::paint (juce::Graphics& g)
{
    g.fillAll (Brand::background);

    auto bounds = getLocalBounds();

    // Header
    auto headerArea = bounds.removeFromTop (36);
    drawHeader (g, headerArea);

    // Controls bar at bottom
    auto controlsArea = bounds.removeFromBottom (70);
    g.setColour (Brand::panel);
    g.fillRect (controlsArea);
    g.setColour (Brand::border);
    g.drawLine (static_cast<float> (controlsArea.getX()),
                static_cast<float> (controlsArea.getY()),
                static_cast<float> (controlsArea.getRight()),
                static_cast<float> (controlsArea.getY()), 1.f);

    // Curve display fills the rest
    drawCurveDisplay (g, bounds.reduced (4, 4));
}

void RoomCorrectionEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop (36);   // header
    auto controlsArea = bounds.removeFromBottom (70);

    // Bypass button (left side of controls)
    bypassButton.setBounds (controlsArea.removeFromLeft (100).reduced (16, 16));

    // Gain knob (right side)
    auto gainArea = controlsArea.removeFromRight (100);
    gainSlider.setBounds (gainArea.removeFromTop (50).reduced (10, 2));
    gainLabel.setBounds (gainArea);
}

//==============================================================================
void RoomCorrectionEditor::drawHeader (juce::Graphics& g, juce::Rectangle<int> area)
{
    g.setColour (Brand::glassFill);
    g.fillRect (area);
    g.setColour (Brand::border);
    g.drawLine (static_cast<float> (area.getX()),
                static_cast<float> (area.getBottom()),
                static_cast<float> (area.getRight()),
                static_cast<float> (area.getBottom()), 1.f);

    auto padded = area.reduced (16, 0);

    // Split into three regions to prevent overlap
    auto leftArea   = padded.removeFromLeft (140);
    auto rightArea  = padded.removeFromRight (60);
    // Centre area is whatever remains

    // Brand name (left)
    g.setColour (Brand::purple);
    g.setFont (getTitleFont (14.f));
    g.drawText ("FUTUREPROOF", leftArea, juce::Justification::centredLeft);

    // Plugin name (centre)
    g.setColour (Brand::textPrimary);
    g.setFont (getHeadingFont (15.f));
    g.drawText ("ROOM CORRECTION", padded, juce::Justification::centred);

    // Version (right)
    g.setColour (Brand::textMuted);
    g.setFont (getBodyFont (12.f));
    g.drawText ("v1.0.0", rightArea, juce::Justification::centredRight);
}

//==============================================================================
void RoomCorrectionEditor::drawCurveDisplay (juce::Graphics& g, juce::Rectangle<int> area)
{
    auto bounds = area.toFloat();

    // Background panel
    g.setColour (Brand::panel.withAlpha (0.4f));
    g.fillRoundedRectangle (bounds, 8.f);
    g.setColour (Brand::border);
    g.drawRoundedRectangle (bounds, 8.f, 1.f);

    // Leave room for axis labels: 50px left for dB, 20px bottom for freq
    auto plotBounds = bounds.reduced (0.f, 8.f);
    plotBounds.removeFromLeft (50.f);
    plotBounds.removeFromBottom (20.f);

    // dB range matching Sonarworks: +18 to -18 dB
    float minDB = -19.f, maxDB = 19.f;
    float dbRange = maxDB - minDB;

    // Grid lines — dB (every 6 dB, matching Sonarworks: +18, +12, +6, 0, -6, -12, -18)
    g.setFont (getBodyFont (11.f));
    for (float db = -18.f; db <= 18.f; db += 6.f)
    {
        float normY = 1.f - (db - minDB) / dbRange;
        float y = plotBounds.getY() + normY * plotBounds.getHeight();

        if (std::abs (db) < 0.1f)
        {
            // 0 dB line — prominent
            g.setColour (juce::Colours::white.withAlpha (0.35f));
            g.drawHorizontalLine (static_cast<int> (y), plotBounds.getX(), plotBounds.getRight());
        }
        else
        {
            // Other dB lines — faint but visible
            g.setColour (juce::Colours::white.withAlpha (0.10f));
            g.drawHorizontalLine (static_cast<int> (y), plotBounds.getX(), plotBounds.getRight());
        }

        // dB label to the left of plot area
        g.setColour (Brand::textPrimary.withAlpha (0.7f));
        juce::String label = (db > 0 ? "+" : "") + juce::String (static_cast<int> (db)) + " dB";
        g.drawText (label,
                    static_cast<int> (bounds.getX() + 2), static_cast<int> (y - 8), 46, 16,
                    juce::Justification::centredRight);
    }

    // Grid lines — frequency (matching Sonarworks: "100 Hz", "1 kHz", "10 kHz")
    float minFreq = 20.f, maxFreq = 20000.f;
    float logMin = std::log10 (minFreq), logMax = std::log10 (maxFreq);
    float logRange = logMax - logMin;

    struct FreqLabel { int freq; const char* text; bool major; };
    FreqLabel freqLabels[] = {
        { 20,    "20",    false },
        { 30,    "30",    false },
        { 50,    "50",    false },
        { 100,   "100",   true  },
        { 200,   "200",   false },
        { 500,   "500",   false },
        { 1000,  "1k",    true  },
        { 2000,  "2k",    false },
        { 5000,  "5k",    false },
        { 10000, "10k",   true  },
        { 20000, "20k",   false }
    };
    g.setFont (getBodyFont (10.f));
    for (auto& fl : freqLabels)
    {
        float normX = (std::log10 (static_cast<float> (fl.freq)) - logMin) / logRange;
        float x = plotBounds.getX() + normX * plotBounds.getWidth();

        g.setColour (fl.major ? Brand::border.withAlpha (0.35f) : Brand::border.withAlpha (0.15f));
        g.drawVerticalLine (static_cast<int> (x), plotBounds.getY(), plotBounds.getBottom());

        g.setColour (Brand::textPrimary.withAlpha (fl.major ? 0.8f : 0.5f));
        g.drawText (fl.text,
                    static_cast<int> (x - 18),
                    static_cast<int> (plotBounds.getBottom() + 2),
                    36, 14, juce::Justification::centred);
    }

    // Additional minor grid lines (no labels)
    int minorFreqs[] = { 40, 60, 70, 80, 300, 400, 600, 700, 800,
                         3000, 4000, 6000, 7000, 8000, 15000 };
    for (int freq : minorFreqs)
    {
        float normX = (std::log10 (static_cast<float> (freq)) - logMin) / logRange;
        float x = plotBounds.getX() + normX * plotBounds.getWidth();
        g.setColour (Brand::border.withAlpha (0.08f));
        g.drawVerticalLine (static_cast<int> (x), plotBounds.getY(), plotBounds.getBottom());
    }

    // Draw curves
    auto leftColour  = Brand::purple;
    auto rightColour = Brand::purple.withAlpha (0.5f).interpolatedWith (juce::Colours::cyan, 0.4f);

    drawCurve (g, plotBounds, RoomCorrectionProcessor::getLeftCurve(), leftColour);
    drawCurve (g, plotBounds, RoomCorrectionProcessor::getRightCurve(), rightColour);

    // Legend
    float legendY = bounds.getY() + 6.f;
    float legendX = plotBounds.getRight() - 80.f;
    g.setFont (getBodyFont (10.f));

    g.setColour (leftColour);
    g.fillRoundedRectangle (legendX, legendY, 12.f, 3.f, 1.f);
    g.setColour (Brand::textMuted);
    g.drawText ("L", static_cast<int> (legendX + 16), static_cast<int> (legendY - 4), 10, 12,
                juce::Justification::centredLeft);

    g.setColour (rightColour);
    g.fillRoundedRectangle (legendX + 30.f, legendY, 12.f, 3.f, 1.f);
    g.setColour (Brand::textMuted);
    g.drawText ("R", static_cast<int> (legendX + 48), static_cast<int> (legendY - 4), 10, 12,
                juce::Justification::centredLeft);
}

void RoomCorrectionEditor::drawCurve (juce::Graphics& g, juce::Rectangle<float> bounds,
                                      const std::vector<RoomCorrectionProcessor::EQPoint>& points,
                                      juce::Colour colour)
{
    float minFreq = 20.f, maxFreq = 20000.f;
    float logMin = std::log10 (minFreq), logMax = std::log10 (maxFreq);
    float logRange = logMax - logMin;
    float minDB = -19.f, maxDB = 19.f;
    float dbRange = maxDB - minDB;

    juce::Path path;
    bool first = true;

    for (float x = 0.f; x <= bounds.getWidth(); x += 1.f)
    {
        float normX = x / bounds.getWidth();
        float freq = std::pow (10.f, logMin + normX * logRange);
        float db = RoomCorrectionProcessor::interpolateEQ (points, freq);
        float normY = 1.f - (db - minDB) / dbRange;
        float y = bounds.getY() + normY * bounds.getHeight();

        if (first) { path.startNewSubPath (bounds.getX() + x, y); first = false; }
        else       path.lineTo (bounds.getX() + x, y);
    }

    g.setColour (colour);
    g.strokePath (path, juce::PathStrokeType (2.f));

    // Subtle fill below curve
    juce::Path fillPath (path);
    fillPath.lineTo (bounds.getRight(), bounds.getBottom());
    fillPath.lineTo (bounds.getX(), bounds.getBottom());
    fillPath.closeSubPath();
    g.setColour (colour.withAlpha (0.06f));
    g.fillPath (fillPath);
}
