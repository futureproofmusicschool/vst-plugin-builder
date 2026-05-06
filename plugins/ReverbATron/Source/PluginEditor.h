#pragma once

#include "PluginProcessor.h"

// ─── Futureproof LookAndFeel ─────────────────────────────────────────────────

class FutureproofLookAndFeel : public juce::LookAndFeel_V4
{
public:
    FutureproofLookAndFeel();

    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider&) override;

    void drawToggleButton(juce::Graphics&, juce::ToggleButton&,
                          bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override;

    void drawLabel(juce::Graphics&, juce::Label&) override;
};

// ─── Pitch Display Component ─────────────────────────────────────────────────

class PitchDisplay : public juce::Component, public juce::Timer
{
public:
    PitchDisplay(ReverbATronProcessor& p);
    void paint(juce::Graphics&) override;
    void timerCallback() override;

private:
    ReverbATronProcessor& processor;
    float displayFreq = 0.0f;
    float displayConfidence = 0.0f;
    int displayMidiNote = -1;

    static juce::String midiNoteToName(int midiNote);
    static float midiNoteToCents(float freq, int midiNote);
};

// ─── Main Editor ─────────────────────────────────────────────────────────────

class ReverbATronEditor : public juce::AudioProcessorEditor
{
public:
    explicit ReverbATronEditor(ReverbATronProcessor&);
    ~ReverbATronEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    ReverbATronProcessor& processorRef;
    FutureproofLookAndFeel lookAndFeel;

    // Helper to create and configure a rotary knob
    struct KnobWithLabel
    {
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };

    // Core knobs
    KnobWithLabel mixKnob, decayKnob, sizeKnob, preDelayKnob;

    // Harmonic Resonance knobs
    KnobWithLabel resAmountKnob, resHarmonicsKnob, resQKnob, resTrackingKnob, resConfidenceKnob;

    // Frequency Sculpt knobs
    KnobWithLabel sculptLowCutKnob, sculptHighCutKnob, sculptMidFreqKnob, sculptMidGainKnob, sculptMidQKnob;

    // Flutter knobs
    KnobWithLabel flutterRateKnob, flutterDepthKnob;

    // Output knobs
    KnobWithLabel stereoWidthKnob, outputLevelKnob;

    // Flutter shape selector
    juce::ComboBox flutterShapeBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> flutterShapeAttachment;

    // Beat sync toggles
    juce::ToggleButton syncEnableToggle { "Sync" };
    juce::ToggleButton syncFlutterToggle { "Sync Flut" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> syncEnableAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> syncFlutterAttachment;

    // Sync division selector
    juce::ComboBox syncDivisionBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> syncDivisionAttachment;

    // Pitch display
    PitchDisplay pitchDisplay;

    // Setup helpers
    void setupKnob(KnobWithLabel& knob, const juce::String& paramId, const juce::String& labelText);
    void drawSectionPanel(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbATronEditor)
};
