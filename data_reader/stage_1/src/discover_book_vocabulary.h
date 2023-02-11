#pragma once

#include "SerialTask.h"



bool discover_book_vocabulary(SerialTask::Type);

MATCHABLE_VARIANT_PROPERTY_VALUE(
    SerialTask,
    discover_spc_book_spc_vocabulary,
    run,
    &discover_book_vocabulary
)
