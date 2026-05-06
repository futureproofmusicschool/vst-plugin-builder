#include "PluginEditor.h"
#include "BinaryData.h"

// ─── Futureproof Brand Fonts ────────────────────────────────────────────────

namespace FutureproofFonts
{
    juce::Typeface::Ptr getPanchang()
    {
        static auto typeface = juce::Typeface::createSystemTypefaceFor(
            BinaryData::PanchangMedium_otf, BinaryData::PanchangMedium_otfSize);
        return typeface;
    }

    juce::Typeface::Ptr getSatoshi()
    {
        static auto typeface = juce::Typeface::createSystemTypefaceFor(
            BinaryData::SatoshiRegular_otf, BinaryData::SatoshiRegular_otfSize);
        return typeface;
    }

    juce::Typeface::Ptr getSatoshiBold()
    {
        static auto typeface = juce::Typeface::createSystemTypefaceFor(
            BinaryData::SatoshiBold_otf, BinaryData::SatoshiBold_otfSize);
        return typeface;
    }

    juce::Font title(float height)  { return juce::Font(getPanchang()).withHeight(height); }
    juce::Font heading(float height) { return juce::Font(getSatoshiBold()).withHeight(height); }
    juce::Font body(float height)    { return juce::Font(getSatoshi()).withHeight(height); }
    juce::Font mono(float height)    { return juce::Font(juce::Font::getDefaultMonospacedFontName(), height, juce::Font::plain); }
}

// ─── Futureproof Brand Colours ──────────────────────────────────────────────

namespace FutureproofBrand
{
    const juce::Colour purple       { 0xa3, 0x73, 0xf8 };
    const juce::Colour deepPurple   { 0x5d, 0x2d, 0xa8 };
    const juce::Colour background   { 0x00, 0x00, 0x00 };
    const juce::Colour panel        { 0x1c, 0x14, 0x29 };
    const juce::Colour textPrimary  { 0xff, 0xff, 0xff };
    const juce::Colour textMuted    = juce::Colours::white.withAlpha(0.6f);
    const juce::Colour border       = juce::Colours::white.withAlpha(0.15f);
    const juce::Colour glassFill    = juce::Colours::white.withAlpha(0.05f);
}

// ═══════════════════════════════════════════════════════════════════════════════
// FutureproofLookAndFeel
// ═══════════════════════════════════════════════════════════════════════════════

FutureproofLookAndFeel::FutureproofLookAndFeel()
{
    setColour(juce::Slider::rotarySliderFillColourId,    FutureproofBrand::purple);
    setColour(juce::Slider::rotarySliderOutlineColourId, FutureproofBrand::panel);
    setColour(juce::Slider::thumbColourId,               FutureproofBrand::textPrimary);
    setColour(juce::Slider::textBoxTextColourId,         FutureproofBrand::textPrimary);
    setColour(juce::Slider::textBoxOutlineColourId,      juce::Colours::transparentBlack);
    setColour(juce::Slider::textBoxBackgroundColourId,   FutureproofBrand::glassFill);

    setColour(juce::Label::textColourId,                 FutureproofBrand::textPrimary);

    setColour(juce::ComboBox::backgroundColourId,        FutureproofBrand::panel);
    setColour(juce::ComboBox::outlineColourId,           FutureproofBrand::border);
    setColour(juce::ComboBox::textColourId,              FutureproofBrand::textPrimary);
    setColour(juce::ComboBox::arrowColourId,             FutureproofBrand::purple);

    setColour(juce::TextButton::buttonColourId,          FutureproofBrand::panel);
    setColour(juce::TextButton::buttonOnColourId,        FutureproofBrand::purple);
    setColour(juce::TextButton::textColourOnId,          FutureproofBrand::textPrimary);
    setColour(juce::TextButton::textColourOffId,         FutureproofBrand::textPrimary);

    setColour(juce::PopupMenu::backgroundColourId,       FutureproofBrand::panel);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, FutureproofBrand::purple);
    setColour(juce::PopupMenu::textColourId,             FutureproofBrand::textPrimary);
}

void FutureproofLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                               float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                               juce::Slider& slider)
{
    juce::ignoreUnused(slider);
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    auto knobRadius = radius * 0.75f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Track arc (background)
    juce::Path trackBg;
    trackBg.addCentredArc(centreX, centreY, radius * 0.88f, radius * 0.88f,
                          0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(FutureproofBrand::panel);
    g.strokePath(trackBg, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved,
                                                juce::PathStrokeType::rounded));

    // Track arc (filled with purple accent)
    juce::Path trackFill;
    trackFill.addCentredArc(centreX, centreY, radius * 0.88f, radius * 0.88f,
                            0.0f, rotaryStartAngle, angle, true);
    g.setColour(FutureproofBrand::purple);
    g.strokePath(trackFill, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved,
                                                  juce::PathStrokeType::rounded));

    // Knob face with gradient (dark purple tones)
    juce::ColourGradient knobGrad(FutureproofBrand::panel.brighter(0.15f), centreX, centreY - knobRadius,
                                  FutureproofBrand::panel.darker(0.3f), centreX, centreY + knobRadius, false);
    g.setGradientFill(knobGrad);
    g.fillEllipse(centreX - knobRadius, centreY - knobRadius, knobRadius * 2, knobRadius * 2);

    // Knob edge
    g.setColour(FutureproofBrand::border);
    g.drawEllipse(centreX - knobRadius, centreY - knobRadius, knobRadius * 2, knobRadius * 2, 1.0f);

    // Pointer line (white)
    juce::Path pointer;
    auto pointerLength = knobRadius * 0.65f;
    auto pointerThickness = 2.0f;
    pointer.addRectangle(-pointerThickness * 0.5f, -knobRadius + 4.0f, pointerThickness, pointerLength);
    pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    g.setColour(FutureproofBrand::textPrimary);
    g.fillPath(pointer);

    // Center dot (small white)
    g.setColour(FutureproofBrand::textPrimary);
    g.fillEllipse(centreX - 2.0f, centreY - 2.0f, 4.0f, 4.0f);
}

void FutureproofLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                               bool shouldDrawButtonAsHighlighted,
                                               bool /*shouldDrawButtonAsDown*/)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(4.0f);
    auto isOn = button.getToggleState();

    // Toggle track
    auto toggleBounds = bounds.withSizeKeepingCentre(40.0f, 20.0f);
    g.setColour(isOn ? FutureproofBrand::purple : FutureproofBrand::panel);
    g.fillRoundedRectangle(toggleBounds, 10.0f);

    // Toggle border
    g.setColour(FutureproofBrand::border);
    g.drawRoundedRectangle(toggleBounds, 10.0f, 1.0f);

    // Toggle knob
    float knobX = isOn ? toggleBounds.getRight() - 18.0f : toggleBounds.getX() + 2.0f;
    g.setColour(shouldDrawButtonAsHighlighted ? FutureproofBrand::textPrimary
                                               : FutureproofBrand::textPrimary.darker(0.1f));
    g.fillEllipse(knobX, toggleBounds.getY() + 2.0f, 16.0f, 16.0f);

    // Label text
    g.setColour(FutureproofBrand::textPrimary);
    g.setFont(FutureproofFonts::body(11.0f));
    g.drawText(button.getButtonText(),
               button.getLocalBounds().removeFromBottom(14),
               juce::Justification::centred);
}

void FutureproofLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.setColour(FutureproofBrand::textPrimary.withAlpha(0.85f));
    g.setFont(FutureproofFonts::body(11.0f));
    g.drawFittedText(label.getText(), label.getLocalBounds(),
                     juce::Justification::centred, 1);
}

// ═══════════════════════════════════════════════════════════════════════════════
// PitchDisplay
// ═══════════════════════════════════════════════════════════════════════════════

PitchDisplay::PitchDisplay(ReverbATronProcessor& p) : processor(p)
{
    startTimerHz(30);
}

void PitchDisplay::timerCallback()
{
    displayFreq = processor.detectedFrequency.load();
    displayConfidence = processor.detectedConfidence.load();
    displayMidiNote = processor.detectedMidiNote.load();
    repaint();
}

