#include <iostream>
#include <vector>
#include <cstdlib>

#include "Solver.hpp"
#include "TardinessProblem.hpp"

void print_vec(std::vector<int> const &v)
{
    for (auto x : v)
        std::cout << x << " ";
    std::cout << std::endl;
}

int main(int argc, char **argv)
{
    if (argc != 7)
    {
        std::cout << "Usage: ./heur problem_size population_size instances age_threshold alpha_threshold reiter_threshold";
        return 1;
    }

    unsigned problem_size = atoi(argv[1]);
    unsigned instances = atoi(argv[2]);
    unsigned population_size = atoi(argv[3]);
    double age_threshold = atof(argv[4]);
    double alpha_threshold = atof(argv[5]);
    unsigned reiter_threshold = atoi(argv[6]);

    std::vector<std::vector<Job>> problem_instances;
    for (unsigned t = 0; t < instances; t++)
    {
        std::vector<Job> jobs(problem_size);
        for (unsigned i = 0; i < problem_size; i++)
            std::cin >> jobs[i].time;
        for (unsigned i = 0; i < problem_size; i++)
            std::cin >> jobs[i].weight;
        for (unsigned i = 0; i < problem_size; i++)
            std::cin >> jobs[i].due;
        problem_instances.push_back(jobs);
    }


    Solver solver;
    for (unsigned t = 0; t < instances; t++)
    {
        TardinessProblemCost cost_function(problem_instances[t]);       
        auto solution = solver.solve(cost_function, problem_size, population_size, reiter_threshold,
                                     age_threshold, alpha_threshold);
        
        std::cout << cost_function(solution) << std::endl;
    }
    return 0;
}
