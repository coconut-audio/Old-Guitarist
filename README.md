# Old Guitarist

Old guitarist is a physically modeled virtual guitar plugin.

![Old Guitarist](screenshot.png)

## Physical Model

|   Symbol | Description                          | Unit       |
|----------|--------------------------------------|------------|
| $γ$      | Damping factor                       | -          |
| $μ$      | Linear density                       | $kg/m$     |
| $E$      | Young's modulus                      | $Pa$       |
| $I$      | Second moment of area (πr⁴/4)        | $m^4$      |
| $l$      | Length of the string                 | $m$        |
| $x$      | Position along the string            | $m$        |
| $T$      | Tension on the string                | $N$        |
| $t$      | Time                                 | $s$        |

### Wave equation

$ \frac{\partial^2 y}{\partial x^2} - \frac{\mu}{T(t)} \frac{\partial^2 y}{\partial t^2} - \gamma \frac{\partial y}{\partial t} - EI \frac{\partial^4 y}{\partial x^4} = 0 $

### Initial conditions

$ y(x, 0) = \begin{cases} \frac{4x}{l} & \text{for } 0 \leq x < \frac{l}{4} \\ \frac{4 (l - x)}{3l} & \text{for } \frac{l}{4} \leq x \leq l \end{cases} $

$ y(0, t) = y(l, t) = 0 $

### Finite difference method
Consider the expression given by

$ f' = \lim_{{\Delta x \to 0}} \frac{{f(x + \Delta x) - f(x)}}{\Delta x} $

> In the context of finite difference method, $\Delta x$ is a finite interval, and the difference quotient is used to approximate the derivative. Instead of taking the limit as $\Delta x \to 0$, a small but finite value of $\Delta x$ is chosen.

The wave equation is discretized to obtain the following form:

$ \frac{y_{x+1}^{t} - 2y_{x}^{t} + y_{x-1}^{t}}{\Delta x^2} - \frac{\mu}{T(t)}\frac{y_{x}^{t+1} - 2y_{x}^{t} + y_{x}^{t-1}}{\Delta t^2}- \gamma \frac{y_{x}^{t+1} - y_{x}^{t-1}}{2 \Delta t} - EI \frac{y_{x-2}^{t} -4y_{x-1}^{t} + 6y_{x}^{t} - 4y_{x+1}^{t} + y_{x+2}^{t}}{\Delta x^4} = 0 $

> The stability analysis of finite difference schemes when applied to the numerical solution of partial differential equations is tied to the `Courant–Friedrichs–Lewy` condition, expressed as:

$ C = \sqrt{\frac{\mu}{T}} \frac{\Delta x}{\Delta t} \leq C_{\text{max}} $

i.e,

$ \frac{1}{\Delta x} \leq \frac{1}{\Delta t} \times \sqrt{\frac{\mu}{T}} $

### Sound generation

> $y^{t}$ is sampled for $x \in (0, l)$. The tone will vary with respect to x. Additionally, an impulse response of a guitar body is convoluted with $y$ to introduce body resonance.

$ (y * h)(t) = \int_{-\infty}^{\infty} x(\tau) h(t - \tau) \, d\tau $

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
> To debug the code, run the standalone with GBD.
```bash
gdb Builds/Old.Guitarist_artefacts/Release/Standalone/Old.Guitarist
```
