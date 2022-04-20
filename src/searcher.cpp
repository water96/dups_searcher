#include <filesystem>

#include "searcher.h"


namespace fl {

std::vector<fl::File> DupsSearcher::GetDirectoryContent(const std::string& dir_path) {

    std::filesystem::directory_iterator dir_iter{ dir_path };
    std::vector<File> regular_files;    //hope on rvo

    //it is better to make reserve of vector before filling it
    //count:
    auto valid_file_num = std::count_if(std::filesystem::begin(dir_iter),
                                        std::filesystem::end(dir_iter),
                                        [](const std::filesystem::directory_entry& de) {
                                            return fl::File::FileIsOk(de.path().string());
                                        });

    regular_files.reserve(valid_file_num);

    dir_iter = std::filesystem::directory_iterator{ dir_path };
    for (const auto& de : dir_iter) {
        fl::File fi(de.path().string());
        if (fi.IsOk()) {
            regular_files.emplace_back(std::move(fi));
        }
    }

    return regular_files;
}

DupsSearcher::GroupedFiles DupsSearcher::GroupBySize(const std::vector<fl::File>& content) {
    GroupedFiles mp;
    for (const auto& f : content) {
        mp[f.GetFileSize()].push_back(f);
    }
    return mp;
}

std::vector<DupsSearcher::TheSameFailsName> DupsSearcher::GetDuplicatedPairs(const std::vector<fl::File>& content, const DupsSearcher::GroupedFiles& grouped) {

    std::vector<TheSameFailsName> res_pairs;
    //===========================================================================
    for (const auto& fi : content) {
        //check hash table has such size
        auto fit = grouped.find(fi.GetFileSize());
        if (fit == grouped.end()) {
            continue;
        }

        //it has, fill pair
        for (const auto& p : fit->second) {
            if (fi == p) {
                res_pairs.emplace_back(std::make_pair(fi.GetFilePath(), p.GetFilePath()));
            }
        }
    }

    return res_pairs;
}

std::vector<fl::File> DupsSearcher::GetDuplicatedFiles(const std::vector<fl::File>& content, const DupsSearcher::GroupedFiles& grouped) {

    std::vector<fl::File> res;
    res.reserve(content.size());
    //===========================================================================
    for (const auto& fi : content) {
        auto fit = grouped.find(fi.GetFileSize());
        if (fit == grouped.end()) {
            continue;
        }

        for (const auto& p : fit->second) {
            if (fi == p) {
                res.push_back(fi);
                break;
            }
        }
    }

    return res;
}


}
