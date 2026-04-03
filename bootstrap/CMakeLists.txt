include(FetchContent)

SET (FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 12.1.0
    GIT_SHALLOW ON
)

FetchContent_MakeAvailable(fmt)

add_subdirectory(osbc)
