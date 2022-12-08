#pragma once

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



#ifdef MATCHABLE_OMIT_BY_INDEX
    matchable with by_index required for stage 1 reader
#endif
#include <matchable/matchable_fwd.h>
#include <matchable/matchable.h>

#include <functional>



MATCHABLE_FWD(SerialTask)
using run_func = std::function<void (SerialTask::Type)>;

PROPERTYx5_MATCHABLE(
    run_func, run,
    unsigned long, progress,
    unsigned long, goal,
    int, progress_steps,
    int, printed_progress,

    SerialTask,

    preparing_spc_prefixes,
    reading_spc_3201_spc__pl_SINGLE_pr_,
    reading_spc_3201_spc__pl_COMPOUND_pr_,
    reading_spc_3201_spc__pl_COMMON_pr_,
    reading_spc_3201_spc__pl_NAMES_pr_,
    reading_spc_3201_spc__pl_NAMES_mns_F_pr_,
    reading_spc_3201_spc__pl_NAMES_mns_M_pr_,
    reading_spc_3201_spc__pl_PLACES_pr_,
    reading_spc_3201_spc__pl_CROSSWD_pr_,
    reading_spc_3201_spc__pl_CRSWD_spc_D_pr_,
    reading_spc_3201_spc__pl_ACRONYMS_pr_,
    reading_spc_3202,
    reading_spc_3203,
    reading_spc_Crumbs,
    saving_spc_matchables
)


namespace SerialTask
{
    void reset_progress();
    void check_progress(SerialTask::Type task);
}
