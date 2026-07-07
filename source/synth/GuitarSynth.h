#pragma once

#include <JuceHeader.h>
#include "Voice.h"
#include "../Processor.h"

class GuitarSynth : public Synthesiser
{
public:
    void routeNoteOn(int midiChannel, int midiNote, float velocity,
                     SynthesiserVoice* targetVoice, SynthesiserSound* sound)
    {
        const ScopedLock sl(lock);

        for (auto* voice : voices)
            if (voice->getCurrentlyPlayingNote() == midiNote && voice->isPlayingChannel(midiChannel))
                stopVoice(voice, 1.0f, true);

        startVoice(targetVoice, sound, midiChannel, midiNote, velocity);
    }
};
