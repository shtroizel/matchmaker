#include <read_3201_acronyms.h>

#include <filesystem>

#include <SerialTask.h>
#include <Stage0Data.h>
#include <read_3201.h>



void read_3201_acronyms(SerialTask::Type task)
{
   std::filesystem::path path_3201_ACRONYMS = Stage0Data::data_dir() + "/3201/files/ACRONYMS.TXT";
   Stage0Data::word_attribute::Flags base_attributes{Stage0Data::word_attribute::acronym::grab()};
   read_3201_default(path_3201_ACRONYMS, base_attributes, task);
}
