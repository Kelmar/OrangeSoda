# OrangeSoda Bootstrap Compiler

set(EXECUTABLE_OUTPUT_PATH "../bin")

set(SRCS "main.cpp"
         "codegen.cpp"
         "lex.cpp"
         "parse.cpp"
         "declarer.cpp"
         "resolver.cpp"
         "scope.cpp"
         "symbol.cpp"
         "symtable.cpp"
         "ast/expr.cpp")

set(HDRS "osbc.h"
         "../include/bootstrap.h"
         "../include/opcodes.h"
         "error.h"
         "codegen.h"
         "lex.h"
         "parse.h"
         "declarer.h"
         "resolver.h"
         "scope.h"
         "symbol.h"
         "symtable.h"
         "token.h")

add_executable(osbc ${SRCS} ${HDRS})

set_property (TARGET osbc PROPERTY CXX_STANDARD 23)

target_link_libraries(osbc PRIVATE fmt::fmt)
target_include_directories(osbc PRIVATE ../include)

if (USE_LLVM)
  find_package(LLVM REQUIRED CONFIG)

  message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
  message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

  llvm_map_components_to_libnames(llvm_libs support core)

  target_link_libraries(osbc PRIVATE ${llvm_libs})
  target_include_directories(osbc PRIVATE ${LLVM_INCLUDE_DIRS})

  add_definitions(${LLVM_DEFINTIONS})
endif()

if (MSVC)
  target_compile_options(osbc PRIVATE /W4 /WX)
else()
  target_compile_options(osbc PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
