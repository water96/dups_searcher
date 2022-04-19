#ifndef __SEARCHER_H__
#define __SEARCHER_H__

#include <vector>
#include <unordered_map>

#include "file.h"

namespace fl {

class DupsSearcher {
public:
    using TheSamePair = std::pair<std::string, std::string>;
    using Result = std::pair<std::vector<TheSamePair>, bool>;
    using GroupedFiles = std::unordered_map<std::size_t, std::vector<fl::File>>;

    DupsSearcher() = default;

    std::vector<fl::File> GetDirectoryContent(const std::string& dir_path);

    GroupedFiles GroupBySize(const std::vector<fl::File>& content);
    std::vector<TheSamePair> FindDuplicates(const std::vector<fl::File>& content, const GroupedFiles& grouped);
    std::vector<fl::File> GetDuplicates(const std::vector<fl::File>& content, const DupsSearcher::GroupedFiles& grouped);

private:

};

}

#endif // ! __SEARCHER_H__
