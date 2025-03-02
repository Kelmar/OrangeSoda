/*************************************************************************/
/*************************************************************************/

#ifndef OS_BOOTSTRAP_H__
#define OS_BOOTSTRAP_H__

/*************************************************************************/

#include <algorithm>
#include <array>
#include <stdexcept>
#include <fstream>
#include <functional>
#include <iostream>
#include <stack>
#include <string>
#include <string_view>
#include <map>
#include <memory>
#include <vector>

#include <cstdio>

#include <fmt/core.h>

/*************************************************************************/

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

/*************************************************************************/

#if !defined(NDEBUG)
# define ASSERT(X_, R_) do { if (!(X_)) { fprintf(stderr,   \
    "**** ASSERT FAILED ****\r\n"                           \
    "TEST: %s\r\n"                                          \
    "REASON: %s\r\n"                                        \
    "FILE: %s\r\n"                                          \
    "LINE: %d\r\n"                                          \
    "**** ASSERT FAILED ****\r\n",                          \
    #X_, R_, __FILE__, __LINE__);                           \
    abort(); } } while(false)
#else
# define ASSERT(X_, R_) (void)(0)
#endif

/*************************************************************************/

#endif /* OS_BOOTSTRAP_H__ */

/*************************************************************************/

