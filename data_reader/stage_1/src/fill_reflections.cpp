#include "fill_reflections.h"

#include <iostream>

#ifdef Q_ONLY
    #include <matchmaker_q/matchmaker.h>
#else
    #include <matchmaker/matchmaker.h>
#endif

#include "Stage1Data.h"



bool fill_reflections(int progress_steps)
{
    int printed_progress{0};
    int progress{0};
    int goal{0};


    // if you belong to me then I belong to you...

    for (auto & [w, sa] : Stage1Data::nil.as_mutable_syn_ant_table())
    {
        for (auto const & s : sa.syn)
        {
            auto & other_syn = Stage1Data::nil.as_mutable_syn_ant_table()[s].syn;
            if (std::find(other_syn.begin(), other_syn.end(), w) == other_syn.end())
                other_syn.push_back(w);
        }
        for (auto const & a : sa.ant)
        {
            auto & other_ant = Stage1Data::nil.as_mutable_syn_ant_table()[a].ant;
            if (std::find(other_ant.begin(), other_ant.end(), w) == other_ant.end())
                other_ant.push_back(w);
        }

        goal = (int) Stage1Data::nil.as_mutable_syn_ant_table().size();
        ++progress;
        if (progress % (goal / progress_steps) == 0 && printed_progress < progress_steps)
        {
            ++printed_progress;
            std::cout << "." << std::flush;
        }
    }

    return true;
}
