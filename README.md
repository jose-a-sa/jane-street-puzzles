# Jane Street Puzzles Solutions

## Description

This repository contains solutions to Jane Street's monthly puzzles, with focus of use of modern C++ for computational solutions.
No external SAT/SMT solver libraries (such as Z3) are used in this repository, with emphasis of coming up with the fastest solution as possible by thinking around the puzzle (e.g. finding the earliest exits during backtracking).
Some solutions are much faster than using SAT/SMT solvers (<1ms). Sometimes, puzzles when SAT/SMT solver would be perfect use case might be considerably slower.
All logic and search are coded from scratch for transparency and reproducibility.

## Puzzles summary

| **Month/Year** | **Problem/Solution**                                     | **Language/Method/Approach**                                   | **Submission**                          | **Comments**                                                      |
| -------------- | -------------------------------------------------------- | -------------------------------------------------------------- | --------------------------------------- | ----------------------------------------------------------------- |
| March 2025     | [Hall of Mirrors 3](2025/march/mirrors_3.md)             | C++23. Backtracking. Precomputation of integer factorizations. | :white_check_mark: Accepted             | Runtime: ~1ms                                                     |
| April 2025     | [Sum One, Somewhere](2025/april/sum-one-somewhere.md)    | Analytical solution.                                           | :white_check_mark: Accepted             |                                                                   |
| May 2025       | [Number Cross 5](2025/may/number-cross-5.md)             | C++23. Backtracking. Lookup tables of all digit displacements. | :white_check_mark: Accepted             | Runtime: ~10m10s                                                  |
| June 2025      | [Some Ones, Somewhere](2025/june/some-ones-somewhere.md) | C++23. Backtracking.                                           | :large_orange_diamond: Partially solved | Solved all partridge tilings. Missed final phrase. Runtime: ~17m. |
| July 2025      | [Robot Road Trip](2025/july/robot-road-trip.md)          | Analytical solution.                                           | :white_check_mark: Accepted             |                                                                   |
