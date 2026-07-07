#pragma once

#include <vector>
#include <Eigen/Dense>
#include "StringParams.h"

class WaveSolver
{
public:
    WaveSolver() = default;
    void reset(const StringParams& params, float stringLength);
    void advance(const StringParams& params, float& tensionDecay);

    float sample(int nodeIndex) const;
    const float* getState() const;
    int getNumNodes() const;

private:
    void applyInitialPluck(float pluckPosition, float stringLength);

    std::vector<float> stateBuffer;
    int numNodes = 0;
    float dx = 0.0f;
};
