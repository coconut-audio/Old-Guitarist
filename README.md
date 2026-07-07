# Old Guitarist

Old guitarist is a physically modeled virtual guitar plugin.

![Old Guitarist](screenshot.png)

## Physical Model

The string is modeled as a damped Euler–Bernoulli beam under tension. The wave equation governs transverse displacement $y(x, t)$ along the string:

$$ \frac{\partial^2 y}{\partial t^2} = c^2 \frac{\partial^2 y}{\partial x^2} - \gamma \frac{\partial y}{\partial t} - \frac{EI}{\mu} \frac{\partial^4 y}{\partial x^4} $$

The first term on the right is the restoring force from tension (gives the wave its pitch), the second is viscous damping (gives the decay), and the third is bending stiffness from the string's finite thickness (gives the inharmonicity). The wave speed $c$ depends on tension and linear density:

$$ c = \sqrt{\frac{T}{\mu}}, \qquad \mu = \rho \pi r^2, \qquad I = \frac{\pi r^4}{4} $$

where $r$ is the string radius, $\rho$ is material density, and $T$ is tension. The moment of inertia $I$ depends on $r^4$, so thicker strings have more inharmonicity.

The string is fixed at both ends (bridge and nut):

$$ y(0, t) = y(l, t) = 0 $$

At $t = 0$ the string is plucked at position $x_p$ to amplitude $y_{\max}$, with zero initial velocity:

$$ y(x, 0) = \begin{cases} \dfrac{y_{\max}}{x_p} \, x & 0 \leq x < x_p \\[6pt] \dfrac{y_{\max}}{l - x_p} \, (l - x) & x_p \leq x \leq l \end{cases} $$

$$ \frac{\partial y}{\partial t} (x, 0) = 0 $$

The pluck position is $x_p = 0.125 \, l$ (one eighth along the string, like a real finger pluck) and the maximum displacement is $y_{\max} = 0.02 \; m$.

### Discretization

The string is divided into $N$ equally spaced nodes. The spacing and time step are:

$$ \Delta x = \frac{l}{N - 1}, \qquad \Delta t = \frac{1}{f_s}, \qquad f_s = 48000 \; Hz, \qquad N = 101 $$

Spatial derivatives are approximated with finite differences. The second derivative (for the tension term) uses a three-point stencil:

$$ \frac{\partial^2 y}{\partial x^2} \bigg|_{i} \approx \frac{y_{i-1} - 2y_i + y_{i+1}}{\Delta x^2} $$

The fourth derivative (for the bending stiffness term) uses a five-point stencil:

$$ \frac{\partial^4 y}{\partial x^4} \bigg|_{i} \approx \frac{y_{i-2} - 4y_{i-1} + 6y_i - 4y_{i+1} + y_{i+2}}{\Delta x^4} $$

### Crank–Nicolson

The time stepping uses the Crank–Nicolson scheme, which averages the spatial operators between the current and next time step. This makes the scheme unconditionally stable for the wave equation part (though the bending term still imposes a CFL condition):

$$ \frac{y_i^{n+1} - 2y_i^n + y_i^{n-1}}{\Delta t^2} = c^2 \left[ \theta \frac{\nabla^2 y_i^{n+1}}{\Delta x^2} + (1 - \theta) \frac{\nabla^2 y_i^n}{\Delta x^2} \right] - \frac{\gamma}{2\Delta t} \left( y_i^{n+1} - y_i^{n-1} \right) - \frac{EI}{\mu} \frac{\nabla^4 y_i^n}{\Delta x^4} $$

where $\theta = 0.5$ gives second-order accuracy in time.

### Tridiagonal system

Collecting the unknown $y_i^{n+1}$ terms on the left and everything else on the right, the scheme reduces to a tridiagonal system for each time step:

$$ -\theta r \, y_{i-1}^{n+1} + (1 + 2\theta r) \, y_i^{n+1} - \theta r \, y_{i+1}^{n+1} = \text{RHS}_i $$

where the Courant number $r$ is:

$$ r = \frac{c^2 \Delta t^2}{\Delta x^2} $$

and the right-hand side contains the known quantities from the current and previous time steps:

$$ \text{RHS}_i = 2y_i^n - y_i^{n-1} + (1 - \theta) \, r \, \nabla^2 y_i^n + \frac{\gamma \Delta t}{2} \, y_i^{n-1} - \frac{EI}{\mu} \frac{\Delta t^2}{\Delta x^4} \, \nabla^4 y_i^n $$

### Thomas algorithm

The tridiagonal system is solved with the Thomas algorithm, which is $O(N)$ instead of $O(N^3)$ for general matrix inversion. The diagonal elements are $d = 1 + 2\theta r$ and the off-diagonals are $o = -\theta r$.

Forward sweep computes modified coefficients:

$$ c'_0 = \frac{o}{d}, \qquad d'_0 = \frac{b_0}{d} $$

$$ m_i = d - o \cdot c'_{i-1}, \qquad c'_i = \frac{o}{m_i}, \qquad d'_i = \frac{b_i - o \cdot d'_{i-1}}{m_i} $$

