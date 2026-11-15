#include <prepare_prefixes.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include <Stage0Data.h>



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

        std::istringstream symbols{line};
        std::string prefix;
        for (std::string symbol; symbols >> symbol;)
        {
            std::string const ch = matchable::escapable::unescape_all(symbol);
            if (ch.size() != 1)
            {
                std::cout << "prepare_prefixes() --> invalid symbol: '" << symbol << "'" << std::endl;
                abort();
            }
            prefix += ch;
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
