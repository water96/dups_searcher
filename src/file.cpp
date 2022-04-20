#include <fstream>
#include <array>
#include <filesystem>

#include "file.h"

//use md5 for hash
#include "md5.h"

namespace fl {

bool File::FileIsOk(const std::string& file_path) {
    std::error_code ec;
    auto res = std::filesystem::is_regular_file(file_path, ec);
    return !ec && res;
}

File::File(const std::string& str) : m_file_path(str) {
    m_is_valid = FileIsOk(m_file_path);
    if (m_is_valid) {
        //it is ok, get size from file system
        std::error_code ec;
        m_file_size = std::filesystem::file_size(m_file_path, ec);
        if (ec) {
            //object not valid
            m_is_valid = false;
        }
    }
}

//custom move in order to make source object invalid after moving
File::File(File&& f) : m_file_path(std::move(f.m_file_path)),
                       m_file_size(f.m_file_size),
                       m_hash_val(std::move(f.m_hash_val)),
                       m_is_valid(f.m_is_valid) {

    f.m_file_size = 0;
    f.m_is_valid = false;
}

File& File::operator=(File&& f) {
    m_file_path = std::move(f.m_file_path);
    m_file_size = f.m_file_size;
    m_hash_val = std::move(f.m_hash_val);
    m_is_valid = f.m_is_valid;

    f.m_file_size = 0;
    f.m_is_valid = 0;

    return *this;
}

std::size_t File::GetFileSize() const {
    return m_file_size;
}

const std::string& File::GetHashSum() const {
    if (!m_hash_val.empty() ||
        !m_is_valid) {
        return m_hash_val;
    }
    //here everithing is ok. Hash is not calculate yet.

    std::ifstream ifs(m_file_path, std::ios_base::binary);
    if (!ifs.is_open()) {
        m_hash_val.clear();
        m_is_valid = false;
        return m_hash_val;
    }

    MD5 md5;
    static constexpr auto CHUNK_SIZE = MD5::BlockSize;
    std::array<char, CHUNK_SIZE> buffer{ 0 };
    size_t bytes_red = 0u;  //count total read bytes
    while (!ifs.eof()) {
        ifs.read(buffer.data(), CHUNK_SIZE);
        auto N = static_cast<std::size_t>(ifs.gcount());
        bytes_red += N;

        md5.add(buffer.data(), N);
    }

    //check that size from file system size counted during hash calculation is the same
    if (bytes_red == GetFileSize()) {
        //it is ok
        m_hash_val = md5.getHash();
        m_is_valid = true;
    }
    else {
        m_is_valid = false;
    }

    return m_hash_val;
}

bool File::IsOk() const {
    return m_is_valid;
}


//check that files, represented by this File objects are the same
bool operator==(const File& f1, const File& f2) {

    //check equivalence (trust file system module)
    if (!std::filesystem::equivalent(f1.GetFilePath(),
                                     f2.GetFilePath())) {

        // if not - files are not the same if fs

        //the first check size of files
        if (f1.GetFileSize() != f2.GetFileSize()) {
            return false;
        }
        //else - ok, sizes are the same

        //then check hash sum
        if (f1.GetHashSum() != f2.GetHashSum()) {
            return false;
        }
        //else ok
    }

    //in the end we should check the validity of both files.
    //Files are the same if:
    //  - have the same size;
    //  - have the same hash
    //  - both are valid
    return f1.IsOk() && f2.IsOk();
}

}