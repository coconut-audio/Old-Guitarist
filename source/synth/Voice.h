#pragma once

#include <JuceHeader.h>
#include "Sound.h"
#include "../model/WaveSolver.h"
#include "../model/StringParams.h"

class Voice final : public SynthesiserVoice
{
public:
    void setStringIndex(int index) { stringIndex = index; }
    int getStringIndex() const { return stringIndex; }
    void setSampleRate(double sr) { sampleRate = sr; }

    bool canPlaySound(SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity,
                   SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(AudioBuffer<float>& outputBuffer,
                         int startSample, int numSamples) override;
    using SynthesiserVoice::renderNextBlock;

    const float* getStringState() const;
    int getNumNodes() const;
    bool isStringActive() const { return activeNote >= 0; }
    int getActiveNote() const { return activeNote; }

private:
    WaveSolver  solver;
    StringParams params;
    int stringIndex = 0;
    int activeNote = -1;
    double sampleRate = 44100.0;
    float stringLength = 0.0f;
    float tensionDecay = 0.0f;
};
