#include <prepare_prefixes.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include <Stage0Data.h>
#include <routing.h>



void prepare_prefixes(SerialTask::Type task)
{
    std::filesystem::path parents_path{Stage0Data::parents_filename()};
    std::ifstream f{parents_path};
    if (!f || Stage0Data::output_dir().empty())
    {
        std::cout << "prepare_prefixes() --> cannot read: " << parents_path << std::endl;
        abort();
    }
    task.set_goal(std::filesystem::file_size(parents_path));
    task.set_progress(0);

    Stage0Data::parent_prefixes() = {""};
    Stage0Data::leaves().clear();

    for (std::string line; std::getline(f, line);)
    {
        task.as_mutable_progress() += line.size() + 1;

        std::string prefix;
        if (!routing::parent_of_line(line, prefix))
        {
            std::cout << "prepare_prefixes() --> invalid symbol: '" << prefix << "'" << std::endl;
            abort();
        }
        if (!prefix.empty())
            Stage0Data::parent_prefixes().insert(prefix);

        SerialTask::check_progress(task);
    }

    auto const & parents = Stage0Data::parent_prefixes();
    for (auto const & prefix : parents)
    {
        if (!prefix.empty() && parents.count(prefix.substr(0, prefix.size() - 1)) == 0)
        {
            std::cout << "prepare_prefixes() --> parent prefix: '" << prefix << "' lacks its parent"
                      << std::endl;
            abort();
        }
    }
}
