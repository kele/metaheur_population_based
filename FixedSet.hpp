#pragma once

#include <vector>

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


FixedSet createEmptyFixedSet(unsigned size)
{
    FixedSet result(size);
    for (unsigned i = 0; i < size; i++)
        result[i].element = i;

    return result;
}

