
#include "triad.hpp"
#include <algorithm>

const int CLASS_CODES[13] = {3, 6, 7, 10, 11, 15, 21, 23, 25, 27, 30, 31, 63};

// bit k of a code <-> k-th edge:  0->1, 0->2, 1->0, 1->2, 2->0, 2->1
static const int EDGE_FROM[6] = {0, 0, 1, 1, 2, 2};
static const int EDGE_TO[6]   = {1, 2, 0, 2, 0, 1};


static const int BIT_OF[3][3] = {
    {-1, 0, 1},
    { 2, -1, 3},
    { 4, 5, -1}
};

static const int ORDERS[6][3] = {
    {0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}
};

static const int* canonTable() {
    static int table[64];
    static bool ready = false;
    if (!ready) {
        for (int c = 0; c < 64; c++) {
            int best = 64;
            for (int p = 0; p < 6; p++) {

                int code = 0;
                for (int k = 0; k < 6; k++) {
                    int from = ORDERS[p][EDGE_FROM[k]];
                    int to = ORDERS[p][EDGE_TO[k]];
                    if (c & (1 << BIT_OF[from][to])) code |= (1 << k);
                }
                best = std::min(best, code);
            }
            table[c] = best;
        }
        ready = true;
    }
    return table;
}

int canonical_of_raw(int rawCode) {
    return canonTable()[rawCode];
}

int raw_code(const EdgeSet& E, int a, int b, int c) {
    int x[3] = {a, b, c};
    int code = 0;
    for (int k = 0; k < 6; k++) {
        if (hasEdge(E, x[EDGE_FROM[k]], x[EDGE_TO[k]])) code |= (1 << k);
    }
    return code;
}

int canonical_code(const EdgeSet& E, int a, int b, int c) {
    return canonTable()[raw_code(E, a, b, c)];
}

int class_from_code(int code) {
    for (int i = 0; i < 13; i++) {
        if (CLASS_CODES[i] == code) return i + 1;
    }
    return 0;
}

int triad_class(const EdgeSet& E, int a, int b, int c) {
    return class_from_code(canonical_code(E, a, b, c));
}

Census triad_census(int n, const std::vector<Edge>& edges) {
    Census result;
    result.fill(0);

    EdgeSet E = makeEdgeSet(edges);


    std::vector<std::vector<int> > nbr(n);
    for (size_t i = 0; i < edges.size(); i++) {
        nbr[edges[i].first].push_back(edges[i].second);
        nbr[edges[i].second].push_back(edges[i].first);
    }
    for (int v = 0; v < n; v++) {
        std::sort(nbr[v].begin(), nbr[v].end());
        nbr[v].erase(std::unique(nbr[v].begin(), nbr[v].end()), nbr[v].end());
    }


    for (int v = 0; v < n; v++) {
        const std::vector<int>& nb = nbr[v];
        for (size_t i = 0; i < nb.size(); i++) {
            for (size_t j = i + 1; j < nb.size(); j++) {
                int a = nb[i], b = nb[j];
                bool linked = hasEdge(E, a, b) || hasEdge(E, b, a);
                if (linked && (v > a || v > b)) continue;
                int cls = triad_class(E, v, a, b);
                if (cls >= 1) result[cls - 1]++;
            }
        }
    }
    return result;
}
