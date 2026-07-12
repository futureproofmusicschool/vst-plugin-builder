#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

// Stage 1 editor: GenericAudioProcessorEditor exposes every APVTS parameter
// so we can confirm the parameter layout without writing UI code yet.
// Stage 3 will replace this with a custom AetherweaveEditor that owns the
// Grain Constellation and stage strip.
class AetherweaveEditor : public juce::GenericAudioProcessorEditor
{
public:
    explicit AetherweaveEditor(AetherweaveProcessor& p);
    ~AetherweaveEditor() override = default;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AetherweaveEditor)
};
