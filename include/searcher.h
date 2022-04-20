#ifndef __SEARCHER_H__
#define __SEARCHER_H__

#include <vector>
#include <unordered_map>

#include "file.h"

namespace fl {

/*
    Just collection of usefull methods
*/
class DupsSearcher {
public:
    //alias for pair of two pathes
    using TheSameFailsName = std::pair<std::string, std::string>;

    //alias for data type - table for relation "size" -> "files of this size"
    using GroupedFiles = std::unordered_map<std::size_t, std::vector<fl::File>>;

    DupsSearcher() = default;

    //List of valid files from specified directory
    std::vector<fl::File> GetDirectoryContent(const std::string& dir_path);

    //group list of files by their sizes
    GroupedFiles GroupBySize(const std::vector<fl::File>& content);

    //Create list of pairs of names identical files
    //Use list of files from one directory and files grouped by size from another
    std::vector<TheSameFailsName> GetDuplicatedPairs(const std::vector<fl::File>& content, const GroupedFiles& grouped);

    //Do the same but return just list of files from the first directory that has duplicates in grouped files
    std::vector<fl::File> GetDuplicatedFiles(const std::vector<fl::File>& content, const GroupedFiles& grouped);

private:

};

}

#endif // ! __SEARCHER_H__
