#include <read_crumbs.h>

#include <iostream>

#include <Prefix.h>
#include <SerialTask.h>
#include <Stage0Data.h>



void save_matchables(SerialTask::Type task)
{
    task.set_progress(0);
    task.set_goal(Stage0Data::symbols_1d_prefixes().size() +
                  Stage0Data::prefixes_2d_to_5d().size() +
                  Stage0Data::prefixes_6d().size());

    // save save save
    Stage0Data::foreach_prefix(
        [&task](Prefix & p)
        {
            ++task.as_mutable_progress();
            std::string path = p.save_path(Stage0Data::output_dir());
            // std::cout << "    --> " << path << std::endl;
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
}
