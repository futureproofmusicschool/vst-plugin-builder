#include "PluginEditor.h"

AetherweaveEditor::AetherweaveEditor(AetherweaveProcessor& p)
    : GenericAudioProcessorEditor(p)
{
    // Target window from ui-mockup-v1.md; Generic editor will scroll if needed.
    setSize(1000, 680);
}
