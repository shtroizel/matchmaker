#pragma once

#include <matchable/matchable_fwd.h>

#include <filesystem>



namespace Stage0Data { MATCHABLE_FWD(word_attribute); }
MATCHABLE_FWD(SerialTask);



void read_3201_default(
    std::filesystem::path const file_path,
    Stage0Data::word_attribute::Flags const & base_attributes,
    SerialTask::Type task
);
