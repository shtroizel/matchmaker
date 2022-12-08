#include <read_3201_places.h>

#include <filesystem>

#include <Stage0Data.h>
#include <read_3201.h>



void read_3201_places(SerialTask::Type task)
{
   std::filesystem::path const path_3201_PLACES{Stage0Data::data_dir() + "/3201/files/PLACES.TXT"};
   Stage0Data::word_attribute::Flags base_attributes{Stage0Data::word_attribute::place::grab()};
   read_3201_default(path_3201_PLACES, base_attributes, task);
}
