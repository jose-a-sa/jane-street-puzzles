# Sum One, Somewhere

## Description

Robot cars have a top speed (which they prefer to maintain at all times while driving) that’s a real number randomly drawn uniformly between 1 and 2 miles per minute. A two-lane highway for robot cars has a fast lane (with minimum speed $a$) and a slow lane (with maximum speed $a$). When a faster car overtakes a slower car in the same lane, the slower car is required to decelerate to either change lanes (if both cars start in the fast lane) or stop on the shoulder (if both cars start in the slow lane). Robot cars decelerate and accelerate at a constant rate of 1 mile per minute per minute, timed so the faster, overtaking car doesn’t have to change speed at all, and passing happens instantaneously. If cars rarely meet (so you never have to consider a car meeting more than one other car on its trip, see Mathematical clarification below), and you want to minimize the miles not driven due to passing, what should a be set to, in miles per minute? Give your answer to 10 decimal places.

Example car interactions: suppose $a$ is set to 1.2 miles per minute. If a car with top speed 1.8 overtakes a car with top speed 1.1, neither has to slow down because they are in different lanes. If instead the car with top speed 1.8 overtakes one with top speed 1.7, the slower car computes the optimal time to start decelerating for 30 seconds (to reach 1.2 miles per minute to switch to the other lane) so the faster car instantly passes and the slower car can immediately start accelerating for another 30 seconds to return to 1.7 miles per minute. This pass cost 0.25 miles (how far behind where the slower car would be if it continued at 1.7 miles per minute).

If a car with top speed 1.1 overtakes one with top speed 1.0 in the slow lane, the slower (slowest!) car must decelerate for a full minute all the way to 0 to allow the pass, and then accelerate for a full minute to reestablish its speed, losing exactly 1 mile of distance.

Assume all car trips are of constant length $N$, starting at arbitrary points and times along an infinitely long highway. This is made more mathematically precise below.

Mathematical clarification: Say car trips arrive at a rate of $z$ car trip beginnings per mile per minute, uniformly across the infinite highway (cars enter and exit their trips at their preferred speed due to on/off ramps), and car trips have a constant length of $N$ miles. Define $f(z,N)$ to be the value of a that minimizes the expected lost distance per car trip due to passing. Find:

$$ \lim_{N\to\infty} \left[ \lim_{z \to 0^{+}} f(z,N) \right] $$

## Solution


### 1. Main argument

As the spanning rate tends to zero ($z \to 0^{+}$), the level of interations diminishes as trip beginnings form a Poisson process of density $z$ (starts per mile-minute). Effectively, assuming that we have a bounded space-time rectangle region
$W = [-T,T] \times [-L,L]$, then the probability of finding the trip starting in a subregion $V \subseteq W$ is given by $z |V|$.
Formally speaking, the lost distance (cost) $C_W(z, N, a)$ for trips in the rectangle $W$ is given by:

$$ C(W, z, N, a) = \sum_{k \ge 0} e^{-z|W|} \frac{(z |W|)^k}{k!} \int_{(W \times [1,2])^k} \frac{\mathrm{d}^k \mathbf{q}}{|W|^k} \, C_{k}(\set{q_i \rvert i \le k}; W, z, N, a) $$

where $q_i=(t_i, x_i, v_i)$ is a triplet for the trip start time, position and velocity, for car $i$.

The expected lost distance in the case of $k$ cars spanning $C_{k}(\set{q_i}_{i \le k}; W, z, N, a)$, especially for a bounded region, is a complicated piecewise in which we need to consider positions being in-bounds and consider higher-order interations.
For e.g., for a finite travelled length $N$, when 2 cars interact the slower of the 2 will travel longer and may allow a 3rd car to catch up to it in certain cases where it would never interact.

The important fact here is that $k$ cars will contribute with $z^k$. Therefore, as $z\to0^{+}$ interactions will become more and more sparse, and the lost distance will be dominated by $C_{2}(q_1, q_2; W, z, N, a)$, since there are no interactions for no cars or a single car

Because $C_{2}(q_1, q_2; W, z, N, a)$ is still tricky to compute for bounded case to various in-bound conditions required to be satisfied by $(t_1, x_1)$, $(t_2, x_2)$. Also, we can use translation symmetry in the unbounded case.


### 2. Lost distance of a single 2-car interaction

To compute the lost distance we can switch from random variables $v_1, v_2 \sim U(1,2)$, to fast/slow car speeds
$$v = \max(v_1, v_2) \qquad u = \min(v_1, v_2)$$
where the probability density is $f(v,u) = 2 \cdot \mathbf{1}_{u < v}$.

