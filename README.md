# Old Guitarist

Old guitarist is a physically modeled virtual guitar plugin.

![Old Guitarist](screenshot.png)

## Physical Model

|   Symbol | Description                          | Unit       |
|----------|--------------------------------------|------------|
| $y(x,t)$ | String displacement                  | $m$        |
| $c$      | Wave speed ($\sqrt{T/\mu}$)          | $m/s$      |
| $\gamma$ | Damping factor                       | -          |
| $\mu$    | Linear density ($\rho \pi r^2$)      | $kg/m$     |
| $E$      | Young's modulus                      | $Pa$       |
| $I$      | Second moment of area ($\pi r^4/4$)  | $m^4$      |
| $l$      | Length of the string                 | $m$        |
| $x$      | Position along the string            | $m$        |
| $T$      | Tension on the string                | $N$        |
| $t$      | Time                                 | $s$        |

### Wave equation

The transverse displacement $y(x,t)$ of a stiff, damped string is governed by:

$$ \frac{\partial^2 y}{\partial t^2} = c^2 \frac{\partial^2 y}{\partial x^2} - \gamma \frac{\partial y}{\partial t} - \frac{EI}{\mu} \frac{\partial^4 y}{\partial x^4} $$

where:

$$ c = \sqrt{\frac{T}{\mu}}, \qquad \mu = \rho \pi r^2, \qquad I = \frac{\pi r^4}{4} $$

The first term on the right drives transverse wave propagation. The second term models viscous damping. The fourth-order term captures bending stiffness (dispersion — higher partials travel faster, giving nylon strings their characteristic inharmonicity).

### Boundary conditions

The string is fixed at both ends:

$$ y(0, t) = y(l, t) = 0 $$

To enforce this in the finite difference scheme, two ghost nodes are placed beyond each boundary, and all four boundary nodes ($i = 0, 1, N{-}2, N{-}1$) are held at zero displacement.

### Initial conditions (pluck shape)

The string is displaced into a triangular pluck shape at $t = 0$, with the peak at position $x_p = 0.125 \, l$:

$$ y(x, 0) = \begin{cases} \dfrac{y_{\max}}{x_p} \, x & 0 \leq x < x_p \\[6pt] \dfrac{y_{\max}}{l - x_p} \, (l - x) & x_p \leq x \leq l \end{cases} $$

where $y_{\max} = 0.02 \; m$ is the peak displacement. The initial velocity is zero: $\partial y / \partial t \, (x, 0) = 0$.

### Finite difference discretization

The string of length $l$ is divided into $N = 101$ equally spaced nodes with spatial step:

$$ \Delta x = \frac{l}{N - 1} $$

and time step $\Delta t = 1 / f_s$ where $f_s = 48000 \; Hz$.

**Spatial derivatives** are approximated by central differences:

Second derivative (Laplacian):

$$ \frac{\partial^2 y}{\partial x^2} \bigg|_{i} \approx \frac{y_{i-1} - 2y_i + y_{i+1}}{\Delta x^2} $$

Fourth derivative (bending stiffness):

$$ \frac{\partial^4 y}{\partial x^4} \bigg|_{i} \approx \frac{y_{i-2} - 4y_{i-1} + 6y_i - 4y_{i+1} + y_{i+2}}{\Delta x^4} $$

**Time integration** uses the Crank–Nicolson scheme ($\theta = 0.5$), which averages the spatial operators between the current and next time levels:

$$ \frac{y_i^{n+1} - 2y_i^n + y_i^{n-1}}{\Delta t^2} = c^2 \left[ \theta \frac{\nabla^2 y_i^{n+1}}{\Delta x^2} + (1 - \theta) \frac{\nabla^2 y_i^n}{\Delta x^2} \right] - \frac{\gamma}{2\Delta t} \left( y_i^{n+1} - y_i^{n-1} \right) - \frac{EI}{\mu} \frac{\nabla^4 y_i^n}{\Delta x^4} $$

Collecting terms with $y_i^{n+1}$ on the left and everything else on the right, the implicit part yields a **tridiagonal system** for the interior nodes $i = 2, \ldots, N{-}3$:

$$ -\theta r \, y_{i-1}^{n+1} + (1 + 2\theta r) \, y_i^{n+1} - \theta r \, y_{i+1}^{n+1} = \text{RHS}_i $$

where $r = c^2 \Delta t^2 / \Delta x^2$ is the **Courant number squared**.

### Right-hand side construction

The explicit (known) right-hand side for each interior node is:

$$ \text{RHS}_i = 2y_i^n - y_i^{n-1} + (1 - \theta) \, r \, \nabla^2 y_i^n + \frac{\gamma \Delta t}{2} \, y_i^{n-1} - \frac{EI}{\mu} \frac{\Delta t^2}{\Delta x^4} \, \nabla^4 y_i^n $$

### Tridiagonal solve (Thomas algorithm)

The tridiagonal system $\mathbf{A} \, \mathbf{y}^{n+1} = \mathbf{b}$ is solved in $O(N)$ by forward elimination and back substitution:

**Forward sweep:**

$$ c'_0 = \frac{o}{d}, \qquad d'_0 = \frac{b_0}{d} $$

$$ m_i = d - o \cdot c'_{i-1}, \qquad c'_i = \frac{o}{m_i}, \qquad d'_i = \frac{b_i - o \cdot d'_{i-1}}{m_i} $$

**Back substitution:**

$$ y_{N-3}^{n+1} = d'_{N-3} $$

$$ y_i^{n+1} = d'_i - c'_i \cdot y_{i+1}^{n+1} $$

where $d = 1 + 2\theta r$ and $o = -\theta r$.

### Stability (CFL condition)

The scheme is unconditionally stable for the wave equation part ($\theta = 0.5$), but the bending stiffness term introduces a secondary constraint. In practice the CFL condition is checked as:

