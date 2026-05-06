#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class UnderwaterEditor : public juce::GenericAudioProcessorEditor
{
public:
    explicit UnderwaterEditor(UnderwaterProcessor& p);
    ~UnderwaterEditor() override = default;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UnderwaterEditor)
};
