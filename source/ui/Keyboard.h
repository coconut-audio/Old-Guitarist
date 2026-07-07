#pragma once

#include <JuceHeader.h>
#include "../Processor.h"

class Keyboard final : public Component
{
public:
    explicit Keyboard(Processor&);
    void paint(Graphics& g) override;
    void resized() override;

    void mouseDown(const MouseEvent& event) override;
    void mouseDrag(const MouseEvent& event) override;
    void mouseUp(const MouseEvent& event) override;

private:
    Processor& processor;

    int noteAtPosition(int x, int y) const;
    int lastNote = -1;

    static constexpr int startNote = 36;
    static constexpr int endNote   = 96;

    static bool isBlack(int note) {
        const int pc = note % 12;
        return pc == 1 || pc == 3 || pc == 6 || pc == 8 || pc == 10;
    }

    static int whiteKeyIndex(int note) {
        int idx = 0;
        for (int n = startNote; n < note; ++n)
            if (! isBlack(n)) ++idx;
        return idx;
    }

    int numWhiteKeys = 0;
    float whiteKeyWidth = 0.0f;

    float blackKeyX(int note) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Keyboard)
};
