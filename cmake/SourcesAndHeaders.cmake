set(sources
    src/md5.cpp
    src/file.cpp
    src/searcher.cpp
)

set(exe_sources
    src/main.cpp
    ${sources}
)

set(headers
    include/file.h
    include/searcher.h
)

set(test_sources
    src/file_test.cpp
)
