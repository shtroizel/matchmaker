#pragma once

#include "SerialTask.h"



bool generate_ordinal_summation_header(SerialTask::Type);

MATCHABLE_VARIANT_PROPERTY_VALUE(
    SerialTask,
    generating_spc_ordinal_summation_dot_h,
    run,
    &generate_ordinal_summation_header
)
