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
                  << (unsigned)my/(unsigned)opt << "\t"
                  << (unsigned)my/(unsigned)r;

        if ((unsigned)my/(unsigned)opt < 1.5)
            std::cout << "\tNICE";
        else if ((unsigned)my/(unsigned)opt > 3)
            std::cout << "\tPOOR";
        std::cout << std::endl;
    }
    return 0;
}
