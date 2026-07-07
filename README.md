# Old Guitarist

Old guitarist is a physically modeled virtual guitar plugin.

![Old Guitarist](screenshot.png)

## Physical Model

| Symbol | Description | Unit |
|--------|-------------|------|
| $\gamma$ | Damping factor | - |
| $\mu$ | Linear density $\rho \pi r^2$ | kg/m |
| $E$ | Young's modulus | Pa |
| $I$ | Second moment of area $(\pi r^4 / 4)$ | m⁴ |
| $l$ | Length of the string | m |
| $x$ | Position along the string | m |
| $T$ | Tension on the string | N |
| $t$ | Time | s |
| $c$ | Wave speed $\sqrt{T / \mu}$ | m/s |
| $\theta$ | Crank–Nicolson parameter (0.5) | - |
| $r$ | String radius | m |
| $\rho$ | Material density (1150 kg/m³) | kg/m³ |
| $f_s$ | Sample rate (48000 Hz) | Hz |
| $N$ | Number of nodes (101) | - |
| $x_p$ | Pluck position $(0.125 \, l)$ | m |
| $y_{\max}$ | Pluck amplitude (0.02 m) | m |

### Wave equation

$$ \frac{\partial^2 y}{\partial t^2} = c^2 \frac{\partial^2 y}{\partial x^2} - \gamma \frac{\partial y}{\partial t} - \frac{EI}{\mu} \frac{\partial^4 y}{\partial x^4} $$

The first term on the right is the restoring force from tension (gives the pitch), the second is viscous damping (gives the decay), and the third is bending stiffness from the string's finite thickness (gives the inharmonicity).

### Initial conditions

The string is plucked at position $x_p$ to amplitude $y_{\max}$, with zero initial velocity:

$$ y(x, 0) = \frac{y_{\max}}{x_p} \, x \quad \text{for } 0 \leq x < x_p $$

$$ y(x, 0) = \frac{y_{\max}}{l - x_p} \, (l - x) \quad \text{for } x_p \leq x \leq l $$

$$ y(0, t) = y(l, t) = 0 $$

$$ \frac{\partial y}{\partial t} (x, 0) = 0 $$

### Finite difference method

The string is divided into $N$ equally spaced nodes:

$$ \Delta x = \frac{l}{N - 1}, \qquad \Delta t = \frac{1}{f_s} $$

The second derivative (tension term) uses a three-point stencil:

$$ \frac{\partial^2 y}{\partial x^2} \bigg|_{i} \approx \frac{y_{i-1} - 2y_i + y_{i+1}}{\Delta x^2} $$

The fourth derivative (bending stiffness) uses a five-point stencil:

$$ \frac{\partial^4 y}{\partial x^4} \bigg|_{i} \approx \frac{y_{i-2} - 4y_{i-1} + 6y_i - 4y_{i+1} + y_{i+2}}{\Delta x^4} $$

> The stability condition is the Courant–Friedrichs–Lewy criterion:

$$ \frac{\Delta x}{\Delta t \cdot c} \leq 1 $$

### Crank–Nicolson

The spatial operator is averaged between time steps $n$ and $n+1$ with $\theta = 0.5$:

$$ \frac{y_i^{n+1} - 2y_i^n + y_i^{n-1}}{\Delta t^2} = c^2 \left[ \theta \frac{\nabla^2 y_i^{n+1}}{\Delta x^2} + (1 - \theta) \frac{\nabla^2 y_i^n}{\Delta x^2} \right] - \frac{\gamma}{2\Delta t} \left( y_i^{n+1} - y_i^{n-1} \right) - \frac{EI}{\mu} \frac{\nabla^4 y_i^n}{\Delta x^4} $$

Collecting the unknown $y_i^{n+1}$ terms on the left yields a tridiagonal system:

$$ -\theta r \, y_{i-1}^{n+1} + (1 + 2\theta r) \, y_i^{n+1} - \theta r \, y_{i+1}^{n+1} = \text{RHS}_i $$

