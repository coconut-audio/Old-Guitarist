#include "Keyboard.h"

Keyboard::Keyboard(Processor& processorRef)
    : processor(processorRef)
{
    setWantsKeyboardFocus(true);
}

int Keyboard::noteAtPosition(int x, int y) const
{
    if (whiteKeyWidth <= 0.0f)
        return -1;

    const float mouseX = static_cast<float>(x);
    const float mouseY = static_cast<float>(y);
    const float w  = static_cast<float>(getWidth());
    const float h  = static_cast<float>(getHeight());

    if (mouseX < 0.0f || mouseX >= w)
        return -1;

    const float blackKeyWidth = whiteKeyWidth * 0.58f;
    const float blackKeyHeight = h * 0.62f;

    for (int note = endNote; note >= startNote; --note)
    {
        if (! isBlack(note))
            continue;

        const float keyCenterX = blackKeyX(note);
        const float blackKeyLeft = keyCenterX - blackKeyWidth * 0.5f;

        if (mouseX >= blackKeyLeft && mouseX < blackKeyLeft + blackKeyWidth && mouseY < blackKeyHeight)
            return note;
    }

    for (int note = startNote; note <= endNote; ++note)
    {
        if (isBlack(note))
            continue;

        const float keyX = static_cast<float>(whiteKeyIndex(note)) * whiteKeyWidth;
        if (mouseX >= keyX && mouseX < keyX + whiteKeyWidth)
            return note;
    }

    return -1;
}

void Keyboard::mouseDown(const MouseEvent& event)
{
    lastNote = noteAtPosition(event.getPosition().x, event.getPosition().y);
    if (lastNote >= 0)
        processor.synth.noteOn(1, lastNote, 0.8f);
}

void Keyboard::mouseDrag(const MouseEvent& event)
{
    const int note = noteAtPosition(event.getPosition().x, event.getPosition().y);
    if (note == lastNote)
        return;

    if (lastNote >= 0)
        processor.synth.noteOff(1, lastNote, 0.0f, true);

    if (note >= 0)
        processor.synth.noteOn(1, note, 0.8f);

    lastNote = note;
}

void Keyboard::mouseUp(const MouseEvent& /*event*/)
{
    if (lastNote >= 0)
    {
        processor.synth.noteOff(1, lastNote, 0.0f, true);
        lastNote = -1;
    }
}

float Keyboard::blackKeyX(int note) const
{
    return static_cast<float>(whiteKeyIndex(note)) * whiteKeyWidth;
}

void Keyboard::resized()
{
    numWhiteKeys = 0;
    for (int n = startNote; n <= endNote; ++n)
        if (! isBlack(n)) ++numWhiteKeys;
    whiteKeyWidth = numWhiteKeys > 0
        ? static_cast<float>(getWidth()) / static_cast<float>(numWhiteKeys)
        : 0.0f;
}

void Keyboard::paint(Graphics& g)
{
    const float h = static_cast<float>(getHeight());
    const float blackKeyWidth = whiteKeyWidth * 0.58f;
    const float blackKeyHeight = h * 0.62f;
    const float pressOffset = 2.0f;

    for (int note = startNote; note <= endNote; ++note)
    {
        if (isBlack(note))
            continue;

        const float x = static_cast<float>(whiteKeyIndex(note)) * whiteKeyWidth;
        const bool pressed = (lastNote == note);
        const float top = pressed ? pressOffset : 0.0f;
        const float keyH = h - top;

        g.setColour(pressed ? Colour(220, 220, 220) : Colours::white);
        g.fillRect(x, top, whiteKeyWidth - 1.0f, keyH);

        g.setColour(Colour(160, 160, 160));
        g.drawLine(x + whiteKeyWidth - 1.0f, top,
                    x + whiteKeyWidth - 1.0f, h, 1.0f);
    }

    for (int note = startNote; note <= endNote; ++note)
    {
        if (! isBlack(note))
            continue;

        const float keyCenterX = blackKeyX(note);
        const float blackKeyLeft = keyCenterX - blackKeyWidth * 0.5f;
        const bool pressed = (lastNote == note);
        const float top = pressed ? pressOffset : 0.0f;
        const float keyH = blackKeyHeight - top;

        g.setColour(pressed ? Colour(50, 50, 50) : Colours::black);
        g.fillRect(blackKeyLeft, top, blackKeyWidth, keyH);

        g.setColour(Colour(80, 80, 80));
        g.drawLine(blackKeyLeft, top + keyH - 1.0f, blackKeyLeft + blackKeyWidth, top + keyH - 1.0f, 1.0f);
    }
}
