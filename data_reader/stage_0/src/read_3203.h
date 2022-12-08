#pragma once

#include "SerialTask.h"



void read_3203(SerialTask::Type task);

MATCHABLE_VARIANT_PROPERTY_VALUE(SerialTask, reading_spc_3203, run, &read_3203)
