#pragma once

#include "SerialTask.h"



void read_3201_mnames(SerialTask::Type task);

MATCHABLE_VARIANT_PROPERTY_VALUE(SerialTask, reading_spc_3201_spc__parl_NAMES_mns_M_parr_,
                                 run, &read_3201_mnames)
