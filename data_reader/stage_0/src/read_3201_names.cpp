#include <read_3201_names.h>

#include <filesystem>

#include <SerialTask.h>
#include <Stage0Data.h>
#include <read_3201.h>



void read_3201_names(SerialTask::Type task)
{
   std::filesystem::path const path_3201_NAMES{Stage0Data::data_dir() + "/3201/files/NAMES.TXT"};
   Stage0Data::word_attribute::Flags base_attributes{Stage0Data::word_attribute::name::grab()};
   read_3201_default(path_3201_NAMES, base_attributes, task);
}
