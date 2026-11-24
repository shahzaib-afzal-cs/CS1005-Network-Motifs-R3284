
#include "nullmodel.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <random>

static void degrees(const std::vector<Edge>& edges, int size, std::vector<int>& in,
                    std::vector<int>& out) {
    in.assign(size, 0);
    out.assign(size, 0);
    for (size_t i = 0; i < edges.size(); i++) {
        out[edges[i].first]++;
        in[edges[i].second]++;
    }
}

std::vector<Edge> randomize(const std::vector<Edge>& edges, long long attempts,
                            unsigned long long seed) {
    std::vector<Edge> cur = edges;
    if (cur.size() < 2) return cur;

    int size = 0;
    for (size_t i = 0; i < cur.size(); i++)
        size = std::max(size, std::max(cur[i].first, cur[i].second) + 1);

    std::vector<int> inBefore, outBefore;
    degrees(cur, size, inBefore, outBefore);

    EdgeSet E = makeEdgeSet(cur);
    std::mt19937_64 rng(seed);
    unsigned long long m = cur.size();

    for (long long t = 0; t < attempts; t++) {
        unsigned long long i = rng() % m;
        unsigned long long j = rng() % m;
        int a = cur[i].first, b = cur[i].second;
        int c = cur[j].first, d = cur[j].second;

        if (a == d || c == b) continue; 
        if (hasEdge(E, a, d) || hasEdge(E, c, b)) continue; 

        E.erase(edgeKey(a, b));
        E.erase(edgeKey(c, d));
        E.insert(edgeKey(a, d));
        E.insert(edgeKey(c, b));
        cur[i] = Edge(a, d);
        cur[j] = Edge(c, b);
    }


    std::vector<int> inAfter, outAfter;
    degrees(cur, size, inAfter, outAfter);
    assert(inAfter == inBefore);
    assert(outAfter == outBefore);
    assert(cur.size() == edges.size());
    assert(E.size() == cur.size());

    return cur;
}

void mean_and_sd(const std::vector<Census>& random, std::array<double, 13>& mean,
                 std::array<double, 13>& sd) {
    double n = static_cast<double>(random.size());
    long long n2 = static_cast<long long>(random.size());
    for (int k = 0; k < 13; k++) {

        long long sum = 0, sumSq = 0;
        for (size_t r = 0; r < random.size(); r++) {
            sum += random[r][k];
            sumSq += random[r][k] * random[r][k];
        }
        long long varTimesN2 = n2 * sumSq - sum * sum;   
        mean[k] = sum / n;
        sd[k] = (varTimesN2 <= 0) ? 0.0 : std::sqrt(static_cast<double>(varTimesN2)) / n;
    }
}

ZScores compute_z(const Census& real, const std::vector<Census>& random) {
    std::array<double, 13> mean, sd;
    mean_and_sd(random, mean, sd);
    ZScores z;
    for (int k = 0; k < 13; k++) {
        z[k] = (sd[k] == 0.0) ? 0.0 : (real[k] - mean[k]) / sd[k];
    }
    return z;
}
