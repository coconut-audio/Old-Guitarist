# Old Guitarist

Old guitarist is a physically modeled virtual guitar plugin.

![Old Guitarist](screenshot.png)

## Physical Model

| Symbol | Description | Unit |
|--------|-------------|------|
| $γ$ | Damping factor | - |
| $μ$ | Linear density $ρπr²$ | $kg/m$ |
| $E$ | Young's modulus | $Pa$ |
| $I$ | Second moment of area (πr⁴/4) | $m^4$ |
| $l$ | Length of the string | $m$ |
| $x$ | Position along the string | $m$ |
| $T$ | Tension on the string | $N$ |
| $t$ | Time | $s$ |
| $c$ | Wave speed $\sqrt{T/μ}$ | $m/s$ |
| $θ$ | Crank–Nicolson parameter (0.5) | - |

### Wave equation

$$\frac{\partial^2 y}{\partial t^2} = c^2 \frac{\partial^2 y}{\partial x^2} - \gamma \frac{\partial y}{\partial t} - \frac{EI}{\mu} \frac{\partial^4 y}{\partial x^4}$$

### Initial conditions

$$y(x, 0) = \frac{y_{\max}}{x_p} \, x \quad \text{for } 0 \leq x < x_p$$

$$y(x, 0) = \frac{y_{\max}}{l - x_p} \, (l - x) \quad \text{for } x_p \leq x \leq l$$

$$y(0, t) = y(l, t) = 0$$

### Finite difference method

Consider the expression given by

$$f' = \lim_{{\Delta x \to 0}} \frac{{f(x + \Delta x) - f(x)}}{\Delta x}$$

> In the context of finite difference method, $\Delta x$ is a finite interval, and the difference quotient is used to approximate the derivative. Instead of taking the limit as $\Delta x \to 0$, a small but finite value of $\Delta x$ is chosen.

The wave equation is discretized to obtain the following form:

$$\frac{y_{i-1} - 2y_i + y_{i+1}}{\Delta x^2} - \frac{\mu}{T}\frac{y_{x}^{t+1} - 2y_{x}^{t} + y_{x}^{t-1}}{\Delta t^2} - \gamma \frac{y_{x}^{t+1} - y_{x}^{t-1}}{2 \Delta t} - EI \frac{y_{i-2} - 4y_{i-1} + 6y_i - 4y_{i+1} + y_{i+2}}{\Delta x^4} = 0$$

> The stability analysis of finite difference schemes when applied to the numerical solution of partial differential equations is tied to the `Courant–Friedrichs–Lewy` condition, expressed as:

$$C = \sqrt{\frac{\mu}{T}} \frac{\Delta x}{\Delta t} \leq C_{\text{max}}$$

i.e,

$$\frac{1}{\Delta x} \leq \frac{1}{\Delta t} \times \sqrt{\frac{\mu}{T}}$$

### Crank–Nicolson

The spatial operator is averaged between time steps $n$ and $n+1$ with $θ = 0.5$, yielding a tridiagonal system:

$$-\theta r \, y_{i-1}^{n+1} + (1 + 2\theta r) \, y_i^{n+1} - \theta r \, y_{i+1}^{n+1} = \text{RHS}_i$$

$$r = \frac{c^2 \Delta t^2}{\Delta x^2}$$

$$\text{RHS}_i = 2y_i^n - y_i^{n-1} + (1 - \theta) \, r \, \nabla^2 y_i^n + \frac{\gamma \Delta t}{2} \, y_i^{n-1} - \frac{EI}{\mu} \frac{\Delta t^2}{\Delta x^4} \, \nabla^4 y_i^n$$

Solved via Thomas algorithm $\mathcal{O}(N)$.

### Sound generation

> $y^{t}$ is sampled for $x \in (0, l)$. The tone will vary with respect to x. Additionally, an impulse response of a guitar body is convoluted with $y$ to introduce body resonance.

$$(y * h)(t) = \int_{-\infty}^{\infty} y(\tau) \, h(t - \tau) \, d\tau$$

## Vectorization

The solver maintains a two-buffer state containing current and previous time step displacements:

$$\mathbf{s} = [\mathbf{y}_n \;\; \mathbf{y}_{n-1}] \in \mathbb{R}^{2N}$$

Only the interior nodes are updated by the tridiagonal solve:

$$\mathbf{u} = [y_2^n, \, y_3^n, \, \ldots, \, y_{N-3}^n]^T \in \mathbb{R}^S, \quad S = N - 4$$

> The second derivative stencil is computed as vectorized array operations on slices of the state:

$$\nabla^2 \mathbf{u} = \mathbf{y}_{n,1:S} - 2\mathbf{u} + \mathbf{y}_{n,3:S+2}$$

> The bending stiffness term uses a five-point stencil, also computed as vectorized operations on overlapping slices:

$$\nabla^4 \mathbf{u} = \mathbf{y}_{n,0:S} - 4\mathbf{y}_{n,1:S+1} + 6\mathbf{u} - 4\mathbf{y}_{n,3:S+3} + \mathbf{y}_{n,4:S+4}$$

> The full right-hand side vector is assembled from vectorized operations:

$$\mathbf{b} = 2\mathbf{u} - \mathbf{u}_{n-1} + (1-\theta) r \nabla^2 \mathbf{u} + \frac{\gamma \Delta t}{2} \mathbf{u}_{n-1} - \frac{EI}{\mu} \frac{\Delta t^2}{\Delta x^4} \nabla^4 \mathbf{u}$$

> The assembled right-hand side feeds into the Thomas algorithm, which remains sequential:

$$\begin{bmatrix} d & o & & \\ o & d & \ddots & \\ & \ddots & \ddots & o \\ & & o & d \end{bmatrix} \mathbf{u}^{n+1} = \mathbf{b}$$

## Per-string parameters

| String | Note | $r$ (mm) | $T$ (N) | $μ$ (kg/m) | $c$ (m/s) |
|--------|------|----------|---------|------------|-----------|
| 6 | E2 | 1.30 | 72 | $5.72 \times 10^{-3}$ | 112.0 |
| 5 | A2 | 1.00 | 68 | $3.61 \times 10^{-3}$ | 137.4 |
| 4 | D3 | 0.85 | 65 | $2.60 \times 10^{-3}$ | 157.8 |
| 3 | G3 | 0.75 | 60 | $2.02 \times 10^{-3}$ | 172.4 |
| 2 | B3 | 0.65 | 55 | $1.52 \times 10^{-3}$ | 189.8 |
| 1 | E4 | 0.55 | 50 | $1.10 \times 10^{-3}$ | 213.2 |

- $ρ = 1150 \; kg/m^3$ (nylon)
- $l = c / (2f)$

## Build from Source

1. **Clone the Repository:**
```bash
git clone https://codeberg.org/vivekvjyn/Old-guitarist.git
cd Old-guitarist
git submodule update --init --recursive
```
2. **Build the Plugin:**
```bash
cmake -B build -G Ninja
cmake --build build
```
> To debug the code, run the standalone with GDB.
```bash
gdb Builds/Old.Guitarist_artefacts/Release/Standalone/Old.Guitarist
```
