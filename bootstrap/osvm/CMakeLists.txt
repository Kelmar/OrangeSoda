# OrangeSoda boostrap Virtual Machine

set(EXECUTABLE_OUTPUT_PATH "../bin")

set(SRCS "main.cpp")

set(HDRS "../include/bootstrap.h"
         "../include/opcodes.h")

add_executable(osvm ${SRCS} ${HDRS})

set_property (TARGET osvm PROPERTY CXX_STANDARD 23)

if (MSVC)
  target_compile_options(osvm PRIVATE /W4 /WX)
else()
  target_compile_options(osvm PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_link_libraries(osvm PRIVATE fmt::fmt)
target_include_directories(osvm PRIVATE ../include)