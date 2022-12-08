#pragma once

#include "SerialTask.h"



void read_3202(SerialTask::Type task);

MATCHABLE_VARIANT_PROPERTY_VALUE(SerialTask, reading_spc_3202, run, &read_3202)
