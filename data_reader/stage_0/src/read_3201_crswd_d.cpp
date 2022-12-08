#include <read_3201_crswd_d.h>

#include <Stage0Data.h>
#include <read_3201.h>



void read_3201_crswd_d(SerialTask::Type task)
{
   std::filesystem::path const path_3201_CRSWD_D{Stage0Data::data_dir() + "/3201/files/CRSWD-D.TXT"};
   Stage0Data::word_attribute::Flags base_attributes;
   read_3201_default(path_3201_CRSWD_D, base_attributes, task);
}