juce::String PitchDisplay::midiNoteToName(int midiNote)
{
    if (midiNote < 0) return "---";
    static const char* names[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    int note = midiNote % 12;
    int octave = (midiNote / 12) - 1;
    return juce::String(names[note]) + juce::String(octave);
}

float PitchDisplay::midiNoteToCents(float freq, int midiNote)
{
    if (freq <= 0.0f || midiNote < 0) return 0.0f;
    float expectedFreq = 440.0f * std::pow(2.0f, (static_cast<float>(midiNote) - 69.0f) / 12.0f);
    return 1200.0f * std::log2(freq / expectedFreq);
}

void PitchDisplay::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);

    // Panel background
    g.setColour(FutureproofBrand::panel);
    g.fillRoundedRectangle(bounds, 8.0f);
    g.setColour(FutureproofBrand::border);
    g.drawRoundedRectangle(bounds, 8.0f, 1.0f);

    bool active = displayConfidence > 0.1f && displayMidiNote >= 0;
    auto textColour = active ? FutureproofBrand::purple : FutureproofBrand::textMuted;

    // Note name (large)
    auto topArea = bounds.reduced(6.0f, 4.0f).removeFromTop(bounds.getHeight() * 0.45f);
    g.setColour(active ? FutureproofBrand::textPrimary : FutureproofBrand::textMuted);
    g.setFont(FutureproofFonts::mono(20.0f));
    juce::String noteName = active ? midiNoteToName(displayMidiNote) : "---";
    g.drawText(noteName, topArea.removeFromLeft(topArea.getWidth() * 0.5f),
               juce::Justification::centredLeft);

    // Frequency (Hz)
    g.setFont(FutureproofFonts::mono(12.0f));
    g.setColour(active ? FutureproofBrand::textMuted : FutureproofBrand::textMuted.withAlpha(0.4f));
    juce::String freqText = active ?
        juce::String(static_cast<int>(displayFreq)) + " Hz" : "--- Hz";
    g.drawText(freqText, topArea, juce::Justification::centredRight);

    // Tuning meter
    auto meterArea = bounds.reduced(8.0f, 2.0f).removeFromBottom(bounds.getHeight() * 0.35f);

    // Meter background
    g.setColour(FutureproofBrand::background);
    g.fillRoundedRectangle(meterArea, 2.0f);

    // Center line
    float centerX = meterArea.getCentreX();
    g.setColour(FutureproofBrand::border);
    g.drawVerticalLine(static_cast<int>(centerX), meterArea.getY(), meterArea.getBottom());

    // Tick marks
    for (int i = -2; i <= 2; ++i)
    {
        float tickX = centerX + static_cast<float>(i) * (meterArea.getWidth() * 0.2f);
        g.drawVerticalLine(static_cast<int>(tickX),
                           meterArea.getBottom() - 4.0f, meterArea.getBottom());
    }

    // Needle (purple when active)
    if (active)
    {
        float cents = midiNoteToCents(displayFreq, displayMidiNote);
        float normalizedCents = juce::jlimit(-50.0f, 50.0f, cents) / 50.0f;
        float needleX = centerX + normalizedCents * (meterArea.getWidth() * 0.4f);

        g.setColour(FutureproofBrand::purple);
        g.fillRect(needleX - 1.5f, meterArea.getY() + 1.0f, 3.0f, meterArea.getHeight() - 2.0f);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// ReverbATronEditor
// ═══════════════════════════════════════════════════════════════════════════════

void ReverbATronEditor::setupKnob(KnobWithLabel& knob, const juce::String& paramId,
                                   const juce::String& labelText)
{
    knob.slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    knob.slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 14);
    knob.slider.setLookAndFeel(&lookAndFeel);
    knob.slider.setColour(juce::Slider::textBoxTextColourId, FutureproofBrand::textPrimary);
    knob.slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    knob.slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(knob.slider);

    knob.label.setText(labelText, juce::dontSendNotification);
    knob.label.setJustificationType(juce::Justification::centred);
    knob.label.setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(knob.label);

    knob.attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, paramId, knob.slider);
}

void ReverbATronEditor::drawSectionPanel(juce::Graphics& g, juce::Rectangle<int> bounds,
                                          const juce::String& title)
{
    auto boundsF = bounds.toFloat();

    // Panel background with glassmorphism fill
    g.setColour(FutureproofBrand::glassFill);
    g.fillRoundedRectangle(boundsF, 10.0f);

    // Subtle border
    g.setColour(FutureproofBrand::border);
    g.drawRoundedRectangle(boundsF, 10.0f, 1.0f);

    // Section title
    g.setColour(FutureproofBrand::textMuted);
    g.setFont(FutureproofFonts::heading(12.0f));
    g.drawText(title, bounds.removeFromTop(18).reduced(10, 0), juce::Justification::centredLeft);
}

