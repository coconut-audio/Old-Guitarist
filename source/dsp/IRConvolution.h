#pragma once

#include <JuceHeader.h>

class IRConvolution
{
public:
    void prepare(const dsp::ProcessSpec& spec)
    {
        convolution.reset();
        convolution.loadImpulseResponse(
            BinaryData::mic_wav, BinaryData::mic_wavSize,
            dsp::Convolution::Stereo::yes,
            dsp::Convolution::Trim::no, 0);
        convolution.prepare(spec);
    }

    void process(dsp::ProcessContextReplacing<float>& context)
    {
        convolution.process(context);

        auto& block = context.getOutputBlock();
        block.multiplyBy(gain);
    }

    void setGain(float newGain) { gain = newGain; }
    float getGain() const { return gain; }

private:
    dsp::Convolution convolution;
    float gain = 4.0f;
};
