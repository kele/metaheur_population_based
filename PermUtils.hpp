#pragma once

#include <vector>

#include "Common.hpp"

void swap(Permutation &p, unsigned i, unsigned j);
void move(Permutation &p, unsigned from, unsigned to);

Population createRandomPopulation(unsigned population_size, unsigned permutation_size);
Permutation getBestSolution(std::vector<Permutation> const &solutions, CostFun const &cost);

