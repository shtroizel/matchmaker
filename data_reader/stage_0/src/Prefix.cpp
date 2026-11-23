#include <Prefix.h>

#include <iostream>
#include <memory>
#include <string>

#include <matchable/MatchableMaker.h>
#include <matchmaker/word_order.h>

#include <Stage0Data.h>
#include <routing.h>



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
    return mm_word_less(str, other.str);
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
    maker->grab(matchable_name)->add_property("int", "definition");
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
    return routing::symbols(str, delim);
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
