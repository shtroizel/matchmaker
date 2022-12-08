#pragma once

#include "SerialTask.h"



void save_matchables(SerialTask::Type);

MATCHABLE_VARIANT_PROPERTY_VALUE(SerialTask, saving_spc_matchables, run, &save_matchables)
