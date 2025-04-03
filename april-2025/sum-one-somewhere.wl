#!/usr/bin/env wolframscript

SumOneSomewhere[r_ /; 0 < r < 1] :=
  Module[{eq1, eq2, cond, p, q},
    eq1 = Equal[r, p * (2 * r - r^2) + (1 - p) * (2 * q - q^2)];
    eq2 = Equal[q, p * (2 * q - q^2)];
    cond = And[p > 0, p < 1, q > 0, q < 1];
    sol = {p,q}/.Solve[{eq1, eq2, cond}, {p, q}, Reals] // FullSimplify;
    sol
  ];

(* Test the function with a specific value of r *)

sol = SumOneSomewhere[1/2];
Print["Probability p: ", N@sol[[1,1]]];
Print["Probability of zero-sum path exists: ", N@sol[[1,2]]];


(* using Groebner basis elimination to isolate relation in p and r *)

gb = GroebnerBasis[{-r + p * (2 * r - r^2) + (1 - p) * (2 * q - q^2),
   -q + p * (2 * q - q^2)}, {p}, {q}];
poly = Collect[Subtract @@ Expand @ FullSimplify[gb[[1]] == 0, r > 0 && (2-r) p != 1], p, Simplify];
Print["General polynomial of p : ", poly]

