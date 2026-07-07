#pragma once

#include <cmath>
#include <algorithm>

struct StringParams
{
    static constexpr float PI = 3.14159265358979323846f;

    // Material
    static constexpr float density       = 1150.0f;
    static constexpr float youngsModulus = 2.5e9f;

    // Per-string physical properties (nylon classical guitar)
    //                     E2      A2      D3      G3      B3      E4
    static constexpr float radius[6] = { 1.30e-3f, 1.00e-3f, 0.85e-3f, 0.75e-3f, 0.65e-3f, 0.55e-3f };
    static constexpr float tension[6] = { 72.0f,    68.0f,    65.0f,    60.0f,    55.0f,    50.0f    };

    // Configurable
    float dampingFactor  = 0.0001f;
    float youngsCoeff    = 0.0005f;
    int   numNodes       = 101;
    int   sampleRate     = 48000;

    // Pluck
    float yElongation        = 0.02f;
    float pluckPositionRatio = 0.125f;

    // Derived (set per-string)
    float linearDensity = 0.0f;
    float waveSpeed     = 0.0f;
    float stringRadius  = 0.0f;
    float stringTension = 0.0f;
    float dt            = 0.0f;

    void computeDerived(int stringIndex)
    {
        stringRadius  = radius[stringIndex];
        stringTension = tension[stringIndex];
        const float area = PI * stringRadius * stringRadius;
        linearDensity = density * area;
        waveSpeed     = std::sqrt(stringTension / linearDensity);
        dt            = 1.0f / static_cast<float>(sampleRate);
    }

    float computeLengthForFrequency(float frequency) const
    {
        return waveSpeed / (2.0f * frequency);
    }

    float computeDx(float stringLength) const
    {
        return stringLength / static_cast<float>(numNodes - 1);
    }

    bool isStable(float stringLength) const
    {
        const float dx = computeDx(stringLength);
        if (dt <= 0.0f || dx <= 0.0f || waveSpeed <= 0.0f) return false;
        return dx / (dt * waveSpeed) <= 1.0f;
    }

    float pluckPosition(float stringLength) const
    {
        return stringLength * pluckPositionRatio;
    }
};
