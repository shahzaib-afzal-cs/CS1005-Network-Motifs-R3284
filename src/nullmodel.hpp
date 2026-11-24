
#pragma once

#include <array>
#include "graph.hpp"


std::vector<Edge> randomize(const std::vector<Edge>& edges, long long attempts,
                            unsigned long long seed);

typedef std::array<double, 13> ZScores;


ZScores compute_z(const Census& real, const std::vector<Census>& random);


void mean_and_sd(const std::vector<Census>& random, std::array<double, 13>& mean,
                 std::array<double, 13>& sd);
