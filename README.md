# Finding Network Motifs in an Email Network (CS1005 - Discrete Structures, Fall 2026)

Language: **C++17**. No graph library is used; only the standard library.

## Group    

| Name | Roll number |
|------|-------------|
| Shahzaib Afzal (group leader) | 25I-0924 |
| Muhammad Usman | 25I-0537 |
| Hashim Amjad | 25I-0814 |
| Umair | 25I-1009 |

Section: **TODO_SECTION**

Group size: 4 members, approved by the course instructor.

## R and derived parameters

R = 924 + 537 + 814 + 1009 = **3284**

| Parameter | Formula | Value |
|-----------|---------|-------|
| Pair number | R mod 190 | 54 |
| Pair (i, j) | 54th pair in lexicographic order | (3, 4) |
| Department A | top-20[3] | **21** |
| Department B | top-20[4] | **1** |
| Number of random networks | 20 + (R mod 31) | 49 |
| Swap attempts | (5 + (R mod 6)) x edges | 7 x edges |
| Random seed | R | 3284 |

Network *t* (t = 0, 1, ...) is randomized with seed `R + t`, so every random graph is
different but the whole run is repeatable from R alone.

Top-20 departments by internal edges (largest first, ties by smaller department ID):

```
[ 0] dept 14  1506      [ 5] dept 10   472      [10] dept 19   306      [15] dept  9   146
[ 1] dept  4  1167      [ 6] dept 17   441      [11] dept 13   279      [16] dept  8   134
[ 2] dept  7   679      [ 7] dept  0   426      [12] dept 16   236      [17] dept 22   118
[ 3] dept 21   601  <-A [ 8] dept 15   357      [13] dept 36   208      [18] dept 37    99
[ 4] dept  1   502  <-B [ 9] dept 11   339      [14] dept  5   147      [19] dept 20    70
```

## How to run

1. The two dataset files are already in `data/` (see `data/README.txt` for the download links).
2. Build and run:

```
make
./motifs
```

Without `make` (for example plain Windows MinGW):

```
g++ -std=c++17 -O2 -o motifs src/main.cpp src/triad.cpp src/nullmodel.cpp src/chart.cpp
motifs.exe        (Windows)      ./motifs        (Linux / WSL / macOS)
```

3. The program prints T1 to T6 on screen and writes two files into `results/`:
   `results_R3284.csv` (all numbers) and `zscores_R3284.svg` (the T7 chart, open it in any
   browser; the title contains R).

For a PNG of the chart (for slides): `pip install matplotlib` then
`python tools/plot_png.py results/results_R3284.csv`.

Self checks (no dataset needed): `make test`.

Do not compile with `-DNDEBUG`: the in/out-degree check inside `randomize` is an `assert`.

## Files

| File | Purpose |
|------|---------|
| `src/main.cpp` | loading and cleaning (T1), department subgraphs (T2), driver, printing, results |
| `src/triad.cpp` | `triad_class`, canonical code, triad census (T3, T4) |
| `src/nullmodel.cpp` | `randomize` with degree assert (T5), Z-scores (T6) |
| `src/chart.cpp` | grouped bar chart as SVG (T7) |
| `tests/test_triad.cpp` | tests: 64 -> 16 -> 13, census vs brute force, randomization, Z rules |
| `tools/plot_png.py` | optional: PNG version of the chart from the results CSV (needs matplotlib) |
| `tools/make_sample_data.py` | makes fake data in the same format, only for trying the program |

## T1 - data cleaning

| Step | Count |
|------|-------|
| Raw edge lines | 25571 |
| Self-loops removed | 642 |
| Duplicate edges removed | 0 |
| Edges in G (simple) | 24929 |
| Nodes with a department label | 1005 |
| Departments | 42 |

## T2 - department subgraphs

| Department | Nodes | Internal edges |
|------------|-------|----------------|
| 21 (A) | 61 | 601 |
| 1 (B) | 65 | 502 |

## T3 - hand example

Triad with nodes a, b, c and edges a->b and b->c.
Bits: 0: 0->1, 1: 0->2, 2: 1->0, 3: 1->2, 4: 2->0, 5: 2->1.

| Ordering (x0,x1,x2) | Edges present in that numbering | Bits | Code |
|---|---|---|---|
| (a,b,c) | 0->1, 1->2 | 0, 3 | 1 + 8 = 9 |
| (a,c,b) | 0->2, 2->1 | 1, 5 | 2 + 32 = 34 |
| (b,a,c) | 1->0, 0->2 | 2, 1 | 4 + 2 = 6 |
| (b,c,a) | 2->0, 0->1 | 4, 0 | 16 + 1 = 17 |
| (c,a,b) | 1->2, 2->0 | 3, 4 | 8 + 16 = 24 |
| (c,b,a) | 2->1, 1->0 | 5, 2 | 32 + 4 = 36 |

