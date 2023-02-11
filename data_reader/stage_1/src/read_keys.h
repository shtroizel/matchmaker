#pragma once

#include "SerialTask.h"



bool read_keys(SerialTask::Type);

MATCHABLE_VARIANT_PROPERTY_VALUE(SerialTask, reading_spc_Keys, run, &read_keys)
