#pragma once

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
