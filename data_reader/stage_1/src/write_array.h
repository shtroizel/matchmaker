#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Stage1Data.h"



inline std::filesystem::path arrays_dir()
{
    return std::filesystem::path{Stage1Data::nil.as_workspace_dir() + "/generated_src/arrays"};
}


// writes one of the arrays the library compiles in whole: its ints as text, each followed by a comma
inline bool write_array(std::string const & name, std::vector<int> const & values)
{
    std::filesystem::create_directories(arrays_dir());
    std::filesystem::path const path{arrays_dir() / (name + ".inc")};
    std::ofstream f{path};
    for (int value : values)
        f << value << ",\n";
    f.close();
    if (!f)
    {
        std::cout << "failed to write " << path << std::endl;
        return false;
    }
    return true;
}


// writes one of the arrays the library compiles in units: each unit a source file of its own that
// defines the array of ints <name>_<unit>, and the list that names the units in order
inline bool write_units(std::string const & name, std::vector<std::vector<int>> const & units)
{
    std::filesystem::create_directories(arrays_dir());
    std::filesystem::path const list_path{arrays_dir() / (name + ".inc")};
    std::ofstream list{list_path};
    for (size_t unit = 0; unit < units.size(); ++unit)
    {
        std::string const unit_name{name + "_" + std::to_string(unit)};
        list << "MM_UNIT(" << unit_name << ")\n";

        std::filesystem::path const path{arrays_dir() / (unit_name + ".cpp")};
        std::ofstream f{path};
        f << "namespace arrays { extern int const " << unit_name << "[]; }\n"
          << "int const arrays::" << unit_name << "[] = {\n";
        for (int value : units[unit])
            f << value << ",\n";
        f << "};\n";
        f.close();
        if (!f)
        {
            std::cout << "failed to write " << path << std::endl;
            return false;
        }
    }
    list.close();
    if (!list)
    {
        std::cout << "failed to write " << list_path << std::endl;
        return false;
    }
    return true;
}


// writes an array that is read an entry at a time, in units of 1 << shift entries
inline bool write_units(std::string const & name, std::vector<int> const & values, int shift)
{
    std::vector<std::vector<int>> units;
    for (size_t first = 0; first < values.size(); first += size_t{1} << shift)
        units.emplace_back(values.begin() + first,
                           values.begin() + std::min(values.size(), first + (size_t{1} << shift)));
    return write_units(name, units);
}