ReverbATronEditor::ReverbATronEditor(ReverbATronProcessor& p)
    : AudioProcessorEditor(p), processorRef(p), pitchDisplay(p)
{
    setLookAndFeel(&lookAndFeel);
    setSize(750, 500);

    // Core knobs
    setupKnob(mixKnob,      "mix",        "Mix");
    setupKnob(decayKnob,    "decay_time", "Decay");
    setupKnob(sizeKnob,     "room_size",  "Size");
    setupKnob(preDelayKnob, "pre_delay",  "Pre-Dly");

    // Harmonic Resonance knobs
    setupKnob(resAmountKnob,     "res_amount",     "Amount");
    setupKnob(resHarmonicsKnob,  "res_harmonics",  "Harmonics");
    setupKnob(resQKnob,          "res_q",          "Q");
    setupKnob(resTrackingKnob,   "res_tracking",   "Track");
    setupKnob(resConfidenceKnob, "res_confidence",  "Confid");

    // Frequency Sculpt knobs
    setupKnob(sculptLowCutKnob,  "sculpt_low_cut",  "Lo Cut");
    setupKnob(sculptHighCutKnob, "sculpt_high_cut", "Hi Cut");
    setupKnob(sculptMidFreqKnob, "sculpt_mid_freq", "Mid Freq");
    setupKnob(sculptMidGainKnob, "sculpt_mid_gain", "Mid Gain");
    setupKnob(sculptMidQKnob,    "sculpt_mid_q",    "Mid Q");

    // Flutter knobs
    setupKnob(flutterRateKnob,  "flutter_rate",  "Rate");
    setupKnob(flutterDepthKnob, "flutter_depth", "Depth");

    // Flutter shape combo
    flutterShapeBox.addItem("Sine", 1);
    flutterShapeBox.addItem("Triangle", 2);
    flutterShapeBox.addItem("Random", 3);
    flutterShapeBox.addItem("Drunk", 4);
    flutterShapeBox.setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(flutterShapeBox);
    flutterShapeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processorRef.apvts, "flutter_shape", flutterShapeBox);

    // Output knobs
    setupKnob(stereoWidthKnob, "stereo_width",  "Width");
    setupKnob(outputLevelKnob, "output_level",  "Output");

    // Sync toggles
    syncEnableToggle.setLookAndFeel(&lookAndFeel);
    syncFlutterToggle.setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(syncEnableToggle);
    addAndMakeVisible(syncFlutterToggle);
    syncEnableAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.apvts, "sync_enable", syncEnableToggle);
    syncFlutterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.apvts, "sync_flutter_rate", syncFlutterToggle);

    // Sync division combo
    syncDivisionBox.addItem("1/16", 1);
    syncDivisionBox.addItem("1/8", 2);
    syncDivisionBox.addItem("1/4", 3);
    syncDivisionBox.addItem("1/2", 4);
    syncDivisionBox.addItem("1 Bar", 5);
    syncDivisionBox.addItem("2 Bars", 6);
    syncDivisionBox.addItem("4 Bars", 7);
    syncDivisionBox.setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(syncDivisionBox);
    syncDivisionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processorRef.apvts, "sync_division", syncDivisionBox);

    // Pitch display
    addAndMakeVisible(pitchDisplay);
}

ReverbATronEditor::~ReverbATronEditor()
{
    setLookAndFeel(nullptr);
}