Back substitution gives the solution:

$$ y_{N-3}^{n+1} = d'_{N-3}, \qquad y_i^{n+1} = d'_i - c'_i \cdot y_{i+1}^{n+1} $$

### Stability

The bending stiffness term imposes a CFL condition on the time step:

$$ \frac{\Delta x}{\Delta t \cdot c} \geq 1 $$

This is checked at initialization and the string length is adjusted if necessary to satisfy it.

### Convolution

The dry signal is convolved with a room impulse response to add spatial ambience:

$$ (y * h)(t) = \int_{-\infty}^{\infty} y(\tau) \, h(t - \tau) \, d\tau $$

## Vectorization

The solver uses Eigen for vectorized operations on the string state. The approach avoids looping over individual nodes for stencil computations, instead operating on slices of the state vector with Eigen's optimized routines.

### State

The solver maintains a two-buffer state containing the current and previous time step displacements, stored contiguously in memory:

$$ \mathbf{s} = \left[ \mathbf{y}_n \;\; \mathbf{y}_{n-1} \right] \in \mathbb{R}^{2N} $$

### Interior degrees of freedom

Only the interior nodes (excluding the fixed boundary nodes and the two ghost nodes needed for the fourth derivative stencil) are updated by the tridiagonal solve. These form the active degrees of freedom:

$$ \mathbf{u} = [y_2^n, \, y_3^n, \, \ldots, \, y_{N-3}^n]^\top \in \mathbb{R}^S, \qquad S = N - 4 $$

### Laplacian

The second derivative stencil is computed as vectorized array operations on slices of the state, avoiding a scalar loop over all nodes:

$$ \nabla^2 \mathbf{u} = \mathbf{y}_{n,[1:S]} - 2\mathbf{u} + \mathbf{y}_{n,[3:S+2]} $$

### Fourth derivative

The bending stiffness term uses a five-point stencil, also computed as vectorized operations on overlapping slices:

$$ \nabla^4 \mathbf{u} = \mathbf{y}_{n,[0:S]} - 4\mathbf{y}_{n,[1:S+1]} + 6\mathbf{u} - 4\mathbf{y}_{n,[3:S+3]} + \mathbf{y}_{n,[4:S+4]} $$

### Right-hand side

The full right-hand side vector is assembled from vectorized operations, combining the wave equation, damping, and bending terms:

$$ \mathbf{b} = 2\mathbf{u} - \mathbf{u}_{n-1} + (1-\theta) \, r \, \nabla^2 \mathbf{u} + \frac{\gamma \Delta t}{2} \, \mathbf{u}_{n-1} - \frac{EI}{\mu} \frac{\Delta t^2}{\Delta x^4} \, \nabla^4 \mathbf{u} $$

### Tridiagonal solve

The assembled right-hand side feeds into the Thomas algorithm, which remains sequential since it is inherently serial (each step depends on the previous). The tridiagonal matrix has constant diagonal and off-diagonal entries:

$$ \begin{bmatrix} d & o & & \\ o & d & \ddots & \\ & \ddots & \ddots & o \\ & & o & d \end{bmatrix} \mathbf{u}^{n+1} = \mathbf{b} $$

### Initial pluck

The pluck profile is a triangular displacement applied to the interior nodes at $t = 0$:

$$ y_i^0 = \begin{cases} \dfrac{y_{\max}}{x_p} \, x_i & x_i < x_p \\[4pt] \dfrac{y_{\max}}{l - x_p} \, (l - x_i) & x_i \geq x_p \end{cases} $$

## Per-string parameters

Each string has different physical properties that determine its tone. Thicker strings have lower pitch, more mass, and more inharmonicity from bending stiffness. The wave speed $c$ is a material property fixed per string, and MIDI notes are played by adjusting the vibrating length $l = c / (2f)$ where $f$ is the target frequency.

| String | Note | Radius (mm) | Tension (N) | Linear density (kg/m) | Wave speed (m/s) |
|--------|------|-------------|-------------|----------------------|------------------|
| 6 | E2 | 1.30 | 72 | $5.72 \times 10^{-3}$ | 112.0 |
| 5 | A2 | 1.00 | 68 | $3.61 \times 10^{-3}$ | 137.4 |
| 4 | D3 | 0.85 | 65 | $2.60 \times 10^{-3}$ | 157.8 |
| 3 | G3 | 0.75 | 60 | $2.02 \times 10^{-3}$ | 172.4 |
| 2 | B3 | 0.65 | 55 | $1.52 \times 10^{-3}$ | 189.8 |
| 1 | E4 | 0.55 | 50 | $1.10 \times 10^{-3}$ | 213.2 |

The material density is $\rho = 1150 \; kg/m^3$ (nylon), and tension values are chosen to match real classical guitar string tensions.

## Build

```bash
git clone https://codeberg.org/vivekvjyn/Old-guitarist.git
cd Old-guitarist
git submodule update --init --recursive
cmake -B build -G Ninja
cmake --build build
```
