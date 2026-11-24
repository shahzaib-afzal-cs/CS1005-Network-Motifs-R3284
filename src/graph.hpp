
#pragma once

#include <array>
#include <cstdint>
#include <unordered_set>
#include <utility>
#include <vector>

typedef std::pair<int, int> Edge;              
typedef std::unordered_set<long long> EdgeSet;
typedef std::array<long long, 13> Census;  


inline long long edgeKey(int u, int v) {
    return (static_cast<long long>(u) << 32) | static_cast<unsigned int>(v);
}

inline bool hasEdge(const EdgeSet& E, int u, int v) {
    return E.count(edgeKey(u, v)) > 0;
}

inline EdgeSet makeEdgeSet(const std::vector<Edge>& edges) {
    EdgeSet s;
    s.reserve(edges.size() * 2 + 1);
    for (size_t i = 0; i < edges.size(); i++) s.insert(edgeKey(edges[i].first, edges[i].second));
    return s;
}
