#pragma once

#include "SerialTask.h"



bool patch_matchables(int progress_steps);

MATCHABLE_VARIANT_PROPERTY_VALUE(SerialTask, patching_spc_matchables, run, &patch_matchables)
