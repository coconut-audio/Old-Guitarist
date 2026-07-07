#include "Editor.h"

Editor::Editor(Processor& processorRef)
    : AudioProcessorEditor(&processorRef)
    , processor(processorRef)
    , visualizer(processor)
    , keyboard(processor)
{
    setSize(1060, 320);
    startTimerHz(30);
    addAndMakeVisible(visualizer);
    addAndMakeVisible(keyboard);
}

Editor::~Editor()
{
    stopTimer();
}

void Editor::paint(Graphics& g)
{
    g.setColour(Colours::black);
    g.fillAll();
}

void Editor::resized()
{
    auto bounds = getLocalBounds();
    const int keyboardHeight = 80;
    keyboard.setBounds(bounds.removeFromBottom(keyboardHeight));
    visualizer.setBounds(bounds);
}

void Editor::timerCallback()
{
    visualizer.repaint();
    keyboard.repaint();
}
