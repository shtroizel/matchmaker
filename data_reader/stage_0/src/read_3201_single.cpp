#include <read_3201_single.h>

#include <filesystem>

#include <Stage0Data.h>
#include <read_3201.h>



void read_3201_single(SerialTask::Type task)
{
    std::filesystem::path const path_3201_SINGLE{Stage0Data::data_dir() + "/3201/files/SINGLE.TXT"};
    Stage0Data::word_attribute::Flags base_attributes;
    read_3201_default(path_3201_SINGLE, base_attributes, task);
}
