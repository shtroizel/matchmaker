#pragma once

#include "SerialTask.h"



void read_3201_names(SerialTask::Type task);

MATCHABLE_VARIANT_PROPERTY_VALUE(SerialTask, reading_spc_3201_spc__pl_NAMES_pr_, run, &read_3201_names)
