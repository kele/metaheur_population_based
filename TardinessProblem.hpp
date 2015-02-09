#pragma once

#include "Common.hpp"

class TardinessProblemCost
{
public:
    explicit TardinessProblemCost(std::vector<Job> const &jobs)
        : m_jobs(jobs)
    {}

    unsigned operator()(Permutation const &p) const
    {
        unsigned cost = 0;
        unsigned time = 0;
        for (auto i : p)
        {
            time += m_jobs[i].time;
            if (time > m_jobs[i].due)
                cost += (time - m_jobs[i].due) * m_jobs[i].weight;
        }
        return cost;
    }

private:
    std::vector<Job> m_jobs;
};
