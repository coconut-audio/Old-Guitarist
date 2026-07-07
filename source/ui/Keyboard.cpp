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

    const float fx = static_cast<float>(x);
    const float fy = static_cast<float>(y);
    const float w  = static_cast<float>(getWidth());
    const float h  = static_cast<float>(getHeight());

    if (fx < 0.0f || fx >= w)
        return -1;

    const float bkw = whiteKeyWidth * 0.58f;
    const float bkh = h * 0.62f;

    for (int note = endNote; note >= startNote; --note)
    {
        if (! isBlack(note))
            continue;

        const float cx = blackKeyX(note);
        const float bx = cx - bkw * 0.5f;

        if (fx >= bx && fx < bx + bkw && fy < bkh)
            return note;
    }

    for (int note = startNote; note <= endNote; ++note)
    {
        if (isBlack(note))
            continue;

        const float kx = static_cast<float>(whiteKeyIndex(note)) * whiteKeyWidth;
        if (fx >= kx && fx < kx + whiteKeyWidth)
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
    const float bkw = whiteKeyWidth * 0.58f;
    const float bkh = h * 0.62f;
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

        const float cx = blackKeyX(note);
        const float bx = cx - bkw * 0.5f;
        const bool pressed = (lastNote == note);
        const float top = pressed ? pressOffset : 0.0f;
        const float keyH = bkh - top;

        g.setColour(pressed ? Colour(50, 50, 50) : Colours::black);
        g.fillRect(bx, top, bkw, keyH);

        g.setColour(Colour(80, 80, 80));
        g.drawLine(bx, top + keyH - 1.0f, bx + bkw, top + keyH - 1.0f, 1.0f);
    }
}
