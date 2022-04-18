#include <fstream>
#include <array>

#include "file.h"
#include "md5.h"

namespace fl {


std::size_t File::GetFileSize() const {
    return m_file_size;
}

std::string File::GetHashSum() const {
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

bool File::IsOk() const {
    return m_is_valid;
}


bool operator==(const File& f1, const File& f2) {

    if (!std::filesystem::equivalent(f1.GetFilePath(),
                                     f2.GetFilePath())) {

        if (f1.GetFileSize() != f2.GetFileSize()) {
            return false;
        }

        if (f1.GetHashSum() != f2.GetHashSum()) {
            return false;
        }
    }

    return f1.IsOk() && f2.IsOk();
}

}