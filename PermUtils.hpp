#pragma once

#include <algorithm>

#include "Common.hpp"

Population createRandomPopulation(size_t population_size, size_t permutation_size)
{
    Population population(population_size);

    for (auto &p : population)
    {
        for (unsigned i = 0; i < permutation_size; i++)
            p.push_back(i);
        std::random_shuffle(p.begin(), p.end());
    }

    return population;
}

Permutation getBestSolution(std::vector<Permutation> const &solutions, CostFun const &cost)
{
    auto compare = [cost] (Permutation const &lhs, Permutation const &rhs)
    {
        return cost(lhs) < cost(rhs);
    };

    return *std::min_element(solutions.begin(), solutions.end(), compare);
}

Permutation swap(Permutation const &p, unsigned i, unsigned j)
{
    Permutation result(p);
    std::swap(result[i], result[j]);
    return result;
}

Permutation move(Permutation const &p, unsigned from, unsigned to)
{
    Permutation result(p);
    auto x = result[from];
    result.erase(result.begin() + from);

    if (from > to)
        result.insert(result.begin() + to, x);
    else
        result.insert(result.begin() + to - 1, x);

    return result;
}
