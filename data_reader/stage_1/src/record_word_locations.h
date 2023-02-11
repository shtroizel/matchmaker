#pragma once

#include "SerialTask.h"



bool record_word_locations(SerialTask::Type);

MATCHABLE_VARIANT_PROPERTY_VALUE(SerialTask, recording_spc_word_spc_locations, run, &record_word_locations)
