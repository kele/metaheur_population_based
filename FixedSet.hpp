#pragma once

#include <vector>

struct Fix
{

public:
    int where;
    int element;
    double alpha;
    unsigned age;

    Fix();
    bool isFixed() const;
    void fix(int where, double alpha);
    void unfix();

};

struct FixedSet : public std::vector<Fix>
{
    explicit FixedSet(unsigned size);
};
