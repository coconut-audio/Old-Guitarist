#include "WaveSolver.h"
#include <Eigen/Dense>
#include <algorithm>
#include <cmath>

using namespace Eigen;

void WaveSolver::reset(const StringParams& params, float stringLength)
{
    numNodes = params.numNodes;
    dx = params.computeDx(stringLength);
    const auto N = static_cast<size_t>(numNodes);
    stateBuffer.resize(N * 2, 0.0f);

    Map<VectorXf> y_n(stateBuffer.data(), N);
    Map<VectorXf> y_nm1(stateBuffer.data() + N, N);
    y_n.setZero();
    y_nm1.setZero();

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
    const float EI = params.youngsModulus * params.youngsModulus;

    const float theta = 0.5f;

    const int S = N - 4;
    if (S <= 0) return;

    Map<VectorXf> y_n(stateBuffer.data(), N);
    Map<VectorXf> y_nm1(stateBuffer.data() + N, N);

    VectorXf y_interior = y_n.segment(2, S);
    VectorXf y_interior_prev = y_nm1.segment(2, S);

    VectorXf laplacian = y_n.segment(1, S) - 2.0f * y_interior + y_n.segment(3, S);

    VectorXf bending = EI / dx4 * (
          y_n.segment(0, S)
        - 4.0f * y_n.segment(1, S)
        + 6.0f * y_interior
        - 4.0f * y_n.segment(3, S)
        + y_n.segment(4, S)
    );

    VectorXf damping = (gamma / (2.0f * dt)) * y_interior_prev;

    VectorXf rhs =
          2.0f * y_interior
        - y_interior_prev
        + dt * dt * (1.0f - theta) * c * c * laplacian / dx2
        + dt * dt * damping
        - dt * dt * bending;

    const float d = 1.0f + 2.0f * theta * r;
    const float o = -theta * r;

    VectorXf cPrime(S);
    VectorXf dPrime(S);

    cPrime(0) = o / d;
    dPrime(0) = rhs(0) / d;

    for (int i = 1; i < S; ++i)
    {
        float m = d - o * cPrime(i - 1);
        cPrime(i) = o / m;
        dPrime(i) = (rhs(i) - o * dPrime(i - 1)) / m;
    }

    VectorXf y_next(S);
    y_next(S - 1) = dPrime(S - 1);

    for (int i = S - 2; i >= 0; --i)
        y_next(i) = dPrime(i) - cPrime(i) * y_next(i + 1);

    y_nm1.segment(2, S) = y_n.segment(2, S);
    y_n.segment(2, S) = y_next;

    y_n(0) = 0.0f;  y_n(1) = 0.0f;
    y_n(N - 1) = 0.0f;  y_n(N - 2) = 0.0f;
    y_nm1(0) = 0.0f;  y_nm1(1) = 0.0f;
    y_nm1(N - 1) = 0.0f;  y_nm1(N - 2) = 0.0f;

    if (tensionDecay > 0.0f)
    {
        tensionDecay -= 100.0f * dt;
        if (tensionDecay < 0.0f)
            tensionDecay = 0.0f;
    }
}

float WaveSolver::sample(int nodeIndex) const
{
    if (nodeIndex < 0 || nodeIndex >= numNodes)
        return 0.0f;
    return stateBuffer[static_cast<size_t>(nodeIndex)];
}

const float* WaveSolver::getState() const
{
    return stateBuffer.data();
}

int WaveSolver::getNumNodes() const
{
    return numNodes;
}

void WaveSolver::applyInitialPluck(float pluckPosition, float stringLength)
{
    const int N = numNodes;
    Map<VectorXf> y_n(stateBuffer.data(), N);
    Map<VectorXf> y_nm1(stateBuffer.data() + N, N);

    VectorXf pos = VectorXf::LinSpaced(N, 0.0f, stringLength);

    const float yMax = 0.02f;
    const float slopeL = yMax / pluckPosition;
    const float slopeR = yMax / (stringLength - pluckPosition);

    VectorXf pluck = pos.unaryExpr([=](float x) {
        return (x < pluckPosition) ? slopeL * x : slopeR * (stringLength - x);
    });

    y_n = pluck;
    y_nm1 = pluck;
}
