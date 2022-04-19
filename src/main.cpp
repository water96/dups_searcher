#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <array>
#include <map>

#include "version.hpp"
#include "md5.h"


class FileIdentity
{
public:
    FileIdentity() = default;

    template <typename T>
    explicit FileIdentity(T&& str) : m_file_path(std::forward<T>(str)) {
        std::error_code ec;
        m_is_reg_file = std::filesystem::is_regular_file(m_file_path, ec);
    }

    std::size_t GetFileSize() const {
        if (m_file_size != 0) {
            return m_file_size;
        }

        std::error_code ec;
        m_file_size = std::filesystem::file_size(m_file_path, ec);
        return m_file_size;
    }

    std::string GetHash() const {
        if (!m_hash_val.empty()) {
            return m_hash_val;
        }

        std::ifstream ifs(m_file_path, std::ios_base::binary);

        if (!ifs.is_open()) {
            m_hash_val.clear();
            m_is_valid = false;
            return m_hash_val;
        }

        MD5 md5;
        static constexpr auto CHUNK_SIZE = MD5::BlockSize;
        std::array<char, CHUNK_SIZE> buffer{ 0 };
        size_t bytes_red = 0u;
        while (!ifs.eof()) {
            ifs.read(buffer.data(), CHUNK_SIZE);
            auto N = static_cast<std::size_t>(ifs.gcount());
            bytes_red += N;

            md5.add(buffer.data(), N);
        }

        if (bytes_red == GetFileSize()) {
            //it is ok
            m_hash_val = md5.getHash();
            m_is_valid = true;
        }

        return m_hash_val;
    }

    bool IsValid() const {
        return m_is_valid;
    }

    bool IsRegular() const {
        return m_is_reg_file;
    }

    const std::string& GetFilePath() const {
        return m_file_path;
    }

private:
    std::string                 m_file_path;
    mutable std::string         m_hash_val;
    mutable std::size_t         m_file_size{ 0 };
    mutable bool                m_is_valid{ false };
    bool                        m_is_reg_file{ false };
};

bool operator==(const FileIdentity& f1, const FileIdentity& f2) {

    if (!f1.IsRegular() ||
        !f2.IsRegular()) {
        return false;
    }

    if (f1.GetFileSize() != f2.GetFileSize()) {
        return false;
    }

    if (f1.GetHash() != f2.GetHash()) {
        return false;
    }

    return f1.IsValid() && f2.IsValid();
}

class DupsFinder {
public:
    using TheSamePair = std::pair<std::string, std::string>;
    using Result = std::pair<std::vector<TheSamePair>, bool>;

    DupsFinder() = default;

    template <typename Td1, typename Td2>
    Result Find(Td1&& d1_path, Td2&& d2_path) {
        std::filesystem::directory_entry pd1{ std::forward<Td1>(d1_path) };
        std::filesystem::directory_entry pd2{ std::forward<Td2>(d2_path) };

        if (!pd1.is_directory() || !pd2.is_directory()) {
            return Result{ {}, false };
        }

        auto d1_files = _get_directory_content(pd1);
        auto d2_files = _get_directory_content(pd2);

        if (std::filesystem::equivalent(pd1.path(), pd2.path())) {
            std::cout << "Directories are the same\n";

            if (d1_files.size() != d2_files.size()) {
                return Result{ {}, false };
            }

            auto N = d1_files.size();
            std::vector<TheSamePair> res;
            res.reserve(N);
            for (auto i = 0u; i < N; ++i) {
                res.emplace_back(std::make_pair(d1_files[i].GetFilePath(), d2_files[i].GetFilePath()));
            }

            return Result{ std::move(res), true };
        }

        //===========================================================================
        //read one directory
        auto mpp = _get_files_map(d1_files);

        std::vector<TheSamePair> res_pairs;
        //===========================================================================
        for (const auto& fi : d2_files) {
            auto fit = mpp.find(fi.GetFileSize());
            if (fit == mpp.end()) {
                continue;
            }

            std::cout << "Find the same size " << fit->first << " of " << fit->second.size() << " files\n";
            for (const auto& p : fit->second) {
                auto res = !std::filesystem::equivalent(p.GetFilePath(), fi.GetFilePath()) &&
                           (fi == p);
                std::cout << "Check equality " << fi.GetFilePath() << " == " << p.GetFilePath() << " ? " << res << "\n";
                if (res) {
                    std::cout << "Hash: " << fi.GetHash() << "\n";
                    res_pairs.emplace_back(std::make_pair(fi.GetFilePath(), p.GetFilePath()));
                }
            }
        }

        return Result{ std::move(res_pairs), true};
    }

private:
    std::vector<FileIdentity> _get_directory_content(
        const std::filesystem::directory_entry& dir) const {
        std::filesystem::recursive_directory_iterator dir_iter{ dir };

        std::cout << "Content of directory " << dir.path() << ":\n";
        auto regular_files_cnter = std::count_if(
            std::filesystem::begin(dir_iter), std::filesystem::end(dir_iter),
            [](const auto& de) {
                std::cout << "directory entry: " << de.path()
                    << ", is regulart file: " << de.is_regular_file() << "\n";
                return de.is_regular_file();
            });

        std::cout << "End\n\n";

        std::vector<FileIdentity> regular_files;
        regular_files.reserve(static_cast<std::size_t>(regular_files_cnter));

        for (const auto& de : std::filesystem::recursive_directory_iterator{ dir }) {
            FileIdentity fi(de.path().string());
            if (fi.IsRegular() &&
                fi.GetFileSize()) {
                regular_files.emplace_back(std::move(fi));
            }
        }

        return regular_files;
    }

    std::unordered_map<std::size_t, std::vector<FileIdentity>>
        _get_files_map(const std::vector<FileIdentity>& files) {

        std::unordered_map<std::size_t, std::vector<FileIdentity>> mp;
        for (const auto& f : files) {
            mp[f.GetFileSize()].push_back(f);
        }
        return mp;
    }
};

#include "file.h"

void test() {
    fl::File f1("Dups.exe");
    fl::File f2("Dups_copy.pdb");

    auto res = f1.IsOk();
    res = f2.IsOk();
    res = (f1 == f2);

    f1 = fl::File{ "Dups_copy1.pdb" };
    f2 = fl::File{ "Dups_copy.pdb" };

    res = f1.IsOk();
    res = f2.IsOk();
    res = (f1 == f2);


    f1 = fl::File{ "test" };
    f2 = fl::File{ "test_copy" };

    res = f1.IsOk();
    res = f2.IsOk();
    res = (f1 == f2);
    std::cout << res << "\n";
}

int main(int argc, const char** argv) {
    std::cout << "\n=============================================\n";
    std::cout << "App starts";
    std::cout << "\n=============================================\n";

    auto rc = 0;

    std::string d1 = "D:\\work";
    std::string d2 = "D:\\work\\voip";

    if (argc != 3) {
        std::cout << "Wrong arguments\n";
    }
    else {
        d1 = argv[1];
        d2 = argv[2];
    }

    try {

        test();

        DupsFinder df;
        auto [dups, res] = df.Find(d1, d2);

        if (res) {
            std::cout << "Duplicates:\n";

            for (const auto& p : dups) {
                std::cout << p.first << " = " << p.second << "\n";
            }
        }   

        rc = !res;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in main: " << e.what() << '\n';
        rc = 1;
    }

    std::cout << "\n=============================================\n";
    std::cout << "App ends";
    std::cout << "\n=============================================\n";

    return rc;
}
