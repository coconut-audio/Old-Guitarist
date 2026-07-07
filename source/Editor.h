#pragma once

#include <JuceHeader.h>
#include "Processor.h"
#include "ui/StringVisualizer.h"
#include "ui/Keyboard.h"

class Editor final : public AudioProcessorEditor, public Timer
{
public:
    explicit Editor(Processor&);
    ~Editor() override;

    void paint(Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    Processor& processor;
    StringVisualizer visualizer;
    Keyboard keyboard;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Editor)
};
