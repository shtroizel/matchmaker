#include <read_3201_common.h>

#include <filesystem>

#include <Stage0Data.h>
#include <read_3201.h>



void read_3201_common(SerialTask::Type task)
{
   std::filesystem::path const path_3201_COMMON{Stage0Data::data_dir() + "/3201/files/COMMON.TXT"};
   Stage0Data::word_attribute::Flags base_attributes;
   read_3201_default(path_3201_COMMON, base_attributes, task);
}
