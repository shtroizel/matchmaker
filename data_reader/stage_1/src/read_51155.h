#pragma once

#include "SerialTask.h"



bool read_51155(SerialTask::Type);

MATCHABLE_VARIANT_PROPERTY_VALUE(SerialTask, reading_spc_51155, run, &read_51155)
