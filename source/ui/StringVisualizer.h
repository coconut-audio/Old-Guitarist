#pragma once

#include <JuceHeader.h>
#include "../Processor.h"

class StringVisualizer final : public Component
{
public:
    explicit StringVisualizer(Processor&);
    void paint(Graphics& g) override;

private:
    Processor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StringVisualizer)
};