$$ r = \frac{c^2 \Delta t^2}{\Delta x^2} $$

$$ \text{RHS}_i = 2y_i^n - y_i^{n-1} + (1 - \theta) \, r \, \nabla^2 y_i^n + \frac{\gamma \Delta t}{2} \, y_i^{n-1} - \frac{EI}{\mu} \frac{\Delta t^2}{\Delta x^4} \, \nabla^4 y_i^n $$

Solved via Thomas algorithm $\mathcal{O}(N)$.

### Sound generation

> $y^{t}$ is sampled for $x \in (0, l)$. The tone varies with $x$. An impulse response of a guitar body is convolved with $y$ to introduce body resonance.

$$ (y * h)(t) = \int_{-\infty}^{\infty} y(\tau) \, h(t - \tau) \, d\tau $$

## Vectorization

Eigen `Map<VectorXf>` provides zero-copy views over the state buffer. Stencil operations (Laplacian, fourth derivative, RHS assembly) are vectorized as Eigen expressions. The Thomas algorithm remains sequential.

$$ \mathbf{s} = \left[ \mathbf{y}_n \;\; \mathbf{y}_{n-1} \right] \in \mathbb{R}^{2N} $$

$$ \mathbf{u} = [y_2^n, \, y_3^n, \, \ldots, \, y_{N-3}^n]^\top \in \mathbb{R}^S, \qquad S = N - 4 $$

$$ \nabla^2 \mathbf{u} = \mathbf{y}_{n,[1:S]} - 2\mathbf{u} + \mathbf{y}_{n,[3:S+2]} $$

$$ \nabla^4 \mathbf{u} = \mathbf{y}_{n,[0:S]} - 4\mathbf{y}_{n,[1:S+1]} + 6\mathbf{u} - 4\mathbf{y}_{n,[3:S+3]} + \mathbf{y}_{n,[4:S+4]} $$

$$ \mathbf{b} = 2\mathbf{u} - \mathbf{u}_{n-1} + (1-\theta) \, r \, \nabla^2 \mathbf{u} + \frac{\gamma \Delta t}{2} \, \mathbf{u}_{n-1} - \frac{EI}{\mu} \frac{\Delta t^2}{\Delta x^4} \, \nabla^4 \mathbf{u} $$

$$ \begin{bmatrix} d & o & & \\ o & d & \ddots & \\ & \ddots & \ddots & o \\ & & o & d \end{bmatrix} \mathbf{u}^{n+1} = \mathbf{b} $$

## Per-string parameters

| String | Note | $r$ (mm) | $T$ (N) | $\mu$ (kg/m) | $c$ (m/s) |
|--------|------|----------|---------|--------------|-----------|
| 6 | E2 | 1.30 | 72 | $5.72 \times 10^{-3}$ | 112.0 |
| 5 | A2 | 1.00 | 68 | $3.61 \times 10^{-3}$ | 137.4 |
| 4 | D3 | 0.85 | 65 | $2.60 \times 10^{-3}$ | 157.8 |
| 3 | G3 | 0.75 | 60 | $2.02 \times 10^{-3}$ | 172.4 |
| 2 | B3 | 0.65 | 55 | $1.52 \times 10^{-3}$ | 189.8 |
| 1 | E4 | 0.55 | 50 | $1.10 \times 10^{-3}$ | 213.2 |

$\rho = 1150 \; \text{kg/m}^3$ (nylon), $l = c / (2f)$ where $f$ is the MIDI note frequency.

## Build from Source

1.  **Clone the Repository:**

```bash
git clone https://codeberg.org/vivekvjyn/Old-guitarist.git
cd Old-guitarist
git submodule update --init --recursive
```

2.  **Build the Plugin:**

```bash
cmake -B build -G Ninja
cmake --build build
```

> To debug the code, run the standalone with GDB.

```bash
gdb Builds/Old.Guitarist_artefacts/Release/Standalone/Old.Guitarist
```
