#pragma once

#include "SerialTask.h"



bool calculate_longest_words(int progress_steps);

MATCHABLE_VARIANT_PROPERTY_VALUE(
    SerialTask,
    calculating_spc_longest_spc_words,
    run,
    &calculate_longest_words
)
