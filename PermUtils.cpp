#include "PermUtils.hpp"

#include <algorithm>

Population createRandomPopulation(unsigned population_size, unsigned permutation_size)
{
    Permutation prototype(permutation_size);
    for (unsigned i = 0; i < prototype.size(); i++)
        prototype[i] = i;

    Population population(population_size, prototype);
    for (auto &p : population)
        std::random_shuffle(p.begin(), p.end());
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

void swap(Permutation &p, unsigned i, unsigned j)
{
    std::swap(p[i], p[j]);
}

void move(Permutation &p, unsigned from, unsigned to)
{
    auto x = p[from];
    p.erase(p.begin() + from);

    if (from > to)
        p.insert(p.begin() + to, x);
    else
        p.insert(p.begin() + to - 1, x);
}
