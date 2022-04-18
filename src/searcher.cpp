#include "searcher.h"


namespace fl {

std::vector<fl::File> DupsSearcher::GetDirectoryContent(const std::string& dir_path) {

    std::filesystem::directory_iterator dir_iter{ dir_path };
    std::vector<File> regular_files;

    for (const auto& de : dir_iter) {
        fl::File fi(de.path().string());
        if (fi.IsOk() /*&&
            fi.GetFileSize()*/) {
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

std::vector<DupsSearcher::TheSamePair> DupsSearcher::FindDuplicates(const std::vector<fl::File>& content, const DupsSearcher::GroupedFiles& grouped) {

    std::vector<TheSamePair> res_pairs;
    //===========================================================================
    for (const auto& fi : content) {
        auto fit = grouped.find(fi.GetFileSize());
        if (fit == grouped.end()) {
            continue;
        }

        for (const auto& p : fit->second) {
            if (fi == p) {
                res_pairs.emplace_back(std::make_pair(fi.GetFilePath(), p.GetFilePath()));
            }
        }
    }

    return res_pairs;
}


}
