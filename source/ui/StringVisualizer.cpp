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

    for (int fret = 0; fret <= numFrets; ++fret)
    {
        const float x = boardLeft + fretX(fret, boardWidth);
        g.setColour(Colour(180, 170, 150));
        g.drawLine(x, boardTop, x, boardBottom, fret == 0 ? 3.0f : 1.5f);
    }

    for (int stringIndex = 0; stringIndex < Processor::numStrings; ++stringIndex)
    {
        auto* voice = processor.voices[stringIndex];
        if (voice == nullptr || voice->getNumNodes() == 0)
            continue;

        const int midiNote = voice->getActiveNote();
        if (midiNote < 0)
            continue;

        const int fretString = processor.getStringForNote(midiNote);
        if (fretString < 0 || fretString != stringIndex)
            continue;

        const int fret = midiNote - Processor::openStringNotes[fretString];
        const float x0 = boardLeft + fretX(fret, boardWidth);
        const float x1 = boardLeft + fretX(fret + 1, boardWidth);

        g.setColour(Colour(80, 80, 80).withAlpha(0.3f));
        g.fillRect(x0, boardTop, x1 - x0, boardHeight);
    }

    for (int stringIndex = 0; stringIndex < 6; ++stringIndex)
    {
        const float y = boardTop + (static_cast<float>(stringIndex) + 0.5f) * (boardHeight / 6.0f);
        const float thickness = 1.0f + static_cast<float>(5 - stringIndex) * 0.35f;

        const int numNodes = processor.voices[stringIndex]->getNumNodes();
        const float* state = processor.voices[stringIndex]->getStringState();

        if (numNodes > 1)
        {
            const float displacementScale = 300.0f;
            for (int nodeIndex = 0; nodeIndex < numNodes - 1; ++nodeIndex)
            {
                const float t0 = static_cast<float>(nodeIndex)     / static_cast<float>(numNodes - 1);
                const float t1 = static_cast<float>(nodeIndex + 1) / static_cast<float>(numNodes - 1);
                const float x0 = boardLeft + t0 * boardWidth;
                const float x1 = boardLeft + t1 * boardWidth;
                const float y0 = y - state[nodeIndex]     * displacementScale;
                const float y1 = y - state[nodeIndex + 1] * displacementScale;

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

    for (int fret = 1; fret <= numFrets; ++fret)
    {
        const float x = boardLeft + fretX(fret, boardWidth);
        g.setColour(Colour(120, 120, 120));
        g.setFont(10.0f);
        g.drawText(String(fret), x - 5.0f, boardBottom + 2.0f, 12.0f, 12.0f, Justification::centred);
    }
}
