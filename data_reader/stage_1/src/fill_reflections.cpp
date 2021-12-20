/*
Copyright (c) 2022, shtroizel
All rights reserved without prejudice.

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
