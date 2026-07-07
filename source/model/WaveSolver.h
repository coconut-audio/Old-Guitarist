#pragma once

#include <vector>
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
    void solveTridiagonal(std::vector<float>& result,
                          const std::vector<float>& lower,
                          const std::vector<float>& diag,
                          const std::vector<float>& upper,
                          const std::vector<float>& rhs) const;

    std::vector<float> current;
    std::vector<float> previous;
    int numNodes = 0;
    float dx = 0.0f;
};
