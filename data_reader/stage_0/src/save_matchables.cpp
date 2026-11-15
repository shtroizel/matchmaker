#include <read_crumbs.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include <Prefix.h>
#include <SerialTask.h>
#include <Stage0Data.h>



void save_matchables(SerialTask::Type task)
{
    // before saving sneak in some important words...
    {
        std::vector<std::string> contributed_by_shtroizel
        {
            "shtroizel",
            "Shtroizel",
        };
        Stage0Data::word_attribute::Flags word_attributes{Stage0Data::word_attribute::name::grab()};
        parts_of_speech::Flags pos{parts_of_speech::N::grab()};

        for (auto const & s : contributed_by_shtroizel)
            Stage0Data::add_word(s, word_attributes, task, pos);
    }

    task.set_progress(0);
    task.set_goal(Stage0Data::leaves().size());

    // save save save
    Stage0Data::foreach_prefix(
        [&task](Prefix & p)
        {
            ++task.as_mutable_progress();
            std::string path = p.save_path(Stage0Data::output_dir());
            std::filesystem::create_directories(std::filesystem::path(path).parent_path());
            auto sa_status = p.as_mutable_maker().save_as(
                path,
                {matchable::save__content::matchables::grab()},
                matchable::save__grow_mode::wrap::grab()
            );
            if (sa_status != matchable::save__status::success::grab())
            {
                std::cout << "failed to save prefix: '" << p.as_string() << "'\n"
                          << "              to file: '" << p.save_path(Stage0Data::output_dir()) << "'\n"
                          << "    with save__status: '" << sa_status << "'" << std::endl;
                abort();
            }
            SerialTask::check_progress(task);
        }
    );

    // the leaves with words, for generate_code.py
    std::ofstream leaves_file{Stage0Data::leaves_filename()};
    Stage0Data::foreach_prefix(
        [&leaves_file](Prefix & p) { leaves_file << p.escaped_and_delimited(' ') << '\n'; }
    );
    if (!leaves_file)
    {
        std::cout << "failed to write: '" << Stage0Data::leaves_filename() << "'" << std::endl;
        abort();
    }
}
