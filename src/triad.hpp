#pragma once
#include "graph.hpp"

extern const int CLASS_CODES[13];

int canonical_code(const EdgeSet& E, int a, int b, int c);

int triad_class(const EdgeSet& E, int a, int b, int c);

int class_from_code(int code);

int raw_code(const EdgeSet& E, int a, int b, int c);

int canonical_of_raw(int rawCode);

Census triad_census(int n, const std::vector<Edge>& edges);
