# Number Cross 5

## Description

![](number-cross-5-update.png)

The 11-by-11 grid above is divided into several regions. Place positive digits (1-9) into the cells. Every cell within a region must contain the same digit, and orthogonally adjacent cells in different regions must have different digits.

After doing this, place some tiles into the grid. Tiles are represented by blacking out a cell, and no two tiles are allowed to share a common edge. When a tile is placed on a cell, it displaces the value (digit) in that cell. That means the orthogonally adjacent cells, collectively, must be incremented by that displaced value. (For instance, in the example the rightmost tile displaces a 5. That displaced 5 results in increments of 1 for the cells below and to the left of the tile, and an increment of 3 for the cell above it.)

Digits may not be incremented any higher than 9. Incrementing digits may cause neighboring cells from different regions to contain the same value – this is fine. Some cells have been highlighted: these cells may not contain tiles and may not be altered by any of the increments.

And finally! Each row has been supplied with a clue. Every number formed by concatenating consecutive groups of un-tiled cells within a row must satisfy the clue given for the row. (As in the example.) Numbers must be at least two digits long and may not repeat in the grid.

The answer to this month’s puzzle is the sum of all the numbers formed in the completed grid. (As in the example.)

## Approach

The program uses a backtrack algorithm to find the solution.

-   Iterate through all the possible initial region number placements using DFS, making sure neighbor regions have different digits.
-   Compile-time computation of all possible $(d+1)(d+2)(d+3)/6$ displacements `{left, top, right, bottom}` for all digits $d\in\{1,\ldots,9\}$.
-   Iterate grid in row-major order, trying configurations where current cell is tiled (checking all valid displacements) and non-tiled.
-   While iterating each cell in a given row `Row`, we check the top cell (above row) to check if it was tiled: backtrack if the number ending on the top tiled cell does not obey predicate the row `Row-1` predicate.

## Solution

Number in order of appearance:

-   342225, 324
-   54, 225, 252
-   5343, 65, 26
-   555, 22, 816
-   55, 55, 155
-   3674412, 15
-   737, 969, 99
-   34, 46368, 89
-   53593, 5995
-   47, 887, 433

**Sum of the numbers: 4135658**

## Completed grid (number-cross-5 output)

-   **highlighted** tile is marked with **@**
-   **blocked** tile is marked with **#**
-   tile with **altered value** is marked with **'**

#### Grid with the initial region configuration

```
+---+---+---+---+---+---+---+---+---+---+---+
| 2   2   2   2   2   2   2   2   2   2   2 |
+   +---+   +   +   +   +   +   +   +   +   +
| 2 | 4 | 2   2@  2@  2   2   2   2   2   2 |
+---+   +---+---+---+---+---+---+---+   +---+
| 4   4 | 3   3   3@  3 | 4   4   4 | 2@| 4 |
+   +---+   +---+   +---+   +   +   +---+   +
| 4 | 3   3 | 4 | 3 | 1 | 4   4   4@  4@  4 |
+   +   +   +   +   +   +---+   +   +---+   +
| 4 | 3   3 | 4 | 3 | 1   1 | 4   4 | 1 | 4 |
+   +---+---+   +---+   +   +---+---+   +   +
| 4   4   4   4   4 | 1@  1   1   1   1 | 4 |
+   +---+---+---+   +---+   +   +---+   +---+
| 4 | 3@| 6@  6 | 4   4@| 1@  1 | 6 | 1   1 |
+   +   +   +   +---+---+---+---+   +---+---+
| 4 | 3@| 6   6   6   6@  6   6   6 | 7   7 |
+---+   +---+---+   +---+   +---+---+   +   +
| 3   3   3   3 | 6@| 3@| 6 | 7   7   7   7 |
+   +   +   +   +---+   +---+---+---+---+---+
| 3   3   3   3   3@  3   3   3   3   3   3 |
+   +   +---+---+---+---+---+---+   +   +   +
| 3   3 | 7   7   7   7   7   7 | 3   3   3 |
+---+---+---+---+---+---+---+---+---+---+---+
```

#### Grid after placing the tiles

```
+---+---+---+---+---+---+---+---+---+---+---+
| #   3'  4'  2   2   2   5'  #   3'  2   4'|
+   +---+   +   +   +   +   +   +   +   +   +
| 5'| 4 | #   2@  2@  5'  #   2   5'  2   # |
+---+   +---+---+---+---+---+---+---+   +---+
| #   5'| 3   4'  3@  # | 6'  5'  # | 2@| 6'|
+   +---+   +---+   +---+   +   +   +---+   +
| 7'| 3   6'| # | 3 | 2'| #   5'  4@  4@  # |
+   +   +   +   +   +   +---+   +   +---+   +
| # | 5'  5'| 5'| # | 2'  2'| #   8'| 1 | 6'|
+   +---+---+   +---+   +   +---+---+   +   +
| 5'  5'  #   5'  5'| 1@  #   1   5'  5'| # |
+   +---+---+---+   +---+   +   +---+   +---+
| # | 3@| 6@  7'| 4   4@| 1@  2'| # | 1   5'|
+   +   +   +   +---+---+---+---+   +---+---+
| 7'| 3@| 7'  #   9'  6@  9'  #   9'| 9'  # |
+---+   +---+---+   +---+   +---+---+   +   +
| 3   4'  #   4'| 6@| 3@| 6 | 8'  #   8'  9'|
+   +   +   +   +---+   +---+---+---+---+---+
| 5'  3   5'  9'  3@  #   5'  9'  9'  5'  # |
+   +   +---+---+---+---+---+---+   +   +   +
| #   4'| 7   #   8'  8'  7   # | 4'  3   3 |
+---+---+---+---+---+---+---+---+---+---+---+
```

#### Generated figures

![](number-cross-5-sol.png)

## Analytics

-   Average runtime: ~10m10s (Apple Silicon M1 Pro)
