/*
Copyright (c) 2020-2022, shtroizel
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/



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
