#pragma once

#include <memory>
#include <string>
#include <matchable/MatchableMaker.h>



class Prefix
{
public:
    Prefix();
    ~Prefix();

    Prefix(Prefix const &) = delete;
    Prefix(Prefix && other);
    void initialize(std::string const & prefix);

    Prefix & operator=(Prefix const &) = delete;
    Prefix & operator=(Prefix && other);

    bool operator<(Prefix const & other) const;

    matchable::MatchableMaker const & as_maker() const;
    matchable::MatchableMaker & as_mutable_maker();

    std::string const & as_string() const;
    std::string & as_mutable_string();

    std::string escaped_and_delimited(char delim) const;
    std::string save_path(std::string const & OUTPUT_DIR) const;

private:
    std::unique_ptr<matchable::MatchableMaker> maker;
    std::string str;
};