The lost distance in a 2 car interaction will depend whether cars are in the fast/slow lanes. Acceleration is constant at $\beta=1$ miles-per-second-per-second.
This can be divided into 3 subcases:

  1. *Both cars in the fast lane* ($u > a$): the slower car has to slow down to $a$ and then go back to $u$. Lost distance on deceleration and acceleration is equal, totalling to:

  $$ \frac{(u-a)^2}{\beta} $$
   
  2. *Both cars in the slow lane* ($v < a$): the slower car has to stop on the side (speed 0) and accelerate back to $u$. Lost distance is similar to above:
    
  $$ \frac{u^2}{\beta} $$
     
  3. *Slow car in the slow lane, fast car in the fast lane* ($v > a, u < a$): No interaction. Lost distance is $0$.

Combining, lost distance from a single 2 car interaction is:

$$ \ell(v,u) = \frac{u^2}{\beta} \mathbf{1}\_{v < a} + \frac{(u-a)^2}{\beta} \mathbf{1}\_{v < a}  $$


### 3. Density of interactions

We need to compute the rate of interations for 2 cars only, assuming that they happen infinity sparsesly separated (so it only affects the travelled time/distance once).
Take the space-time region to be the full 2d-plane and imagine two cars spans with starting coordinates/velocity $(t_1, x_1, v_1)$, $(t_2, x_2, v_2)$.
Using tarnslation symmetry we can set $(t_1, x_1) = (0,0)$ and $(t_2, x_2) = (\Delta t, \Delta x)$. We can assume, wlog, that the slower car is the one at origin.
Therefore, in order to interact, the cars trajectories must intersect at $(t^{\*}, x^{\*})$:

$$ x^{\*} = u t^{\*} \quad,\quad x^{\*} = \Delta_{x} + v (t^{\*} - \Delta_{t}) \qquad\implies\qquad t^{\*} = \frac{- \Delta_{x} + v \Delta_{t} }{v - u} $$

However, this intersection must occur during a run of a **same length $N$** for both cars: $0 < t^{\*} < \frac{N}{u}$, $\Delta_{t} < t^{\*} < \Delta_{t} + \frac{N}{v}$.
This implied that the offset distance/time between cars must be in

$$ \mathcal{R} = \left\lbrace (\Delta_{t}, \Delta_{x}) \middle\rvert : 0 < - \Delta_{x} + v \Delta_{t} < (v - u) \frac{N}{u} ~,~ 0 < - \Delta_{x} + u \Delta_{t} < (v - u) \frac{N}{v} \right\rbrace $$

The rate at which cars are able to start in the region $\mathcal{R}$ to be able to intersect the car starting at $(t,x) = (0,0)$ is simply $z | \mathcal{R} |$, which is equivalent to

$$ z |R| = z \left[ (v- u) \frac{N}{u} \right] \left[ (v - u) \frac{N}{v} \right] / \left| \det\left(\begin{matrix} v & -1 \\\ u & -1 \end{matrix} \right) \right| = z N^2 \frac{(v - u)}{u v} $$

The rate of interaction is therefore, proportional to

$$\rho(v, u) \propto 2 z N^2 \frac{(v - u)}{u v} \mathbf{1}_{u < v} $$

This can be normalized in $[1,2]^2$ but since we are optimizing for $a$, the additional constant is not relevant.

**Note:** If $v=u$ we do not have interactions as expected. One would expect that rate of interaction would be modelled by the relative velocity $(v - u)$. This would be the case if the cars travelled for the same about of fixed before exiting the highway. Because the cars tavel the **same distance $N$**, the rate is proportional to $\left(\frac{1}{u} - \frac{1}{v}\right)$ instead. This behavior is still maintained as $N \to \infty$.


### 4. Optimization equation and $a$ solution

The expected loss due to sparse interactions $(z \to 0^+)$ is given by

$$ L(a) = \int\_{[1,2]^2} \mathrm{d}v \mathrm{d}u \~ \ell(v,u) \rho(v, u) \propto \int\_{1}^{2} \mathrm{d}v \int\_{1}^{2} \mathrm{d}u \Big\[ u^2 \\,\mathbf{1}\_{v < a} + (u-a)^2 \\,\mathbf{1}\_{u > a} \Big\] \frac{(v - u)}{u v} \mathbf{1}\_{u < v} $$

Integrating this and taking the derivative we are left with solving the equation

$$ L'(a) = 0 \quad\implies\quad -5 + \frac{2}{3} a^2 \big(8 + 3 \log(2) \big) + \frac{2}{3a} - 2 a (a+4) \log(a) - 8 a \big(1 - \log(2) \big)= 0$$

Solutions to the optimization equation can be found numerically:

$$a_0 = 0.3887939223595148617\ldots$$

$$a_1 = 1.1771414168155060174\ldots$$

$$a_2 = 9.6341672917549353222\ldots$$

The only solution that obeys the problem restrictions $1 < a < 2$ is the middle root, therefore the solution to the puzzle (chopped at 10 decimal places) is

$$ a = 1.1771414168 $$
