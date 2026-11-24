#include <cassert>
#include <cstdlib>
#include <iostream>
#include <set>
#include <vector>

#include "../src/graph.hpp"
#include "../src/nullmodel.hpp"
#include "../src/triad.hpp"

static int failures = 0;
#define CHECK(cond, msg) do { if (!(cond)) { std::cout << "FAIL: " << msg << "\n"; failures++; } } while (0)

static bool connected(const EdgeSet& E, int a, int b, int c) {
    int links = 0;
    if (hasEdge(E, a, b) || hasEdge(E, b, a)) links++;
    if (hasEdge(E, a, c) || hasEdge(E, c, a)) links++;
    if (hasEdge(E, b, c) || hasEdge(E, c, b)) links++;
    return links >= 2;
}

static std::vector<Edge> randomGraph(int n, int m, unsigned seed) {
    srand(seed);
    std::set<Edge> s;
    while (static_cast<int>(s.size()) < m) {
        int u = rand() % n, v = rand() % n;
        if (u != v) s.insert(Edge(u, v));
    }
    return std::vector<Edge>(s.begin(), s.end());
}

int main() {

    std::set<int> orbits, connectedOrbits;
    for (int c = 0; c < 64; c++) {
        int canon = canonical_of_raw(c);
        orbits.insert(canon);
        if (class_from_code(canon) > 0) connectedOrbits.insert(canon);
    }
    CHECK(orbits.size() == 16, "there should be 16 orbits");
    CHECK(connectedOrbits.size() == 13, "there should be 13 connected orbits");
    for (int i = 0; i < 13; i++) CHECK(orbits.count(CLASS_CODES[i]) == 1, "class code is a canonical code");


    {
        std::vector<Edge> e;
        e.push_back(Edge(0, 1));
        e.push_back(Edge(1, 2));
        EdgeSet S = makeEdgeSet(e);
        CHECK(canonical_code(S, 0, 1, 2) == 6, "hand example code");
        CHECK(triad_class(S, 0, 1, 2) == 2, "hand example class");
        CHECK(canonical_code(S, 2, 0, 1) == 6, "order of the arguments must not matter");
    }


    {
        std::vector<Edge> g = randomGraph(8, 20, 5);
        EdgeSet S = makeEdgeSet(g);
        for (int a = 0; a < 8; a++)
            for (int b = a + 1; b < 8; b++)
                for (int c = b + 1; c < 8; c++) {
                    int k = triad_class(S, a, b, c);
                    CHECK(k == triad_class(S, a, c, b) && k == triad_class(S, b, a, c) &&
                          k == triad_class(S, b, c, a) && k == triad_class(S, c, a, b) &&
                          k == triad_class(S, c, b, a), "ordering independence");
                    CHECK((k > 0) == connected(S, a, b, c), "class 0 exactly when disconnected");
                }
    }


    for (unsigned seed = 1; seed <= 6; seed++) {
        int n = 12 + seed * 3;
        int m = n * 2 + seed * 4;
        std::vector<Edge> g = randomGraph(n, m, seed);
        EdgeSet S = makeEdgeSet(g);
        Census brute;
        brute.fill(0);
        for (int a = 0; a < n; a++)
            for (int b = a + 1; b < n; b++)
                for (int c = b + 1; c < n; c++) {
                    int k = triad_class(S, a, b, c);
                    if (k > 0) brute[k - 1]++;
                }
        Census fast = triad_census(n, g);
        CHECK(fast == brute, "census mismatch for seed " + std::to_string(seed));
    }


    {
        std::vector<Edge> g = randomGraph(40, 200, 9);
        std::vector<Edge> r = randomize(g, 10 * g.size(), 123);
        std::set<Edge> uniq(r.begin(), r.end());
        CHECK(r.size() == g.size(), "edge count");
        CHECK(uniq.size() == r.size(), "no parallel edges");
        bool loops = false;
        for (size_t i = 0; i < r.size(); i++) if (r[i].first == r[i].second) loops = true;
        CHECK(!loops, "no self-loops");
        CHECK(r != g, "graph should have changed");
        CHECK(randomize(g, 10 * g.size(), 123) == r, "same seed must give the same graph");
    }

    // Z-score rules: sigma == 0 gives Z = 0
    {
        Census real;
        real.fill(5);
        std::vector<Census> rnd(4, real);   // all random graphs identical -> sigma = 0
        ZScores z = compute_z(real, rnd);
        for (int k = 0; k < 13; k++) CHECK(z[k] == 0.0, "sigma 0 -> Z 0");

        std::vector<Census> r2;
        int vals[8] = {2, 4, 4, 4, 5, 5, 7, 9};
        for (int i = 0; i < 8; i++) { Census c; c.fill(vals[i]); r2.push_back(c); }
        Census real2;
        real2.fill(11);
        ZScores z2 = compute_z(real2, r2);
        CHECK(z2[0] > 2.999 && z2[0] < 3.001, "population sd Z = (11-5)/2 = 3");
    }

    if (failures == 0) std::cout << "all tests passed\n";
    return failures == 0 ? 0 : 1;
}
