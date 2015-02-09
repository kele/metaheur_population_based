#pragma once

#include <vector>
#include <functional>

using Permutation = std::vector<int>;
using Population = std::vector<Permutation>;
using CostFun = std::function<double(Permutation const&)>;

struct Job
{
    unsigned time;
    unsigned weight;
    unsigned due;
};
