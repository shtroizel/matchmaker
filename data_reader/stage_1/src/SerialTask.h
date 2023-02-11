#pragma once

#ifdef MATCHABLE_OMIT_BY_INDEX
    matchable with by_index required for stage 1 reader
#endif

#include <matchable/matchable.h>
#include <matchable/matchable_fwd.h>

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

    discover_spc_book_spc_vocabulary,
    reading_spc_Keys,
    reading_spc_51155,
    reading_spc_3202,
    fill_spc_reflections,
    calculating_spc_longest_spc_words,
    discover_spc_embedded_spc_words,
    generating_spc_ordinal_summation_dot_h,
    reading_spc_crumbs,
    creating_spc_books,
    recording_spc_word_spc_locations,
    patching_spc_matchables
)


namespace SerialTask
{
    void reset_progress();
    void check_progress(SerialTask::Type task);
}
