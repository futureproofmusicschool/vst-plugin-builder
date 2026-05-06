#include "PluginEditor.h"

UnderwaterEditor::UnderwaterEditor(UnderwaterProcessor& p)
    : GenericAudioProcessorEditor(p)
{
    setSize(400, 300);
}
