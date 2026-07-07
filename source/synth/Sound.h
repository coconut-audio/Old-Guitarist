#pragma once

#include <JuceHeader.h>

class Sound final : public SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};
