#pragma once

#include "SerialTask.h"



bool read_3202(SerialTask::Type);

MATCHABLE_VARIANT_PROPERTY_VALUE(SerialTask, reading_spc_3202, run, &read_3202)
