#include <iostream>
#include <algorithm>
#include <functional>
#include <vector>
#include <cstdlib>
#include <cassert>

const int NOT_FIXED = -1;

const unsigned POPULATION_SIZE = 10;


using Permutation = std::vector<int>;
using Population = std::vector<Permutation>;
using CostFun = std::function<double(Permutation const&)>;

struct Fix
{
    int where = NOT_FIXED;
    int element;
    double alpha;
    unsigned age;

    bool isFixed() const
    {
        return where != NOT_FIXED;
    }
    void fix(int where)
    {
        age = 0;
        this->where = where;
    }

    void unfix()
    {
        this->where = NOT_FIXED;
    }
};

using FixedSet = std::vector<Fix>;

FixedSet createEmptyFixedSet(unsigned size)
{
    FixedSet result(size);

    for (unsigned i = 0; i < size; i++)
        result[i].element = i;

    return result;
}

Permutation swap(Permutation const &p, unsigned i, unsigned j)
{
    assert(i < p.size() and j < p.size());

    Permutation result(p);
    std::swap(result[i], result[j]);
    return result;
}

Permutation move(Permutation const &p, unsigned from, unsigned to)
{
    assert(from < p.size() and to < p.size());

    Permutation result(p);
    auto x = result[from];
    result.erase(result.begin() + from);

    if (from > to)
        result.insert(result.begin() + to, x);
    else
        result.insert(result.begin() + to - 1, x);

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
            neighbours.push_back(swap(p, i, j));
            neighbours.push_back(move(p, i, j));
        }
    }

    return neighbours;
}


Permutation localOptimum(Permutation const &perm, CostFun const &cost_function)
{
    auto neighbours = neighbourhood(perm);
    return *std::min_element(neighbours.begin(), neighbours.end(),
                            [&](auto const &lhs, auto const &rhs){ return cost_function(lhs) < cost_function(rhs); });
}

const double ALPHA_THRESHOLD = 0.6;
const double AGE_THRESHOLD = 0.15;

void newFixedSet(FixedSet &fixed, std::vector<Permutation> const &permutations)
{
    // Ageing phase
    for (auto &e : fixed)
        if (e.isFixed())
            e.age++;

    // Inserting phase
    for (auto &e : fixed)
    {
        if (not e.isFixed())
        {
            for (unsigned l = 0; l < permutations.front().size(); l++)
            {
                auto count = std::count_if(permutations.begin(), permutations.end(),
                                          [=] (auto const &p) { return p[l] == e.element; });

                double alpha = count/POPULATION_SIZE;
                if (alpha > ALPHA_THRESHOLD)
                {
                    e.fix(l);
                    break;
                }
            }
        }
    }

    // Delething phase
    bool deleted = false;
    for (auto &e : fixed)
    {
        if (e.alpha/e.age <= AGE_THRESHOLD)
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

void newPopulation(Population &population, std::vector<int> const &fixed)
{
    // TODO
}

Population createRandomPopulation(size_t size)
{
    Population population(size);

    for (auto &p : population)
        std::random_shuffle(p.begin(), p.end());

    return population;
}

Permutation getBestSolution(Population const &solutions, CostFun const &cost_function)
{
    auto compare = [cost_function] (Permutation const &lhs, Permutation const &rhs)
    {
        return cost_function(lhs) < cost_function(rhs);
    };

    return *std::max_element(solutions.begin(), solutions.end(), compare);
}


Permutation solve(unsigned population_size, unsigned iterations, CostFun const &cost_function)
{
    Population population = createRandomPopulation(population_size);
    Permutation best = getBestSolution(population, cost_function);

    for (unsigned i = 0; i < iterations; i++)
    {
         
    }

    return best;
}

void print_vec(std::vector<int> const &v)
{
    for (auto x : v)
        std::cout << x << " ";
    std::cout << std::endl;
}

int main()
{
    std::vector<int> p{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    auto n = neighbourhood(p);
    std::cout << "Size: " << n.size() << std::endl;
    for (auto const &v : n)
    {
        assert(v.size() == p.size());
        assert(std::is_permutation(p.begin(), p.end(), v.begin()));
        print_vec(v);
    }

    return 0;
}
