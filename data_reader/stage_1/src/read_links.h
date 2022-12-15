#pragma once

#include "SerialTask.h"



bool read_links(int progress_steps);

MATCHABLE_VARIANT_PROPERTY_VALUE(SerialTask, reading_spc_links, run, &read_links)
