#include "calculate_longest_words.h"

#include <algorithm>
#include <cstring>
#include <functional>
#include <iostream>
#include <queue>

#ifdef Q_ONLY
    #include <matchmaker_q/matchmaker.h>
#else
    #include <matchmaker/matchmaker.h>
#endif

#include <matchmaker/data_reader_common.h>

#include "Stage1Data.h"
#include "write_array.h"




// used for longest words calculation (priority q)
struct LengthIndex
{
    int length{0};
    int index{0};
    friend bool operator<(LengthIndex const & l, LengthIndex const & r);
};
inline bool operator<(LengthIndex const & l, LengthIndex const & r)
{
    if (l.length < r.length)
        return true;
    if (l.length > r.length)
        return false;
    return strcmp(mm_at(r.index, nullptr), mm_at(l.index, nullptr)) < 0;
}



bool calculate_longest_words(SerialTask::Type task)
{
    task.set_progress(0);
    task.set_goal(mm_count() * 2);

    std::vector<int> & by_longest = Stage1Data::nil.as_mutable_by_longest();

    std::priority_queue<
        LengthIndex,
        std::vector<LengthIndex>,
        std::less<std::vector<LengthIndex>::value_type>
    > q;
    LengthIndex cur;
    for (int i = 0; i < mm_count(); ++i)
    {
        mm_at(i, &cur.length);
        cur.index = i;
        q.push(cur);
        ++task.as_mutable_progress();
        SerialTask::check_progress(task);
    }

    // the longest terms come first, so each length's terms share a run of length indexes
    std::vector<int> as_longest(mm_count());
    std::vector<int> lengths;
    std::vector<int> length_firsts;
    std::vector<int> length_counts;
    while (!q.empty())
    {
        if (lengths.empty() || lengths.back() != q.top().length)
        {
            lengths.push_back(q.top().length);
            length_firsts.push_back((int) by_longest.size());
            length_counts.push_back(0);
        }
        ++length_counts.back();
        as_longest[q.top().index] = (int) by_longest.size();
        by_longest.push_back(q.top().index);
        q.pop();
        ++task.as_mutable_progress();
        SerialTask::check_progress(task);
    }

    // the library searches the lengths in ascending order
    std::reverse(lengths.begin(), lengths.end());
    std::reverse(length_firsts.begin(), length_firsts.end());
    std::reverse(length_counts.begin(), length_counts.end());

    return write_array("from_longest", by_longest) &&
           write_array("as_longest", as_longest) &&
           write_array("lengths", lengths) &&
           write_array("length_firsts", length_firsts) &&
           write_array("length_counts", length_counts);
}
