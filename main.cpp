#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <vector>
#include <cstdlib>
#include <cassert>

void print_vec(std::vector<int> const &v)
{
    for (auto x : v)
        std::cout << x << " ";
    std::cout << std::endl;
}

using Permutation = std::vector<int>;
using Population = std::vector<Permutation>;
using CostFun = std::function<double(Permutation const&)>;

const int NOT_FIXED = -1;
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

    void fix(int where, double alpha)
    {
        age = 0;
        this->where = where;
        this->alpha = alpha;
    }

    void unfix()
    {
        where = NOT_FIXED;
    }
};

using FixedSet = std::vector<Fix>;

struct Job
{
    unsigned time;
    double weight;
    unsigned due;
};

class TardinessProblemInstance
{
public:
    explicit TardinessProblemInstance(std::vector<Job> const &jobs)
        : m_jobs(jobs)
    {}

    double operator()(Permutation const &p)
    {
        double cost = 0;
        unsigned time = 0;
        for (auto i : p)
        {
            time += m_jobs[i].time;
            cost += std::max(0, (int)time - (int)m_jobs[i].due)*m_jobs[i].weight;
        }
        return cost;
    }

private:
    std::vector<Job> m_jobs;
};

class Solver
{
public:
    Solver(CostFun const &cost_function,
           unsigned problem_size,
           unsigned population_size,
           double aging_threshold,
           double alpha_threshold)
        : m_cost_function(cost_function),
          m_problem_size(problem_size),
          m_population_size(population_size),
          m_aging_threshold(aging_threshold),
          m_alpha_threshold(alpha_threshold)
    {}

    Permutation solve(CostFun const &cost_function,
           unsigned problem_size,
           unsigned population_size,
           double aging_threshold,
           double alpha_threshold)
    {
        m_cost_function = cost_function;
        m_problem_size = problem_size;
        m_population_size = population_size;
        m_aging_threshold = aging_threshold;
        m_alpha_threshold = alpha_threshold;

        Population population = createRandomPopulation();
        Permutation best = getBestSolution(population);
        FixedSet fixed_set = createEmptyFixedSet();

        bool better_found = false;
        do
        {
            better_found = false;
            std::vector<Permutation> local_minimas;
            
            for (auto const &p : population)
            {
                auto local_minimum = localOptimum(p, m_cost_function);
                local_minimas.push_back(local_minimum);
            }
            
            Permutation new_best = getBestSolution(local_minimas, m_cost_function);
            if (m_cost_function(new_best) < m_cost_function(best))
            {
                best = new_best;
                better_found = true;
            }

            newFixedSet(fixed_set, local_minimas);
            population = newRandomPopulation(fixed_set);
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
    bool fixed_something = false;
    for (auto &e : fixed)
    {
        if (not e.isFixed())
        {
            for (unsigned l = 0; l < permutations.front().size(); l++)
            {
                auto count = std::count_if(permutations.begin(), permutations.end(),
                                          [=] (auto const &p) { return p[l] == e.element; });

                double alpha = (double)count/(double)g_population_size;
                if (alpha >= g_alpha_threshold)
                {
                    e.fix(l, alpha);
                    fixed_something = true;
                    break;
                }
            }
        }

        if (fixed_something)
            break;
    }

    // Delething phase
    bool deleted = false;
    for (auto &e : fixed)
    {
        if (e.alpha/e.age <= g_age_threshold)
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

Population newRandomPopulation(unsigned population_size, FixedSet const &fixed, unsigned permutation_size)
{
    std::vector<int> free;
    for (auto const &f : fixed)
        if (not f.isFixed())
            free.push_back(f.element);

    Permutation prototype(permutation_size, -1);
    for (auto const &f : fixed)
        if (f.isFixed())
            prototype[f.where] = f.element;
    
    Population result;
    for (unsigned i = 0; i < population_size; i++)
    {
        auto free_left = free;
        std::random_shuffle(free_left.begin(), free_left.end());

        Permutation current = prototype;
        for (auto &x : current)
        {
            if (x == -1)
            {
                x = free_left.back();
                free_left.pop_back();
            }
        }

        result.push_back(current);
    }
    return result;
}

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

Permutation getBestSolution(std::vector<Permutation> const &solutions, CostFun const &cost_function)
{
    auto compare = [cost_function] (Permutation const &lhs, Permutation const &rhs)
    {
        return cost_function(lhs) < cost_function(rhs);
    };

    return *std::min_element(solutions.begin(), solutions.end(), compare);
}
FixedSet createEmptyFixedSet()
{
    FixedSet result(m_problem_size);

    for (unsigned i = 0; i < m_problem_size; i++)
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
            neighbours.push_back(swap(move(p, i, j), i, j));
        }
    }

    return neighbours;
}


Permutation localOptimum(Permutation const &perm)
{
    auto neighbours = neighbourhood(perm);
    return *std::min_element(neighbours.begin(), neighbours.end(),
                            [&](auto const &lhs, auto const &rhs){ return m_cost_function(lhs) < m_cost_function(rhs); });
}



private:
    CostFun m_cost_function;
    unsigned m_problem_size;
    unsigned m_population_size;
    double m_aging_threshold;
    double m_alpha_threshold;
};

int main(int argc, char **argv)
{
    
    if (argc != 3)
    {
        std::cout << "Usage: ./heur problem_size instances";
        return 1;
    }

    unsigned permutation_size = atoi(argv[1]);
    unsigned instances = atoi(argv[2]);

    std::vector<int> best_solutions;
    std::vector<int> random_solutions;
    for (unsigned t = 0; t < instances; t++)
    {
        std::vector<Job> jobs(permutation_size);
        for (unsigned i = 0; i < permutation_size; i++)
            std::cin >> jobs[i].time;
        for (unsigned i = 0; i < permutation_size; i++)
            std::cin >> jobs[i].weight;
        for (unsigned i = 0; i < permutation_size; i++)
            std::cin >> jobs[i].due;

        TardinessProblemInstance instance(jobs);
        auto solution = solve(g_population_size, permutation_size, instance);

        auto our_cost = instance(solution);
        best_solutions.push_back(our_cost);

        std::random_shuffle(solution.begin(), solution.end());
        random_solutions.push_back(instance(solution));
    }

    std::vector<int> opt_solutions;
    std::ifstream f("wtopt40.txt");
    for (unsigned i = 0; i < instances; i++)
    {
        unsigned x;
        f >> x;
        opt_solutions.push_back(x);
    }

    std::cout << "my\tr\topt\tmy/opt\tmy/r\n";
    for (unsigned i = 0; i < instances; i++)
    {
        unsigned my = best_solutions[i];
        unsigned r = random_solutions[i];
        unsigned opt = opt_solutions[i];
        std::cout << my  << "\t"
                  << r << "\t"
                  << opt << "\t"
                  << (double)my/(double)opt << "\t"
                  << (double)my/(double)r;

        if ((double)my/(double)opt < 1.5)
            std::cout << "\tNICE";
        else if ((double)my/(double)opt > 3)
            std::cout << "\tPOOR";
        std::cout << std::endl;
    }
    return 0;
}
