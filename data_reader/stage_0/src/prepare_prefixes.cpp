#include <prepare_prefixes.h>

#include <filesystem>
#include <iostream>

#include <Stage0Data.h>



bool read_prefix(
    FILE * f,
    std::array<std::string, Stage0Data::MAX_PREFIX_DEPTH> & prefix,
    SerialTask::Type type
);



void prepare_prefixes(SerialTask::Type task)
{
    std::filesystem::path prefix_file_path{Stage0Data::prefixes_filename()};
    task.set_goal(std::filesystem::file_size(prefix_file_path));
    task.set_progress(0);

    Stage0Data::prefixes_2d_to_5d().reserve(1000);
    Stage0Data::prefix_indexes_2d_to_5d().clear();
    Stage0Data::prefixes_6d().clear();
    if (Stage0Data::output_dir().empty())
        abort();

    FILE * f = fopen(prefix_file_path.c_str(), "r");
    if (nullptr == f)
    {
        perror(Stage0Data::prefixes_filename().c_str());
        abort();
    }

    std::array<std::string, Stage0Data::MAX_PREFIX_DEPTH> p;

    while (read_prefix(f, p, task))
    {
        std::string p_as_str;
        for (int i = 0; i < Stage0Data::MAX_PREFIX_DEPTH && p[i] != "nil"; ++i)
        {
            if (p[i].empty())
            {
                std::string prefix;
                for (auto const & sym : p)
                {
                    if (prefix.empty())
                        prefix += " ";
                    prefix += "'";
                    prefix += sym;
                    prefix += "'";
                }
                std::cout << "read_prefixes() --> invalid prefix: " << prefix << std::endl;
                abort();
            }

            p[i] = matchable::escapable::unescape_all(p[i]);
            p_as_str += p[i];
        }

        if (p[0] == "nil" || p[1] == "nil")
        {
            std::cout << "read_prefixes() --> invalid prefix: " << p_as_str << std::endl;
            abort();
        }

        if (p[5] == "nil")
        {
            int const index{(int) Stage0Data::prefixes_2d_to_5d().size()};
            if (!Stage0Data::prefix_indexes_2d_to_5d().emplace(p_as_str, index).second)
            {
                std::cout << "read_prefixes() --> duplicate prefix: " << p_as_str << std::endl;
                abort();
            }
        }
        else
        {
            Prefix p_6d;
            p_6d.initialize(p_as_str);
            Stage0Data::prefixes_6d().push_back(std::move(p_6d));
            continue;
        }

        // finish prefix for depths 2 to 5
        Prefix p_2_to_5d;
        p_2_to_5d.initialize(p_as_str);
        Stage0Data::prefixes_2d_to_5d().push_back(std::move(p_2_to_5d));

        SerialTask::check_progress(task);
    }

    for (auto const & [prefix, index] : Stage0Data::prefix_indexes_2d_to_5d())
    {
        for (size_t len = 2; len < prefix.size(); ++len)
        {
            if (Stage0Data::prefix_indexes_2d_to_5d().count(prefix.substr(0, len)) > 0)
            {
                std::cout << "read_prefixes() --> prefix: " << prefix
                          << " overlaps prefix: " << prefix.substr(0, len) << std::endl;
                abort();
            }
        }
    }

    if (!std::is_sorted(Stage0Data::prefixes_6d().begin(), Stage0Data::prefixes_6d().end()))
    {
        std::cout << "prefixes_6d is not sorted!" << std::endl;
        abort();
    }

    fclose(f);
}



bool read_prefix(
    FILE * f,
    std::array<std::string, Stage0Data::MAX_PREFIX_DEPTH> & prefix,
    SerialTask::Type task
)
{
    for (auto & sym : prefix)
        sym.clear();

    int ch;
    int i{0};

    while (true)
    {
        ++task.as_mutable_progress();

        ch = fgetc(f);
        if (ch == EOF)
            return false;

        if (ch == '\n')
            break;

        if (ch == ' ')
        {
            ++i;
            if (i < Stage0Data::MAX_PREFIX_DEPTH)
                continue;

            abort();
        }

        prefix[i] += (char) ch;
    }

    return true;
}
