#!/usr/bin/env wolframscript

SumOneSomewhere[r_ /; 0 < r < 1] :=
  Module[{eq1, eq2, cond, p, q},
    eq1 = Equal[r, p * (2 * r - r^2) + (1 - p) * (2 * q - q^2)];
    eq2 = Equal[q, p * (2 * q - q^2)];
    cond = And[p > 0, p < 1, q > 0, q < 1];
    sol = {p, q} /. Solve[{eq1, eq2, cond}, {p, q}, Reals] // FullSimplify;
    sol
  ];

(* Test the function with a specific value of r *)

sol = SumOneSomewhere[1/2];
Print["Probability p: ", N @ sol[[1, 1]]];
Print["Probability of zero-sum path exists: ", N @ sol[[1, 2]]];

(* using Groebner basis elimination to isolate relation in p and r *)

gbp = GroebnerBasis[{-r + p * (2 * r - r^2) + (1 - p) * (2 * q - q^2),
   -1 + p * (2 - q)}, {p, q}, {q}];
gbq = GroebnerBasis[{-r + p * (2 * r - r^2) + (1 - p) * (2 * q - q^2),
   -1 + p * (2 - q)}, {p, q}, {p}];
polyp = Collect[Subtract @@ Expand @ FullSimplify[gbp[[1]] == 0, r > 0], p, Simplify];
polyq = Collect[Subtract @@ Expand @ FullSimplify[gbq[[1]] == 0, r > 0], q, Simplify];
Print["General polynomial of p : ", polyp]
Print["General polynomial of q : ", polyq]


func = Root[1 - 3 #1 + (2 + r) #1^2 + (-2 r + r^2) #1^3&, 1];
sol = func /. r -> 1/2;
dir = Directive[FontFamily -> "CMU Serif", 13, Black];
node = Style[StringRiffle[{0.5, N[sol, 15]}, {"(", ", ", ")"}], dir];
plt = Plot[Callout[func, node, {1/2, sol} + {0.1, -0.1}, {1/2, sol} + {0.002,
   -0.005}], {r, 0, 1}, Frame -> True, AspectRatio -> 1, Axes -> None, 
  FrameTicksStyle -> dir, PlotRange -> {{0, 1.0001}, {0, 1.0001}}, PlotPoints
   -> 50, FrameTicks -> {{Range[0, 1, 0.25], None}, {Range[0, 1, 0.25],
   None}}, MaxRecursion -> 5, PlotRangePadding -> 0, FrameLabel -> Map[
  Style[#, Italic, dir]&, {"r", "p"}], GridLines -> {{0, 1/2, 1}, {0, sol,
   1}}, ImageSize -> 300, Epilog -> {{Red, Disk[{1/2, sol}, 0.01]}}];
Export["sum-one-somewhere-plot.png", plt];