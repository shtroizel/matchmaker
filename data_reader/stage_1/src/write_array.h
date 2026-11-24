#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Stage1Data.h"



// writes one of the arrays the library compiles in: its ints as text, each followed by a comma
inline bool write_array(std::string const & name, std::vector<int> const & values)
{
    std::filesystem::path const dir{Stage1Data::nil.as_workspace_dir() + "/generated_src/arrays"};
    std::filesystem::create_directories(dir);
    std::filesystem::path const path{dir / (name + ".inc")};
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
