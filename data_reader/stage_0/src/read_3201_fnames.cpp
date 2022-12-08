#include <read_3201_fnames.h>

#include <filesystem>

#include <Stage0Data.h>
#include <read_3201.h>



void read_3201_fnames(SerialTask::Type task)
{
   std::filesystem::path const path_3201_NAMES_F{Stage0Data::data_dir() + "/3201/files/NAMES-F.TXT"};
   Stage0Data::word_attribute::Flags base_attributes{
         Stage0Data::word_attribute::name::grab(),
         Stage0Data::word_attribute::female_name::grab()
   };
   read_3201_default(path_3201_NAMES_F, base_attributes, task);
}
