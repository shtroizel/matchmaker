/*
Copyright (c) 2022, shtroizel
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/



#include <SerialTask.h>

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
        static std::string const success = ": SUCCESS";
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
