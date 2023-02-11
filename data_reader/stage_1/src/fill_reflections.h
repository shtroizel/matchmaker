#pragma once

#include "SerialTask.h"



bool fill_reflections(SerialTask::Type);

MATCHABLE_VARIANT_PROPERTY_VALUE(SerialTask, fill_spc_reflections, run, &fill_reflections)
