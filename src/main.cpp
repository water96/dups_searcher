#include <iostream>

#include "version.hpp"
#include "md5.h"
#include "searcher.h"

class App {
public:
    App() = default;
    virtual ~App() = default;

    bool ParseArgs(int argc, const char** argv) noexcept {
        if (argc != 3) {
            std::cerr << "Usage: dups FILE1 FILE2\n";
            return false;
        }

        m_d1_path = argv[1];
        m_d2_path = argv[2];
        return true;
    }

    int Work() noexcept {
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

int main(int argc, const char** argv) { 
    App app;

    if(!app.ParseArgs(argc, argv)){
        return 1;
    }

    return app.Work();
}
