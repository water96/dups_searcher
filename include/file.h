#ifndef __FILE_H__
#define __FILE_H__


#include <filesystem>
#include <string>

namespace fl {

class File
{
public:
    File() = default;

    File(File&& f);
    File& operator=(File&& f);

    File(const File& f) = default;
    File& operator=(const File& f) = default;

    explicit File(std::string str);
    std::size_t GetFileSize() const;
    std::string GetHashSum() const;
    bool IsOk() const;
    const std::string& GetFilePath() const {
        return m_file_path;
    }

private:
    std::string                 m_file_path;
    std::size_t                 m_file_size{ 0 };
    mutable std::string         m_hash_val;
    mutable bool                m_is_valid{ false };
};

bool operator==(const File& f1, const File& f2);

}

#endif // ! __FILE_H__
