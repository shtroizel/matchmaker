#pragma once

#include "SerialTask.h"


bool discover_embedded_words(int progress_steps);

MATCHABLE_VARIANT_PROPERTY_VALUE(SerialTask, discover_spc_embedded_spc_words, run, &discover_embedded_words)
