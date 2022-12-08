#pragma once

#include "SerialTask.h"



void read_3201_compound(SerialTask::Type task);

MATCHABLE_VARIANT_PROPERTY_VALUE(SerialTask, reading_spc_3201_spc__pl_COMPOUND_pr_,
                                 run, &read_3201_compound)
