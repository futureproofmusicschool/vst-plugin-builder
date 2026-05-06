#pragma once

#include "PluginProcessor.h"

class RoomCorrectionEditor : public juce::AudioProcessorEditor
{
public:
    explicit RoomCorrectionEditor (RoomCorrectionProcessor&);
    ~RoomCorrectionEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    RoomCorrectionProcessor& processorRef;

    // Brand colours
    struct Brand
    {
        static inline const juce::Colour purple      { 0xa3, 0x73, 0xf8 };
        static inline const juce::Colour deepPurple  { 0x5d, 0x2d, 0xa8 };
        static inline const juce::Colour background  { 0x00, 0x00, 0x00 };
        static inline const juce::Colour panel       { 0x1c, 0x14, 0x29 };
        static inline const juce::Colour textPrimary { 0xff, 0xff, 0xff };
        static inline const juce::Colour textMuted   = juce::Colours::white.withAlpha (0.6f);
        static inline const juce::Colour border      = juce::Colours::white.withAlpha (0.15f);
        static inline const juce::Colour glassFill   = juce::Colours::white.withAlpha (0.05f);
    };

    // LookAndFeel
    class LookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        LookAndFeel();
    };
    LookAndFeel lnf;

    // Controls
    juce::TextButton bypassButton { "BYPASS" };
    juce::Slider gainSlider;
    juce::Label gainLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    // Fonts
    juce::Font getTitleFont (float height);
    juce::Font getHeadingFont (float height);
    juce::Font getBodyFont (float height);

    // Painting helpers
    void drawHeader (juce::Graphics&, juce::Rectangle<int> area);
    void drawCurveDisplay (juce::Graphics&, juce::Rectangle<int> area);
    void drawCurve (juce::Graphics&, juce::Rectangle<float> bounds,
                    const std::vector<RoomCorrectionProcessor::EQPoint>& points,
                    juce::Colour colour);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RoomCorrectionEditor)
};
