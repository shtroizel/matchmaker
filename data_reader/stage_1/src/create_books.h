#pragma once

#include "SerialTask.h"



bool create_books(int progress_steps);

MATCHABLE_VARIANT_PROPERTY_VALUE(SerialTask, creating_spc_books, run, &create_books)
