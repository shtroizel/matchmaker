#include <string>
#include <vector>

#include <matchable/matchable.h>


MATCHABLE(
    q_usage,
    included,
    omitted,
    only
)


struct QFile
{
    std::filesystem::directory_entry entry;
    friend bool operator<(QFile const & l, QFile const & r);
};
inline bool operator<(QFile const & l, QFile const & r)
{
    return r.entry.path().filename() < l.entry.path().filename();
}
