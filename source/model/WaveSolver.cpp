#include "WaveSolver.h"
#include <algorithm>
#include <cmath>

void WaveSolver::reset(const StringParams& params, float stringLength)
{
    numNodes = params.numNodes;
    dx = params.computeDx(stringLength);
    const auto nodeCount = static_cast<size_t>(numNodes);
    current.resize(nodeCount, 0.0f);
    previous.resize(nodeCount, 0.0f);

    std::fill(current.begin(), current.end(), 0.0f);
    std::fill(previous.begin(), previous.end(), 0.0f);

    applyInitialPluck(params.pluckPosition(stringLength), stringLength);
}

void WaveSolver::advance(const StringParams& params, float& tensionDecay)
{
    const int N = numNodes;
    const float dt = params.dt;
    const float dx2 = dx * dx;
    const float dx4 = dx2 * dx2;

    const float c = params.waveSpeed + tensionDecay;
    const float r = c * c * dt * dt / dx2;

    const float gamma = params.dampingFactor;
    const float E2 = params.youngsCoeff * params.youngsCoeff;

    const float theta = 0.5f;

    const int systemSize = N - 4;
    if (systemSize <= 0) return;

    std::vector<float> lower(static_cast<size_t>(systemSize), -theta * r);
    std::vector<float> diag(static_cast<size_t>(systemSize), 1.0f + 2.0f * theta * r);
    std::vector<float> upper(static_cast<size_t>(systemSize), -theta * r);
    std::vector<float> rhs(static_cast<size_t>(systemSize), 0.0f);
    std::vector<float> solution(static_cast<size_t>(systemSize), 0.0f);

    for (int idx = 0; idx < systemSize; ++idx)
    {
        const int i = idx + 2;

        const float laplacianExplicit = current[static_cast<size_t>(i - 1)]
                                      - 2.0f * current[static_cast<size_t>(i)]
                                      + current[static_cast<size_t>(i + 1)];

        const float uPrev = previous[static_cast<size_t>(i)];
        const float uCurr = current[static_cast<size_t>(i)];

        float dampingContribution = 0.0f;
        if (dt > 0.0f)
            dampingContribution = (gamma / (2.0f * dt)) * uPrev;

        const float bendingContribution = E2 * (
            current[static_cast<size_t>(i - 2)]
          - 4.0f * current[static_cast<size_t>(i - 1)]
          + 6.0f * uCurr
          - 4.0f * current[static_cast<size_t>(i + 1)]
          + current[static_cast<size_t>(i + 2)]
        ) / dx4;

        rhs[static_cast<size_t>(idx)] =
              2.0f * uCurr
            - uPrev
            + dt * dt * (1.0f - theta) * c * c * laplacianExplicit / dx2
            + dt * dt * dampingContribution
            - dt * dt * bendingContribution;
    }

    solveTridiagonal(solution, lower, diag, upper, rhs);

    for (int idx = 0; idx < systemSize; ++idx)
    {
        const int i = idx + 2;
        previous[static_cast<size_t>(i)] = current[static_cast<size_t>(i)];
        current[static_cast<size_t>(i)] = solution[static_cast<size_t>(idx)];
    }

    current[0] = 0.0f;
    current[1] = 0.0f;
    current[static_cast<size_t>(N - 1)] = 0.0f;
    current[static_cast<size_t>(N - 2)] = 0.0f;
    previous[0] = 0.0f;
    previous[1] = 0.0f;
    previous[static_cast<size_t>(N - 1)] = 0.0f;
    previous[static_cast<size_t>(N - 2)] = 0.0f;

    if (tensionDecay > 0.0f)
    {
        tensionDecay -= 100.0f * dt;
        if (tensionDecay < 0.0f)
            tensionDecay = 0.0f;
    }
}

void WaveSolver::solveTridiagonal(std::vector<float>& result,
                                  const std::vector<float>& lower,
                                  const std::vector<float>& diag,
                                  const std::vector<float>& upper,
                                  const std::vector<float>& rhs) const
{
    const auto n = static_cast<int>(result.size());
    if (n <= 0) return;

    std::vector<float> cPrime(static_cast<size_t>(n), 0.0f);
    std::vector<float> dPrime(static_cast<size_t>(n), 0.0f);

    cPrime[0] = upper[0] / diag[0];
    dPrime[0] = rhs[0] / diag[0];

    for (int i = 1; i < n; ++i)
    {
        const auto ui = static_cast<size_t>(i);
        const auto up = static_cast<size_t>(i - 1);
        const float m = diag[ui] - lower[ui] * cPrime[up];
        cPrime[ui] = upper[ui] / m;
        dPrime[ui] = (rhs[ui] - lower[ui] * dPrime[up]) / m;
    }

    result[static_cast<size_t>(n - 1)] = dPrime[static_cast<size_t>(n - 1)];

    for (int i = n - 2; i >= 0; --i)
    {
        const auto ui = static_cast<size_t>(i);
        const auto un = static_cast<size_t>(i + 1);
        result[ui] = dPrime[ui] - cPrime[ui] * result[un];
    }
}

float WaveSolver::sample(int nodeIndex) const
{
    if (nodeIndex < 0 || nodeIndex >= numNodes)
        return 0.0f;
    return current[static_cast<size_t>(nodeIndex)];
}

const float* WaveSolver::getState() const
{
    return current.data();
}

int WaveSolver::getNumNodes() const
{
    return numNodes;
}

void WaveSolver::applyInitialPluck(float pluckPosition, float stringLength)
{
    const float peakDisplacement = 0.02f;
    const float dyLeft  = peakDisplacement / pluckPosition;
    const float dyRight = peakDisplacement / (stringLength - pluckPosition);

    for (int i = 0; i < numNodes; ++i)
    {
        const float position = static_cast<float>(i)
                             * (stringLength / static_cast<float>(numNodes - 1));

        if (position < pluckPosition)
            current[static_cast<size_t>(i)] = dyLeft * position;
        else
            current[static_cast<size_t>(i)] = dyRight * (stringLength - position);
    }

    std::copy(current.begin(), current.end(), previous.begin());
}