void ReverbATronEditor::paint(juce::Graphics& g)
{
    // Main background with subtle radial gradient
    juce::ColourGradient bgGrad(FutureproofBrand::deepPurple.withAlpha(0.15f),
                                 static_cast<float>(getWidth()) * 0.5f,
                                 static_cast<float>(getHeight()) * 0.3f,
                                 FutureproofBrand::background, 0.0f, static_cast<float>(getHeight()), true);
    g.setGradientFill(bgGrad);
    g.fillRect(getLocalBounds());

    // Header bar (36px tall, glassmorphism)
    auto headerArea = getLocalBounds().removeFromTop(36);
    g.setColour(FutureproofBrand::glassFill);
    g.fillRect(headerArea);

    // "FUTUREPROOF" - left, muted, Panchang
    g.setColour(FutureproofBrand::textMuted);
    g.setFont(FutureproofFonts::title(13.0f));
    g.drawText("FUTUREPROOF", headerArea.reduced(16, 0), juce::Justification::centredLeft);

    // Plugin name - centre, white, Panchang
    g.setColour(FutureproofBrand::textPrimary);
    g.setFont(FutureproofFonts::title(15.0f));
    g.drawText("REVERB-A-TRON", headerArea, juce::Justification::centred);

    // Version - right, muted, Satoshi
    g.setColour(FutureproofBrand::textMuted);
    g.setFont(FutureproofFonts::body(11.0f));
    g.drawText("v1.0.0", headerArea.reduced(16, 0), juce::Justification::centredRight);

    // Separator line
    g.setColour(FutureproofBrand::border);
    g.drawHorizontalLine(36, 0.0f, static_cast<float>(getWidth()));

    // Section panels
    auto contentArea = getLocalBounds().reduced(16).withTrimmedTop(30);
    int rowHeight = (contentArea.getHeight() - 20) / 3;

    // Row 1
    auto row1 = contentArea.removeFromTop(rowHeight);
    auto mainPanel = row1.removeFromLeft(230);
    row1.removeFromLeft(10);
    auto resPanel = row1;
    drawSectionPanel(g, mainPanel, "MAIN");
    drawSectionPanel(g, resPanel, "HARMONIC RESONANCE");

    contentArea.removeFromTop(10);

    // Row 2
    auto row2 = contentArea.removeFromTop(rowHeight);
    auto sculptPanel = row2.removeFromLeft(380);
    row2.removeFromLeft(10);
    auto flutterPanel = row2;
    drawSectionPanel(g, sculptPanel, "FREQ SCULPT");
    drawSectionPanel(g, flutterPanel, "FLUTTER");

    contentArea.removeFromTop(10);

    // Row 3
    auto row3 = contentArea;
    auto syncPanel = row3.removeFromLeft(350);
    row3.removeFromLeft(10);
    auto outputPanel = row3;
    drawSectionPanel(g, syncPanel, "BEAT SYNC");
    drawSectionPanel(g, outputPanel, "OUTPUT");
}

