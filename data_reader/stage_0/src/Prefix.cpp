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



#include <Prefix.h>

#include <iostream>
#include <memory>
#include <string>

#include <matchable/MatchableMaker.h>

#include <Stage0Data.h>



Prefix::Prefix() : maker{new matchable::MatchableMaker()}
{
}



Prefix::~Prefix()
{
}



Prefix::Prefix(Prefix && other) : maker{std::move(other.maker)}, str{std::move(other.str)}
{
}



Prefix & Prefix::operator=(Prefix && other)
{
    if (this != &other)
    {
        str = std::move(other.str);
        maker = std::move(other.maker);
    }
    return *this;
}



bool Prefix::operator<(Prefix const & other) const
{
    return str  < other.str;
}



void Prefix::initialize(std::string const & prefix)
{
    str = prefix;
    std::string const matchable_name = "word_" + escaped_and_delimited('_');
    maker->grab(matchable_name)->add_property("int8_t", "pos");
    maker->grab(matchable_name)->add_property("int", "syn");
    maker->grab(matchable_name)->add_property("int", "ant");
    maker->grab(matchable_name)->add_property("int", "by_longest_index");
    maker->grab(matchable_name)->add_property("int", "ordinal_summation");
    maker->grab(matchable_name)->add_property("int", "embedded");
    maker->grab(matchable_name)->add_property("int", "book_index");
    maker->grab(matchable_name)->add_property("int", "chapter_index");
    maker->grab(matchable_name)->add_property("int", "paragraph_index");
    maker->grab(matchable_name)->add_property("int", "word_index");

    // "word_attribute" properties
    {
        auto add_bool_prop =
            [&](std::string const & prop)
            {
                std::string const prop_name = "is_" + prop;
                maker->grab(matchable_name)->add_property("int8_t", prop_name);
            };

        add_bool_prop(Stage0Data::word_attribute::name::grab().as_string());
        add_bool_prop(Stage0Data::word_attribute::male_name::grab().as_string());
        add_bool_prop(Stage0Data::word_attribute::female_name::grab().as_string());
        add_bool_prop(Stage0Data::word_attribute::place::grab().as_string());
        add_bool_prop(Stage0Data::word_attribute::compound::grab().as_string());
        add_bool_prop(Stage0Data::word_attribute::acronym::grab().as_string());
        add_bool_prop(Stage0Data::word_attribute::phrase::grab().as_string());
        add_bool_prop("used_in_book");
    }
}



matchable::MatchableMaker const & Prefix::as_maker() const
{
    return *maker;
}



matchable::MatchableMaker & Prefix::as_mutable_maker()
{
    return *maker;
}



std::string const & Prefix::as_string() const
{
    return str;
}



std::string & Prefix::as_mutable_string()
{
    return str;
}



std::string Prefix::escaped_and_delimited(char delim) const
{
    std::string ret;

    if (str.empty())
    {
        std::cout << "Prefix::escaped_and_delimited('"
                    << delim << "') --> str is empty!" << std::endl;
        abort();
    }

    for (size_t i = 0; i < str.size(); ++i)
    {
        std::string escaped = matchable::escapable::escape(str.substr(i, 1));
        if (escaped.size() > 2)
        {
            if (escaped[0] != escaped[escaped.size() - 1])
            {
                std::cout << "Prefix::escaped_and_delimited('" << delim << "')\n"
                            << "    --> escaped.size() > 2 but first and last do not match!\n"
                            << "    --> str is: '" << str << "'" << std::endl;
                abort();
            }
        }
        else if (escaped.size() != 1)
        {
            std::cout << "Prefix::escaped_and_delimited('" << delim << "')\n"
                    << "    --> escape failed with: '" << escaped << "'\n"
                    << "    -->             str is: '" << str << "'" << std::endl;
            abort();
        }

        if (i > 0)
            ret += delim;

        // manually explicitly escape numbers since matchable escapes numbers to themselves.
        if (str[i] >= '0' && str[i] <= '9')
            ret += "esc_";
        ret += escaped;
    }

    return ret;
}



std::string Prefix::save_path(std::string const & OUTPUT_DIR) const
{
    if (str.empty())
    {
        std::cout << "Prefix::save_path() called but str is empty!" << std::endl;
        abort();
    }

    std::string ret = OUTPUT_DIR;
    ret += "/";
    ret += escaped_and_delimited('/');
    ret += "/";
    ret += escaped_and_delimited('_');
    ret += ".h";

    return ret;
}
