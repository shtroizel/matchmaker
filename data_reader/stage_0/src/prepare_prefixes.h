#pragma once

#include "SerialTask.h"



void prepare_prefixes(SerialTask::Type);

MATCHABLE_VARIANT_PROPERTY_VALUE(SerialTask, preparing_spc_prefixes, run, &prepare_prefixes)
