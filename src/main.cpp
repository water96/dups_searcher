#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "version.hpp"

class FileWrapper {
 public:
  template <typename T>
  bool Set(T&& str) {
    m_hash_val = 0;
    m_file_path = std::forward<T>(str);

    std::ifstream ifs(m_file_path, std::ios_base::binary);

    if (!ifs.is_open()) {
      m_file_path.clear();
      return false;
    }

    std::hash<char> hs;
    while (!ifs.eof()) {
      char byte;
      ifs.read(&byte, 1);

      m_hash_val ^= (hs(byte) << 1);  // or use boost::hash_combine
    }

    return false;
  }

  const std::string& GetFilePath() const { return m_file_path; }

  std::size_t GetHashValue() const { return m_hash_val; }

 private:
  std::string m_file_path;
  std::size_t m_hash_val{0};
};

bool operator==(const FileWrapper& f1, const FileWrapper& f2) {
  std::ifstream if1(f1.GetFilePath());
  std::ifstream if2(f2.GetFilePath());

  if (!if1.is_open() || !if2.is_open()) {
    return false;
  }

  while (!if1.eof() && !if2.eof()) {
    char byte1;
    char byte2;
    if1.read(&byte1, 1);
    if2.read(&byte2, 1);

    if (byte1 != byte2) {
      return false;
    }
  }

  return if1.eof() && if2.eof();
}

struct MyHash {
  std::size_t operator()(const FileWrapper& f) const noexcept {
    return f.GetHashValue();  // or use boost::hash_combine
  }
};

class DupsFinder {
 public:
  using TheSamePair = std::pair<std::string, std::string>;

  using Result = std::pair<std::vector<TheSamePair>, bool>;

  DupsFinder() = default;

  template <typename Td1, typename Td2>
  Result Find(Td1&& d1_path, Td2&& d2_path) {
    std::filesystem::directory_entry pd1{std::forward<Td1>(d1_path)};
    std::filesystem::directory_entry pd2{std::forward<Td2>(d2_path)};

    if (!pd1.is_directory() || !pd2.is_directory()) {
      return Result{{}, false};
    }

    auto d1_files = _get_directory_content(pd1);
    auto d2_files = _get_directory_content(pd2);

    if (std::filesystem::equivalent(pd1.path(), pd2.path())) {
      std::cout << "Directories are the same\n";

      if (d1_files.size() != d2_files.size()) {
        return Result{{}, false};
      }

      auto N = d1_files.size();
      std::vector<TheSamePair> res;
      res.reserve(N);
      for (auto i = 0u; i < N; ++i) {
        res.emplace_back(std::make_pair(d1_files[i], d2_files[i]));
      }

      return Result{std::move(res), true};
    }


    std::unordered_map<FileWrapper, std::vector<std::string>, MyHash> mp;

    return Result{{}, true};
  }

 private:
  std::vector<std::filesystem::path> _get_directory_content(
      const std::filesystem::directory_entry& dir) const {
    std::filesystem::directory_iterator dir_iter{dir};

    std::cout << "Content of directory " << dir.path() << ":\n";
    auto regular_files_cnter = std::count_if(
        std::filesystem::begin(dir_iter), std::filesystem::end(dir_iter),
        [](const auto& de) {
          std::cout << "directory entry: " << de.path()
                    << ", is regulart file: " << de.is_regular_file() << "\n";
          return de.is_regular_file();
        });

    std::cout << "End\n\n";

    std::vector<std::filesystem::path> regular_files;
    regular_files.reserve(regular_files_cnter);

    for (const auto& de : std::filesystem::directory_iterator{dir}) {
      if (de.is_regular_file()) {
        regular_files.push_back(de.path());
      }
    }

    return regular_files;
  }
};

int main(int argc, const char** argv) {
  std::cout << "\n=============================================\n";
  std::cout << "Program starts";
  std::cout << "\n=============================================\n";

  auto rc = 0;

  try {
    /* code */
    DupsFinder df;

    auto [dups, res] = df.Find("/home/aleksey/work/duplicates",
                               "/home/aleksey/work/duplicates");

    if (res) {
      std::cout << "Duplicates:\n";

      for (const auto& p : dups) {
        std::cout << p.first << " = " << p.second << "\n";
      }
    }

    rc = !res;
  } catch (const std::exception& e) {
    std::cerr << "Exception in main: " << e.what() << '\n';
    rc = 1;
  }

  std::cout << "\n=============================================\n";
  std::cout << "Program ends";
  std::cout << "\n=============================================\n";

  return rc;
}
