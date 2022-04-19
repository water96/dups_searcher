#include <iostream>
#include <memory>
#include <list>
#include <algorithm>
#include <cassert>

#include "version.hpp"
#include "md5.h"
#include "searcher.h"

class AppBase{
public:
    virtual ~AppBase() = default;
    virtual bool ParseArgs(int argc, const char** argv) noexcept = 0;
    virtual int Work() noexcept = 0;
};

class App : public AppBase {
public:
    App() = default;

    bool ParseArgs(int argc, const char** argv) noexcept override {
        if (argc != 3) {
            std::cerr << "Usage: dups FILE1 FILE2\n";
            return false;
        }

        m_d1_path = argv[1];
        m_d2_path = argv[2];
        return true;
    }

    int Work() noexcept override {
        int rc = 0;

        try {
            std::cout << "Search duplicates between:\n - " << m_d1_path << "\n"
                                                          << " - " << m_d2_path << "\n";

            fl::DupsSearcher ds;

            auto d1_content = ds.GetDirectoryContent(m_d1_path);
            auto d2_content = ds.GetDirectoryContent(m_d2_path);

            auto grouped_files = ds.GroupBySize(d1_content);

            auto dups = ds.FindDuplicates(d2_content, grouped_files);

            for (const auto& p : dups) {
                std::cout << p.first << " = " << p.second << "\n";
            }
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            rc = 1;
        }

        return rc;
    }

private:
    std::string     m_d1_path{};
    std::string     m_d2_path{};
};

class AppSeveralDirs : public AppBase {
public:
    AppSeveralDirs() = default;

    bool ParseArgs(int argc, const char** argv) noexcept override {
        if (argc < 3) {
            std::cerr << "Usage: dups FILE1 FILE2 ...\n";
            return false;
        }

        m_dirs.reserve(argc - 1);
        for(auto i = 1; i < argc; ++i ){
            m_dirs.emplace_back(argv[i]);
        }

        return true;
    }

    int Work() noexcept override {
        int rc = 0;

        try {

            //get content of all dirs
            std::list<std::vector<fl::File>> content;

            auto min_size_dir = std::numeric_limits<std::size_t>::max();
            for(const auto& dir_path : m_dirs){
                auto dir_cont = m_dub_searcher.GetDirectoryContent(dir_path);
                if(dir_cont.size() < min_size_dir){
                    content.emplace_front(std::move(dir_cont));
                }
                else{
                    content.emplace_back(std::move(dir_cont));
                }

                min_size_dir = content.front().size();
            }

            //the first vect in list has the smallest len. Extract it
            m_min_dir_content = std::move(content.front());
            content.pop_front();
            //==================================

            //Create vector of maps grouped by size
            m_grouped_by_size_content.clear();
            m_grouped_by_size_content.reserve(content.size());

            for(const auto& dir_content : content){
                m_grouped_by_size_content.emplace_back(m_dub_searcher.GroupBySize(dir_content));
            }
            //=================================

            //main processing is here - determine vector of common objects between all dirs
            for(const auto& group : m_grouped_by_size_content){
                auto dups = m_dub_searcher.GetDuplicates(m_min_dir_content, group);
                m_min_dir_content = std::move(dups);
                if(m_min_dir_content.empty()){
                    break;
                }
            }
            //==========================

            PrintResults();
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            rc = 1;
        }

        return rc;
    }

    void PrintResults() const{
        for (const auto& p : m_min_dir_content) {
            std::cout << p.GetFilePath() << " =\n";

            for(const auto& group : m_grouped_by_size_content){
                const auto & vec = group.at(p.GetFileSize());
                for(const auto& fi : vec){
                    if( p == fi){
                        std::cout << "\t= " << fi.GetFilePath() << "\n";
                    }
                }
            }
        }
    }

private:
    std::vector<std::string>                        m_dirs;
    fl::DupsSearcher                                m_dub_searcher;
    std::vector<fl::File>                           m_min_dir_content;
    std::vector<fl::DupsSearcher::GroupedFiles>     m_grouped_by_size_content;
};

int main(int argc, const char** argv) {

    auto app = std::make_unique<App>();

    if(!app->ParseArgs(argc, argv)){
        return 1;
    }

    return app->Work();
}
