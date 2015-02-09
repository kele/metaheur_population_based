#include "FixedSet.hpp"

const int NOT_FIXED = -1;

Fix::Fix()
{
    where = NOT_FIXED;
}

bool Fix::isFixed() const
{
    return where != NOT_FIXED;
}

void Fix::fix(int where, double alpha)
{
    age = 1;
    this->where = where;
    this->alpha = alpha;
}

void Fix::unfix()
{
    where = NOT_FIXED;
}


FixedSet::FixedSet(unsigned size)
    : std::vector<Fix>(size)
{
    for (unsigned i = 0; i < size; i++)
        (*this)[i].element = i;
}