The minimum is **6**, which is Class 2 (a path, one node sends to another which sends on).
The program prints the same result and `make test` checks it.

## How the counting avoids duplicates

For each node v we take every pair of its neighbours (a, b). That triad is connected because v
touches both. A triad that has only two links (a path) has exactly one such centre, so it is seen
once. A triangle (three links) has three centres, so it is only counted when v is the smallest of
the three nodes. Every connected triad is therefore counted exactly once. `make test` compares
this with a brute force check over all triples.

## T4 - triad counts

| Class | Code | Dept 21 (A) | Dept 1 (B) |
|---|---|---|---|
| 1 | 3 | 126 | 443 |
| 2 | 6 | 113 | 174 |
| 3 | 7 | 908 | 1074 |
| 4 | 10 | 44 | 91 |
| 5 | 11 | 19 | 59 |
| 6 | 15 | 174 | 189 |
| 7 | 21 | 420 | 266 |
| 8 | 23 | 919 | 740 |
| 9 | 25 | 0 | 1 |
| 10 | 27 | 30 | 50 |
| 11 | 30 | 41 | 61 |
| 12 | 31 | 423 | 370 |
| 13 | 63 | 985 | 385 |

## T6 - Z-scores

49 random networks per department, 7 x edges swap attempts each, population standard deviation,
Z = 0 when sigma = 0. Motif means Z > 2, anti-motif means Z < -2.

| Class | Code | Z (A) | Z (B) | A | B |
|---|---|---|---|---|---|
| 1 | 3 | -9.79 | -9.14 | anti | anti |
| 2 | 6 | -9.98 | -9.82 | anti | anti |
| 3 | 7 | -2.16 | -2.24 | anti | anti |
| 4 | 10 | -8.88 | -10.81 | anti | anti |
| 5 | 11 | -6.19 | -5.17 | anti | anti |
| 6 | 15 | -4.82 | -3.32 | anti | anti |
| 7 | 21 | -4.30 | -8.95 | anti | anti |
| 8 | 23 | 11.95 | 10.45 | **motif** | **motif** |
| 9 | 25 | -4.75 | -4.94 | anti | anti |
| 10 | 27 | -10.53 | -10.12 | anti | anti |
| 11 | 30 | -6.61 | -3.58 | anti | anti |
| 12 | 31 | -10.48 | -2.86 | anti | anti |
| 13 | 63 | 11.99 | 13.37 | **motif** | **motif** |

Full means and standard deviations are in `results/results_R3284.csv`.

## T7 - chart

`results/zscores_R3284.svg`. Classes 1 to 13 on the x-axis, two bars per class (Dept 21 and
Dept 1), dashed red lines at Z = +2 and Z = -2, and R in the title.

## T8 - how do the two departments communicate differently?

Both departments have exactly two motifs and they are the same two: class 8 (code 23, two
reciprocal pairs sharing one person) and class 13 (code 63, the fully mutual triangle). Every one
of the other eleven classes is an anti-motif in both. The shared story is that internal department
mail is reciprocal and clustered. Once degree structure is held fixed, one-directional patterns
such as the out-star (class 1) and the feed-forward triangle (class 5) appear far less often than
chance, while groups where everyone answers everyone appear far more often. The departments differ
in how strict that reciprocity is. Dept 21 has 985 fully mutual triangles on 61 people against 385
on Dept 1's 65 people, and it crushes class 12 (code 31, a mutual triangle missing one direction)
at Z = -10.48 compared with Dept 1's mild Z = -2.86. So in Dept 21 a triangle is either completely
two-way or it does not form at all, which points to a small core of people who all correspond with
each other. Dept 1 tolerates half-reciprocal triangles and shows far more one-to-many sending in
raw counts (443 class-1 out-stars against Dept 21's 126), which looks like announcement traffic
from a few senders on top of a looser reciprocal core.

## Checks done

* `make test`: 16 orbits and 13 connected, ordering independence, census equal to brute force,
  randomization keeps degrees / edge count / no loops or duplicates, Z = 0 when sigma = 0.
* `randomize` itself uses `assert` for the in/out-degree check.
* The census was also compared with `networkx.triadic_census`, used only as a check on T4 (its
  class order is different from ours). The core counting, classification, randomization and
  Z-score code is all ours.

## Contributions

* Shahzaib Afzal: TODO_CONTRIB
* Muhammad Usman: TODO_CONTRIB
* Hashim Amjad: TODO_CONTRIB
* Umair: TODO_CONTRIB
