#pragma once

#include <JuceHeader.h>
#include "synth/Voice.h"
#include "synth/GuitarSynth.h"
#include "dsp/IRConvolution.h"

class Processor final : public AudioProcessor
{
public:
    static constexpr int numStrings = 6;
    static constexpr int openStringNotes[numStrings] = { 40, 45, 50, 55, 59, 64 };
    static constexpr int maxFret = 21;

    Processor();
    ~Processor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(AudioBuffer<float>&, MidiBuffer&) override;
    using AudioProcessor::processBlock;

    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const String& newName) override;

    void getStateInformation(MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    GuitarSynth synth;
    Voice* voices[numStrings] = {};
    AudioProcessorValueTreeState apvts;

    int getStringForNote(int midiNote) const;

private:
    dsp::ProcessSpec spec;
    IRConvolution convolution;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Processor)
};
