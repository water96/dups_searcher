#ifndef __FILE_H__
#define __FILE_H__


#include <string>

namespace fl {

//represents a regular file in file system
class File
{
public:
    File() = default;   //default for containers
    //one file - one file path
    explicit File(const std::string& str);

    File(File&& f);
    File& operator=(File&& f);

    File(const File& f) = default;
    File& operator=(const File& f) = default;

    //size of file
    std::size_t GetFileSize() const;
    //hash sum
    const std::string& GetHashSum() const;
    //check object is valid. If not other methods return invalid values
    //Object can be not valid just after construction (if file path is wrong or it is not file)
    //or when we try to calc hash.
    bool IsOk() const;

    const std::string& GetFilePath() const {
        return m_file_path;
    }

    //static for fast checking file is valid regular file
    static bool FileIsOk(const std::string& file_path);

private:
    std::string                 m_file_path;
    std::size_t                 m_file_size{ 0 };
    //mutable in order to calc hash in case if really neccessary
    mutable std::string         m_hash_val;
    mutable bool                m_is_valid{ false };
};

//custom comparison
bool operator==(const File& f1, const File& f2);

}

#endif // ! __FILE_H__
