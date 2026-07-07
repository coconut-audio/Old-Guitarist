#include "Voice.h"

bool Voice::canPlaySound(SynthesiserSound* sound)
{
    return dynamic_cast<Sound*>(sound) != nullptr;
}

void Voice::startNote(int midiNoteNumber, float /*velocity*/,
                      SynthesiserSound* /*sound*/,
                      int /*currentPitchWheelPosition*/)
{
    activeNote = midiNoteNumber;
    tensionDecay = 15.0f;

    params.sampleRate = static_cast<int>(sampleRate);
    params.computeDerived(stringIndex);

    stringLength = params.computeLengthForFrequency(
        MidiMessage::getMidiNoteInHertz(midiNoteNumber));

    if (! params.isStable(stringLength))
    {
        DBG("WARNING: CFL violation — Courant number = "
            << params.computeDx(stringLength) / (params.dt * params.waveSpeed));
    }

    solver.reset(params, stringLength);
}

void Voice::stopNote(float /*velocity*/, bool /*allowTailOff*/)
{
    activeNote = -1;
    clearCurrentNote();
}

void Voice::pitchWheelMoved(int /*newPitchWheelValue*/) {}
void Voice::controllerMoved(int /*controllerNumber*/, int /*newControllerValue*/) {}

void Voice::renderNextBlock(AudioBuffer<float>& outputBuffer,
                            int startSample, int numSamples)
{
    if (solver.getNumNodes() == 0)
        return;

    for (int sample = startSample; sample < startSample + numSamples; ++sample)
    {
        solver.advance(params, tensionDecay);

        const float output = solver.sample(solver.getNumNodes() / 8);

        for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
            outputBuffer.addSample(ch, sample,
                0.15f * output / params.yElongation);
    }
}

const float* Voice::getStringState() const
{
    return solver.getState();
}

int Voice::getNumNodes() const
{
    return solver.getNumNodes();
}
