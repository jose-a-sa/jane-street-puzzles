#!/usr/bin/env python

from sympy import Symbol, Eq, And, nsolve, Rational, N, S

def sum_one_somewhere(r):
    """
    Solve equations {r == p (2r - r^2) + (1 - p) (2q - q^2), q == p (2q - q^2)}
    for p, q
    """
    
    # Define the symbols
    p = Symbol('p', real=True, positive=True)
    q = Symbol('q', real=True, positive=True)
    
    # Define the equations
    eq1 = Eq(r, p * (2 * r - r**2) + (1 - p) * (2 * q - q**2))
    eq2 = Eq(q, p * (2 * q - q**2))
    constraint = And(p > 0, p < 1, q > 0, q < 1)
    
    # Solve the equations
    sols = [s for s in nsolve((eq1, eq2), (p, q), (0.5,0.2)) if constraint.subs((p,q), s)]
    
    return sols

if __name__ == "__main__":
    # Example usage
    r = Rational(1, 2)
    (p_val, q_val) = sum_one_somewhere(r)
    print(f"Probability p: {N(x=p_val)}")
    print(f"Probability of zero-sum path exists: {N(q_val)}")
