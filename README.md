# Old Guitarist

Old guitarist is a physically modeled virtual guitar plugin.

![Old Guitarist](screenshot.png)

## Physical Model

| Symbol | Description | Unit |
|--------|-------------|------|
| $\gamma$ | Damping factor | - |
| $\mu$ | Linear density | kg/m |
| $E$ | Young's modulus | Pa |
| $I$ | Second moment of area $(\pi r^4 / 4)$ | m⁴ |
| $l$ | Length of the string | m |
| $x$ | Position along the string | m |
| $T$ | Tension on the string | N |
| $t$ | Time | s |
| $c$ | Wave speed $\sqrt{T / \mu}$ | m/s |
| $\theta$ | Crank–Nicolson parameter (0.5) | - |

### Wave equation

$$ \frac{\partial^2 y}{\partial x^2} - \frac{\mu}{T} \frac{\partial^2 y}{\partial t^2} - \gamma \frac{\partial y}{\partial t} - EI \frac{\partial^4 y}{\partial x^4} = 0 $$

### Initial conditions

$$ y(x, 0) = \begin{cases} \dfrac{y_{\max}}{x_p} \, x & 0 \leq x < x_p \\[6pt] \dfrac{y_{\max}}{l - x_p} \, (l - x) & x_p \leq x \leq l \end{cases} $$

$$ y(0, t) = y(l, t) = 0 $$

### Finite difference method

The wave equation is discretized as:

$$ \frac{y_{x+1}^{t} - 2y_{x}^{t} + y_{x-1}^{t}}{\Delta x^2} - \frac{\mu}{T}\frac{y_{x}^{t+1} - 2y_{x}^{t} + y_{x}^{t-1}}{\Delta t^2} - \gamma \frac{y_{x}^{t+1} - y_{x}^{t-1}}{2 \Delta t} - EI \frac{y_{x-2}^{t} -4y_{x-1}^{t} + 6y_{x}^{t} - 4y_{x+1}^{t} + y_{x+2}^{t}}{\Delta x^4} = 0 $$

> The stability condition is the Courant–Friedrichs–Lewy criterion:

$$ C = \sqrt{\frac{\mu}{T}} \frac{\Delta x}{\Delta t} \leq C_{\text{max}} $$

$$ \frac{1}{\Delta x} \leq \frac{1}{\Delta t} \times \sqrt{\frac{\mu}{T}} $$

### Crank–Nicolson

The spatial operator is averaged between time steps $t$ and $t+1$ with parameter $\theta = 0.5$, yielding a tridiagonal system:

$$ -\theta r \, y_{i-1}^{n+1} + (1 + 2\theta r) \, y_i^{n+1} - \theta r \, y_{i+1}^{n+1} = \text{RHS}_i $$

$$ r = \frac{c^2 \Delta t^2}{\Delta x^2} $$

Solved via Thomas algorithm ($O(N)$).

### Sound generation

> $y^{t}$ is sampled for $x \in (0, l)$. The tone varies with $x$. An impulse response of a guitar body is convolved with $y$ to introduce body resonance.

$$ (y * h)(t) = \int_{-\infty}^{\infty} y(\tau) \, h(t - \tau) \, d\tau $$

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
