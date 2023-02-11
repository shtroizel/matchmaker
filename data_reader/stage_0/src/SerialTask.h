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
    reading_spc_Crumbs,
    reading_spc_Keys,
    reading_spc_3201_spc__parl_SINGLE_parr_,
    reading_spc_3201_spc__parl_COMPOUND_parr_,
    reading_spc_3201_spc__parl_COMMON_parr_,
    reading_spc_3201_spc__parl_NAMES_parr_,
    reading_spc_3201_spc__parl_NAMES_mns_F_parr_,
    reading_spc_3201_spc__parl_NAMES_mns_M_parr_,
    reading_spc_3201_spc__parl_PLACES_parr_,
    reading_spc_3201_spc__parl_CROSSWD_parr_,
    reading_spc_3201_spc__parl_CRSWD_spc_D_parr_,
    reading_spc_3201_spc__parl_ACRONYMS_parr_,
    reading_spc_3202,
    reading_spc_3203,
    saving_spc_matchables
)


namespace SerialTask
{
    void reset_progress();
    void check_progress(SerialTask::Type task);
}
