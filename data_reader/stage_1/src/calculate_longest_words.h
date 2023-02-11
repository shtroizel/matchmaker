#pragma once

#include "SerialTask.h"



bool calculate_longest_words(SerialTask::Type);

MATCHABLE_VARIANT_PROPERTY_VALUE(
    SerialTask,
    calculating_spc_longest_spc_words,
    run,
    &calculate_longest_words
)
