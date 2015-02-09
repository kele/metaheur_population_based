#pragma once

#include "Common.hpp"
#include "FixedSet.hpp"
#include "PermUtils.hpp"

class Solver
{
public:
    Permutation solve(CostFun const &cost, unsigned problem_size, unsigned population_size, unsigned reiter_threshold,
                      double age_threshold, double alpha_threshold) const;

protected:
    void adjustFixedSet(FixedSet &fixed, std::vector<Permutation> const &permutations) const;
    Population nextRandomPopulation(FixedSet const &fixed) const;
    std::vector<Permutation> neighbourhood(Permutation const &p) const;

    Permutation localOptimum(Permutation const &perm, CostFun const &cost) const;

    mutable unsigned m_problem_size;
    mutable unsigned m_population_size;
    mutable double m_age_threshold;
    mutable double m_alpha_threshold;

};
