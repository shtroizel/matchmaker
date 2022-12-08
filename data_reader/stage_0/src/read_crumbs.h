#pragma once

#include "SerialTask.h"



void read_crumbs(SerialTask::Type task);

MATCHABLE_VARIANT_PROPERTY_VALUE(SerialTask, reading_spc_Crumbs, run, &read_crumbs)
