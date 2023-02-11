#include "fill_reflections.h"

#include <iostream>

#ifdef Q_ONLY
    #include <matchmaker_q/matchmaker.h>
#else
    #include <matchmaker/matchmaker.h>
#endif

#include "Stage1Data.h"



bool fill_reflections(SerialTask::Type task)
{
    task.set_goal(Stage1Data::nil.as_mutable_syn_ant_table().size());
    task.set_progress(0);
    task.set_printed_progress(0);


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

        // task.set_goal(Stage1Data::nil.as_mutable_syn_ant_table().size());
        ++task.as_mutable_progress();
        SerialTask::check_progress(task);
    }

    return true;
}