void ReverbATronEditor::resized()
{
    auto contentArea = getLocalBounds().reduced(16).withTrimmedTop(30);
    int rowHeight = (contentArea.getHeight() - 20) / 3;
    int knobSize = 60;
    int labelH = 14;

    auto placeKnob = [knobSize, labelH](KnobWithLabel& knob, juce::Rectangle<int> area)
    {
        auto knobArea = area.removeFromTop(area.getHeight() - labelH);
        knob.slider.setBounds(knobArea.withSizeKeepingCentre(knobSize, knobSize));
        knob.label.setBounds(area);
    };

    // ─── Row 1: MAIN | HARMONIC RESONANCE ───
    {
        auto row1 = contentArea.removeFromTop(rowHeight);
        auto mainPanel = row1.removeFromLeft(230).reduced(8).withTrimmedTop(18);
        row1.removeFromLeft(10);
        auto resPanel = row1.reduced(8).withTrimmedTop(18);

        // Main: single row of 4 knobs
        int cW = mainPanel.getWidth() / 4;
        placeKnob(mixKnob,      { mainPanel.getX(),           mainPanel.getY(), cW, mainPanel.getHeight() });
        placeKnob(decayKnob,    { mainPanel.getX() + cW,      mainPanel.getY(), cW, mainPanel.getHeight() });
        placeKnob(sizeKnob,     { mainPanel.getX() + cW * 2,  mainPanel.getY(), cW, mainPanel.getHeight() });
        placeKnob(preDelayKnob, { mainPanel.getX() + cW * 3,  mainPanel.getY(), cW, mainPanel.getHeight() });

        // Resonance: 5 knobs on top, pitch display below
        auto knobRow = resPanel.removeFromTop(resPanel.getHeight() * 3 / 5);
        int rW = knobRow.getWidth() / 5;
        placeKnob(resAmountKnob,     { knobRow.getX(),            knobRow.getY(), rW, knobRow.getHeight() });
        placeKnob(resHarmonicsKnob,  { knobRow.getX() + rW,      knobRow.getY(), rW, knobRow.getHeight() });
        placeKnob(resQKnob,          { knobRow.getX() + rW * 2,  knobRow.getY(), rW, knobRow.getHeight() });
        placeKnob(resTrackingKnob,   { knobRow.getX() + rW * 3,  knobRow.getY(), rW, knobRow.getHeight() });
        placeKnob(resConfidenceKnob, { knobRow.getX() + rW * 4,  knobRow.getY(), rW, knobRow.getHeight() });

        pitchDisplay.setBounds(resPanel.withSizeKeepingCentre(180, resPanel.getHeight() - 4));
    }

    contentArea.removeFromTop(10);

    // ─── Row 2: FREQ SCULPT | FLUTTER ───
    {
        auto row2 = contentArea.removeFromTop(rowHeight);
        auto sculptPanel = row2.removeFromLeft(380).reduced(8).withTrimmedTop(18);
        row2.removeFromLeft(10);
        auto flutterPanel = row2.reduced(8).withTrimmedTop(18);

        // Sculpt: 5 knobs
        int sW = sculptPanel.getWidth() / 5;
        placeKnob(sculptLowCutKnob,  { sculptPanel.getX(),            sculptPanel.getY(), sW, sculptPanel.getHeight() });
        placeKnob(sculptMidFreqKnob, { sculptPanel.getX() + sW,      sculptPanel.getY(), sW, sculptPanel.getHeight() });
        placeKnob(sculptMidGainKnob, { sculptPanel.getX() + sW * 2,  sculptPanel.getY(), sW, sculptPanel.getHeight() });
        placeKnob(sculptMidQKnob,    { sculptPanel.getX() + sW * 3,  sculptPanel.getY(), sW, sculptPanel.getHeight() });
        placeKnob(sculptHighCutKnob, { sculptPanel.getX() + sW * 4,  sculptPanel.getY(), sW, sculptPanel.getHeight() });

        // Flutter: 2 knobs + combo
        int fW = flutterPanel.getWidth() / 3;
        placeKnob(flutterRateKnob,  { flutterPanel.getX(),       flutterPanel.getY(), fW, flutterPanel.getHeight() });
        placeKnob(flutterDepthKnob, { flutterPanel.getX() + fW,  flutterPanel.getY(), fW, flutterPanel.getHeight() });

        auto shapeArea = juce::Rectangle<int>(flutterPanel.getX() + fW * 2, flutterPanel.getY(),
                                              fW, flutterPanel.getHeight());
        flutterShapeBox.setBounds(shapeArea.withSizeKeepingCentre(70, 24));
    }

    contentArea.removeFromTop(10);

    // ─── Row 3: BEAT SYNC | OUTPUT ───
    {
        auto row3 = contentArea;
        auto syncPanel = row3.removeFromLeft(350).reduced(8).withTrimmedTop(18);
        row3.removeFromLeft(10);
        auto outputPanel = row3.reduced(8).withTrimmedTop(18);

        // Sync: toggle + combo + toggle
        int bW = syncPanel.getWidth() / 3;
        syncEnableToggle.setBounds(juce::Rectangle<int>(syncPanel.getX(), syncPanel.getY(),
                                   bW, syncPanel.getHeight()).withSizeKeepingCentre(60, 40));
        syncDivisionBox.setBounds(juce::Rectangle<int>(syncPanel.getX() + bW, syncPanel.getY(),
                                  bW, syncPanel.getHeight()).withSizeKeepingCentre(80, 24));
        syncFlutterToggle.setBounds(juce::Rectangle<int>(syncPanel.getX() + bW * 2, syncPanel.getY(),
                                    bW, syncPanel.getHeight()).withSizeKeepingCentre(60, 40));

        // Output: 2 knobs
        int oW = outputPanel.getWidth() / 2;
        placeKnob(stereoWidthKnob, { outputPanel.getX(),       outputPanel.getY(), oW, outputPanel.getHeight() });
        placeKnob(outputLevelKnob, { outputPanel.getX() + oW,  outputPanel.getY(), oW, outputPanel.getHeight() });
    }
}