$$ \frac{\Delta x}{\Delta t \cdot c} \leq 1 $$

If violated, a warning is logged but the solver continues (the implicit scheme is more forgiving than explicit).

### Sound generation

The string displacement $y(x, t)$ is sampled at a fixed point along the string. The output is normalized by the pluck amplitude and scaled by a gain factor. An impulse response of a guitar body is convolved with the output to introduce body resonance:

$$ (y * h)(t) = \int_{-\infty}^{\infty} y(\tau) \, h(t - \tau) \, d\tau $$

## Vectorization

The nodal values of the string at time step $n$ are collected into a state vector. By organizing the finite difference stencils as vector operations, the right-hand side of the linear system is assembled without explicit loops over nodes.

### State representation

The displacement at all $N$ nodes for two consecutive time levels is stored contiguously:

$$ \mathbf{s} = \left[ \mathbf{y}_n \;\; \mathbf{y}_{n-1} \right] \in \mathbb{R}^{2N} $$

where $\mathbf{y}_n = [y_0^n, y_1^n, \ldots, y_{N-1}^n]^\top$ and $\mathbf{y}_{n-1} = [y_0^{n-1}, y_1^{n-1}, \ldots, y_{N-1}^{n-1}]^\top$.

### Interior degrees of freedom

The boundary nodes $y_0 = y_1 = y_{N-2} = y_{N-1} = 0$ are fixed. The linear system is solved only for the $S = N - 4$ interior nodes. Define:

$$ \mathbf{u} = [y_2^n, \, y_3^n, \, \ldots, \, y_{N-3}^n]^\top \in \mathbb{R}^S $$

### Stencil operations as vector shifts

The finite difference stencils for the spatial derivatives can be written as vector operations by shifting the index. Let $\mathbf{y}_{n,[a:b]}$ denote the segment of $\mathbf{y}_n$ from index $a$ to $b$.

**Laplacian.** The second derivative $\nabla^2 y_i = y_{i-1} - 2y_i + y_{i+1}$ applied simultaneously to all interior nodes becomes:

$$ \nabla^2 \mathbf{u} = \mathbf{y}_{n,[1:S]} - 2\mathbf{u} + \mathbf{y}_{n,[3:S+2]} $$

This is a single expression: three vector reads, two scalar-vector multiplications, and two vector additions.

**Fourth derivative.** The biharmonic operator $\nabla^4 y_i = y_{i-2} - 4y_{i-1} + 6y_i - 4y_{i+1} + y_{i+2}$ is assembled from five shifted segments:

$$ \nabla^4 \mathbf{u} = \mathbf{y}_{n,[0:S]} - 4\mathbf{y}_{n,[1:S+1]} + 6\mathbf{u} - 4\mathbf{y}_{n,[3:S+3]} + \mathbf{y}_{n,[4:S+4]} $$

### Right-hand side assembly

The explicit right-hand side of the Crank–Nicolson scheme, evaluated at all interior nodes simultaneously, is:

$$ \mathbf{b} = 2\mathbf{u} - \mathbf{u}_{n-1} + (1-\theta) \, r \, \nabla^2 \mathbf{u} + \frac{\gamma \Delta t}{2} \, \mathbf{u}_{n-1} - \frac{EI}{\mu} \frac{\Delta t^2}{\Delta x^4} \, \nabla^4 \mathbf{u} $$

Each term on the right is a vector in $\mathbb{R}^S$. The five terms are combined in a single expression, which a compiler can evaluate as a fused sequence of SIMD operations over the $S$ components.

### Tridiagonal system

The implicit part of the Crank–Nicolson scheme produces a tridiagonal system for the unknown $\mathbf{u}^{n+1}$:

$$ \begin{bmatrix} d & o & & \\ o & d & \ddots & \\ & \ddots & \ddots & o \\ & & o & d \end{bmatrix} \mathbf{u}^{n+1} = \mathbf{b} $$

where $d = 1 + 2\theta r$ and $o = -\theta r$. This system is solved by the Thomas algorithm in $O(S)$ operations. Because each step of the forward sweep depends on the previous result, the tridiagonal solve is inherently sequential and cannot be vectorized.

### Pluck shape

The initial displacement is a triangular pluck with peak $y_{\max}$ at position $x_p$:

$$ y_i^0 = \begin{cases} \dfrac{y_{\max}}{x_p} \, x_i & x_i < x_p \\[4pt] \dfrac{y_{\max}}{l - x_p} \, (l - x_i) & x_i \geq x_p \end{cases} $$

Both $\mathbf{y}_n$ and $\mathbf{y}_{n-1}$ are initialized to this shape, corresponding to zero initial velocity.

### Per-string physical parameters

| String | Note | Radius (mm) | Tension (N) | Linear density (kg/m) | Wave speed (m/s) |
|--------|------|-------------|-------------|----------------------|-------------------|
| 6 (E2) | E2   | 1.30        | 72          | $5.72 \times 10^{-3}$ | 112.0 |
| 5 (A2) | A2   | 1.00        | 68          | $3.61 \times 10^{-3}$ | 137.4 |
| 4 (D3) | D3   | 0.85        | 65          | $2.60 \times 10^{-3}$ | 157.8 |
| 3 (G3) | G3   | 0.75        | 60          | $2.02 \times 10^{-3}$ | 172.4 |
| 2 (B3) | B3   | 0.65        | 55          | $1.52 \times 10^{-3}$ | 189.8 |
| 1 (E4) | E4   | 0.55        | 50          | $1.10 \times 10^{-3}$ | 213.2 |

Nylon density: $\rho = 1150 \; kg/m^3$. String length is computed from $l = c / (2f)$ where $f$ is the MIDI note frequency.

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
