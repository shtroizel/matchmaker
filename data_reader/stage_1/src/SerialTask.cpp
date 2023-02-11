#include "SerialTask.h"

#include <iostream>



namespace SerialTask
{
    matchable::MatchBox<SerialTask::Type, int> mod_progress;
    matchable::MatchBox<SerialTask::Type, int> prev_mod_progress;

    char progress_char(SerialTask::Type task);



    void reset_progress()
    {
        for (auto t : SerialTask::variants_by_index())
        {
            mod_progress.set(t, 0);
            prev_mod_progress.set(t, 0);
        }
    }



    void check_progress(SerialTask::Type task)
    {
        mod_progress.set(task, task.as_progress() % (task.as_goal() / task.as_progress_steps()));

        if (mod_progress.at(task) < prev_mod_progress.at(task) &&
                task.as_printed_progress() < task.as_progress_steps())
        {
            ++task.as_mutable_printed_progress();
            std::cout << progress_char(task) << std::flush;
        }
        prev_mod_progress.set(task, mod_progress.at(task));
    }



    char progress_char(SerialTask::Type task)
    {
        static std::string const success = "..SUCCESS";
        int const term_width = 80;
        int const margin = 7;
        int const transition_col = term_width - success.length() - margin + 1;
        if (transition_col != 65)
        {
            std::cout << "transition_col: " << transition_col << std::endl;
            abort();
        }
        int const cur_col = margin * 2 + 1 + (int) task.as_string().size() + task.as_printed_progress();
        if (cur_col < transition_col)
        {
            return '.';
        }
        else if (task.as_string().size() + task.as_printed_progress() <
                task.as_string().size() + task.as_progress_steps() + success.size())
        {
            int index = cur_col - transition_col;

            // should be impossible
            if (index < 0)
                abort();

            if (index < (int) success.length())
                return success[index];
        }

        return '?';
    }
}
