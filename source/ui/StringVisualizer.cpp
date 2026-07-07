#include "StringVisualizer.h"
#include "../synth/Voice.h"

static constexpr int numFrets = 21;

static float fretX(int fret, float boardWidth)
{
    if (fret == 0)
        return 0.0f;
    const float scale = 1.0f / (1.0f - std::pow(2.0f, -static_cast<float>(numFrets) / 12.0f));
    return boardWidth * scale * (1.0f - std::pow(2.0f, -static_cast<float>(fret) / 12.0f));
}

StringVisualizer::StringVisualizer(Processor& processorRef)
    : processor(processorRef)
{
}

void StringVisualizer::paint(Graphics& g)
{
    g.setColour(Colours::black);
    g.fillAll();

    const float w = static_cast<float>(getWidth());
    const float h = static_cast<float>(getHeight());

    const float boardLeft   = 0.0f;
    const float boardWidth  = w;
    const float boardTop    = 10.0f;
    const float boardBottom = h - 20.0f;
    const float boardHeight = boardBottom - boardTop;

    g.setColour(Colours::black);
    g.fillRect(boardLeft, boardTop, boardWidth, boardHeight);

    for (int f = 0; f <= numFrets; ++f)
    {
        const float x = boardLeft + fretX(f, boardWidth);
        g.setColour(Colour(180, 170, 150));
        g.drawLine(x, boardTop, x, boardBottom, f == 0 ? 3.0f : 1.5f);
    }

    for (int i = 0; i < Processor::numStrings; ++i)
    {
        auto* voice = processor.voices[i];
        if (voice == nullptr || voice->getNumNodes() == 0)
            continue;

        const int midiNote = voice->getActiveNote();
        if (midiNote < 0)
            continue;

        const int s = processor.getStringForNote(midiNote);
        if (s < 0 || s != i)
            continue;

        const int fret = midiNote - Processor::openStringNotes[s];
        const float x0 = boardLeft + fretX(fret, boardWidth);
        const float x1 = boardLeft + fretX(fret + 1, boardWidth);

        g.setColour(Colour(80, 80, 80).withAlpha(0.3f));
        g.fillRect(x0, boardTop, x1 - x0, boardHeight);
    }

    for (int s = 0; s < 6; ++s)
    {
        const float y = boardTop + (static_cast<float>(s) + 0.5f) * (boardHeight / 6.0f);
        const float thickness = 1.0f + static_cast<float>(5 - s) * 0.35f;

        const int numNodes = processor.voices[s]->getNumNodes();
        const float* state = processor.voices[s]->getStringState();

        if (numNodes > 1)
        {
            const float dy = 300.0f;
            for (int n = 0; n < numNodes - 1; ++n)
            {
                const float t0 = static_cast<float>(n)     / static_cast<float>(numNodes - 1);
                const float t1 = static_cast<float>(n + 1) / static_cast<float>(numNodes - 1);
                const float x0 = boardLeft + t0 * boardWidth;
                const float x1 = boardLeft + t1 * boardWidth;
                const float y0 = y - state[n]     * dy;
                const float y1 = y - state[n + 1] * dy;

                g.setColour(Colours::white);
                g.drawLine(x0, y0, x1, y1, thickness);
            }
        }
        else
        {
            g.setColour(Colours::white);
            g.drawLine(boardLeft, y, boardLeft + boardWidth, y, thickness);
        }
    }

    for (int f = 1; f <= numFrets; ++f)
    {
        const float x = boardLeft + fretX(f, boardWidth);
        g.setColour(Colour(120, 120, 120));
        g.setFont(10.0f);
        g.drawText(String(f), x - 5.0f, boardBottom + 2.0f, 12.0f, 12.0f, Justification::centred);
    }
}
