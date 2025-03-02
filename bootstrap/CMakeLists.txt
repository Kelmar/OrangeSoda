include(FetchContent)

SET (FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 11.0.2
    GIT_SHALLOW ON
)

FetchContent_MakeAvailable(fmt)

add_subdirectory(osvm)
add_subdirectory(osbc)
