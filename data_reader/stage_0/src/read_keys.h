#pragma once

#include "SerialTask.h"



void read_keys(SerialTask::Type task);

MATCHABLE_VARIANT_PROPERTY_VALUE(SerialTask, reading_spc_Keys, run, &read_keys)
