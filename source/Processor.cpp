#include "Processor.h"
#include "Editor.h"

Processor::Processor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
      #if ! JucePlugin_IsMidiEffect
        #if ! JucePlugin_IsSynth
          .withInput("Input", AudioChannelSet::stereo(), true)
        #endif
          .withOutput("Output", AudioChannelSet::stereo(), true)
      #endif
      ),
      apvts(*this, nullptr, "PARAMETERS", {})
#endif
{
    synth.clearVoices();
    for (int i = 0; i < numStrings; ++i)
    {
        auto* v = new Voice();
        v->setStringIndex(i);
        voices[i] = v;
        synth.addVoice(v);
    }

    synth.clearSounds();
    synth.addSound(new Sound());
}

Processor::~Processor() {}

const String Processor::getName() const { return JucePlugin_Name; }

bool Processor::acceptsMidi()  const { return true; }
bool Processor::producesMidi() const { return false; }
bool Processor::isMidiEffect() const { return false; }

double Processor::getTailLengthSeconds() const { return 0.0; }

int Processor::getNumPrograms()    { return 1; }
int Processor::getCurrentProgram() { return 0; }

void Processor::setCurrentProgram(int) {}
const String Processor::getProgramName(int) { return {}; }
void Processor::changeProgramName(int, const String&) {}

void Processor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);

    spec.maximumBlockSize = static_cast<uint32>(samplesPerBlock);
    spec.sampleRate       = sampleRate;
    spec.numChannels      = static_cast<uint32>(getMainBusNumOutputChannels());

    convolution.prepare(spec);
}

void Processor::releaseResources() {}

bool Processor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    #endif

    return true;
}

int Processor::getStringForNote(int midiNote) const
{
    int bestString = -1;
    int bestFret = maxFret + 1;

    for (int string = 5; string >= 0; --string)
    {
        const int fret = midiNote - openStringNotes[string];
        if (fret >= 0 && fret <= maxFret && fret < bestFret)
        {
            bestFret = fret;
            bestString = string;
        }
    }

    return bestString;
}

void Processor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    buffer.clear();

    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();
        const int note = msg.getNoteNumber();
        const int s = getStringForNote(note);

        if (s < 0)
            continue;

        if (msg.isNoteOn())
        {
            synth.routeNoteOn(msg.getChannel(), note, msg.getFloatVelocity(),
                              voices[s], synth.getSound(0).get());
        }
        else if (msg.isNoteOff())
        {
            voices[s]->stopNote(msg.getFloatVelocity(), true);
        }
    }

    synth.renderNextBlock(buffer, MidiBuffer(), 0, buffer.getNumSamples());

    dsp::AudioBlock<float> block(buffer);
    dsp::ProcessContextReplacing<float> context(block);
    convolution.process(context);
}

bool Processor::hasEditor() const { return true; }

AudioProcessorEditor* Processor::createEditor()
{
    return new Editor(*this);
}

void Processor::getStateInformation(MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> params(apvts.state.createXml());
    copyXmlToBinary(*params, destData);
}

void Processor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> params(getXmlFromBinary(data, sizeInBytes));
    if (params != nullptr)
        if (params->hasTagName(apvts.state.getType()))
            apvts.state = ValueTree::fromXml(*params);
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Processor();
}
