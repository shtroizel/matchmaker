#pragma once

#include "SerialTask.h"



bool read_crumbs(int progress_steps);

MATCHABLE_VARIANT_PROPERTY_VALUE(SerialTask, reading_spc_crumbs, run, &read_crumbs)
