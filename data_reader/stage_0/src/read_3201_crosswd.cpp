#include <read_3201_crosswd.h>

#include <filesystem>

#include <Stage0Data.h>
#include <read_3201.h>



void read_3201_crosswd(SerialTask::Type task)
{
   std::filesystem::path const path_3201_CROSSWD{Stage0Data::data_dir() + "/3201/files/CROSSWD.TXT"};
   Stage0Data::word_attribute::Flags base_attributes;
   read_3201_default(path_3201_CROSSWD, base_attributes, task);
}
