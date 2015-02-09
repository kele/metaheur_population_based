#pragma once

#include "Common.hpp"
#include "PermUtils.hpp"
#include "FixedSet.hpp"

class Solver
{
public:
    Permutation solve(CostFun const &cost,
                      unsigned problem_size,
                      unsigned population_size,
                      unsigned aging_threshold,
                      unsigned alpha_threshold)
    {
        m_problem_size = problem_size;
        m_population_size = population_size;
        m_aging_threshold = aging_threshold;
        m_alpha_threshold = alpha_threshold;

        Population population = createRandomPopulation(population_size, problem_size);
        Permutation best = getBestSolution(population, cost);
        FixedSet fixed_set = createEmptyFixedSet(problem_size);

        bool better_found = false;
        do
        {
            better_found = false;

            std::vector<Permutation> local_minimas;
            for (auto const &p : population)
            {
                auto local_minimum = localOptimum(p, cost);
                local_minimas.push_back(local_minimum);
            }
            
            Permutation new_best = getBestSolution(local_minimas, cost);
            if (cost(new_best) < cost(best))
            {
                best = new_best;
                better_found = true;
            }

            newFixedSet(fixed_set, local_minimas);
            population = newRandomPopulation(population_size, problem_size, fixed_set);
        } while (better_found);

        return best;
    }

protected:
    void newFixedSet(FixedSet &fixed, std::vector<Permutation> const &permutations)
    {
        // Ageing phase
        for (auto &e : fixed)
            if (e.isFixed())
                e.age++;

        // Inserting phase
        for (unsigned pos = 0; pos < m_problem_size; pos++)
        {
            for (auto &e : fixed)
            {
                bool fixed_something_at_pos = false;
                if (not e.isFixed())
                {
                    unsigned count = std::count_if(permutations.begin(), permutations.end(),
                                        [e, pos] (auto const &p) { return p[pos] == e.element; });

                    double alpha = static_cast<double>(count) / static_cast<double>(m_population_size);
                    if (alpha >= m_alpha_threshold)
                    {
                        e.fix(pos, alpha);
                        fixed_something_at_pos = true;
                    }
                }
                if (fixed_something_at_pos)
                    break;
            }
        }

        // Delething phase
        bool deleted = false;
        for (auto &e : fixed)
        {
            if (e.alpha/e.age <= m_aging_threshold)
            {
                e.unfix();
                deleted = true;
            }
        }
        if (not deleted)
        {
            auto it = std::min_element(fixed.begin(), fixed.end(), 
                      [](auto const &lhs, auto const &rhs) { return lhs.alpha/lhs.age < rhs.alpha/rhs.age; });
            it->unfix();
        }
    }

    Population newRandomPopulation(FixedSet const &fixed)
    {
        std::vector<int> free;
        for (auto const &f : fixed)
            if (not f.isFixed())
                free.push_back(f.element);

        const int EMPTY = -1;
        Permutation prototype(m_problem_size, EMPTY);
        for (auto const &f : fixed)
            if (f.isFixed())
                prototype[f.where] = f.element;
        
        Population result;
        for (unsigned i = 0; i < m_population_size; i++)
        {
            auto free_left = free;
            std::random_shuffle(free_left.begin(), free_left.end());

            Permutation current = prototype;
            for (auto &x : current)
            {
                if (x == EMPTY)
                {
                    x = free_left.back();
                    free_left.pop_back();
                }
            }

            result.push_back(current);
        }
        return result;
    }

    std::vector<Permutation> neighbourhood(Permutation const &p)
    {
        std::vector<Permutation> neighbours;
        for (unsigned i = 0; i < p.size(); i++)
        {
            for (unsigned j = 0; j < p.size(); j++)
            {
                if (i == j)
                    continue;
                neighbours.push_back(swap(move(p, i, j), i, j));
            }
        }
        return neighbours;
    }


    Permutation localOptimum(Permutation const &perm, CostFun const &cost)
    {
        auto neighbours = neighbourhood(perm);
        return getBestSolution(neighbours, cost);
    }

    unsigned m_problem_size;
    unsigned m_population_size;
    unsigned m_aging_threshold;
    unsigned m_alpha_threshold;

};
