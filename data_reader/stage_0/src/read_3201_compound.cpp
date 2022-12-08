#include <read_3201_compound.h>

#include <filesystem>

#include <Stage0Data.h>
#include <read_3201.h>



void read_3201_compound(SerialTask::Type task)
{
   std::filesystem::path const path_3201_COMPOUND{Stage0Data::data_dir() + "/3201/files/COMPOUND.TXT"};
   Stage0Data::word_attribute::Flags base_attributes{Stage0Data::word_attribute::compound::grab()};
   read_3201_default(path_3201_COMPOUND, base_attributes, task);
}
