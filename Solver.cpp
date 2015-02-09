#include "Solver.hpp"

#include <algorithm>
#include <set>

#include "PermUtils.hpp"


Permutation Solver::solve(CostFun const &cost,
                          unsigned problem_size,
                          unsigned population_size,
                          unsigned reiter_threshold,
                          double age_threshold,
                          double alpha_threshold) const
{
    m_problem_size = problem_size;
    m_population_size = population_size;
    m_age_threshold = age_threshold;
    m_alpha_threshold = alpha_threshold;

    Population population = createRandomPopulation(population_size, problem_size);
    Permutation best = getBestSolution(population, cost);
    FixedSet fixed_set(problem_size);

    unsigned threshold = reiter_threshold;
    while (threshold--)
    {
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
            threshold = reiter_threshold;
        }

        adjustFixedSet(fixed_set, local_minimas);
        population = nextRandomPopulation(fixed_set);
    }
    return best;
}

void ageFixedSet(FixedSet &fixed_set)
{
    for (auto &e : fixed_set)
        if (e.isFixed())
            e.age++;
}

void fixNewElements(FixedSet &fixed_set, Population const &population, double alpha_threshold)
{
    if (population.empty())
        return;

    auto problem_size = population.front().size();

    std::set<unsigned> unfixed_positions;
    for (unsigned i = 0; i < problem_size; i++)
        unfixed_positions.insert(i);

    for (auto const &f : fixed_set)
        if (f.isFixed())
            unfixed_positions.erase(f.where);

    for (auto pos : unfixed_positions)
    {
        bool fixed_something_at_pos = false;
        for (auto &e : fixed_set)
        {
            if (not e.isFixed())
            {
                unsigned count = std::count_if(population.begin(), population.end(),
                                    [e, pos] (auto const &p) { return p[pos] == e.element; });

                double alpha = static_cast<double>(count) / static_cast<double>(problem_size);
                if (alpha >= alpha_threshold)
                {
                    e.fix(pos, alpha);
                    fixed_something_at_pos = true;
                }
            }
            if (fixed_something_at_pos)
                break;
        }
    }
}

void freeElementsDueToAge(FixedSet &fixed_set, double age_threshold)
{
    bool deleted = false;
    for (auto &e : fixed_set)
    {
        if (e.isFixed())
        {
            if (e.alpha/e.age <= age_threshold)
            {
                e.unfix();
                deleted = true;
            }
        }
    }
    if (not deleted)
    {
        bool found_something = false;
        double min_ratio = std::numeric_limits<double>::max();
        unsigned min_pos;
        for (unsigned i = 0; i < fixed_set.size(); i++)
        {
            if (fixed_set[i].isFixed())
            {
                if (fixed_set[i].alpha / fixed_set[i].age < min_ratio)
                {
                    found_something = true;
                    min_pos = i;
                    min_ratio = fixed_set[i].alpha / fixed_set[i].age;
                }
            }
        }
        if (found_something)
            fixed_set[min_pos].unfix();
    }
}

void Solver::adjustFixedSet(FixedSet &fixed, std::vector<Permutation> const &permutations) const
{
    ageFixedSet(fixed);
    fixNewElements(fixed, permutations, m_alpha_threshold);
    freeElementsDueToAge(fixed, m_age_threshold);
}

std::vector<Permutation> Solver::neighbourhood(Permutation const &p) const
{
    std::vector<Permutation> neighbours;
    for (unsigned i = 0; i < p.size(); i++)
    {
        for (unsigned j = 0; j < p.size(); j++)
        {
            if (i == j)
                continue;
            
            Permutation next = p;
            swap(next, i, j);
            move(next, i, j);
            neighbours.push_back(next);
        }
    }
    return neighbours;
}

std::vector<int> getFreeElements(FixedSet const &fixed_set)
{
    std::vector<int> free;
    for (auto const &f : fixed_set)
        if (not f.isFixed())
            free.push_back(f.element);
    return free;
}

Permutation getPrototypeWithFixedElements(FixedSet const &fixed, unsigned size, int free_elem)
{
    Permutation proto(size, free_elem);
    for (auto const &f : fixed)
        if (f.isFixed())
            proto[f.where] = f.element;
    return proto;
}

Permutation getPermutationWithRandomFreeElements(std::vector<int> free_elements,
                                                 Permutation prototype,
                                                 const int free_elem)
{
    std::random_shuffle(free_elements.begin(), free_elements.end());

    unsigned i = 0;
    for (auto &x : prototype)
        if (x == free_elem)
            x = free_elements[i++];
    return prototype;
}

Population Solver::nextRandomPopulation(FixedSet const &fixed) const
{
    auto free = getFreeElements(fixed);

    const int EMPTY = -1;
    auto prototype = getPrototypeWithFixedElements(fixed, m_problem_size, EMPTY);
    
    Population result;
    for (unsigned i = 0; i < m_population_size; i++)
        result.push_back(getPermutationWithRandomFreeElements(free, prototype, EMPTY));
    return result;
}

Permutation Solver::localOptimum(Permutation const &perm, CostFun const &cost) const
{
    auto neighbours = neighbourhood(perm);
    return getBestSolution(neighbours, cost);
}
