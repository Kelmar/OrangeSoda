/*************************************************************************/
/*************************************************************************/

#ifndef OSBC_ERROR_H__
#define OSBC_ERROR_H__

/*************************************************************************/

#include <exception>
#include <fmt/base.h>

/*************************************************************************/

namespace err
{
    /*
     * At the moment we are simply throwing an exception when we run into
     * a compile error.  This isn't the most elegant of solutions, as it
     * can only report one error at a time, but for the purposes of our
     * bootstrap compiler should be fine.
     */

    /// @brief Details of a compile error.
    class compile_error : public std::runtime_error
    {
    private:
        int m_lineNumber;

    public:
        /* constructor */ compile_error(int lineNumber, const std::string &what) noexcept
            : std::runtime_error(what)
            , m_lineNumber(lineNumber)
        {
        }

        /* constructor */ compile_error(const compile_error &other)
            : std::runtime_error(other)
            , m_lineNumber(other.m_lineNumber)
        {
        }

        int lineNumber() const { return m_lineNumber; }
    };
}

template <typename... T>
auto compile_error(int lineNumber, fmt::format_string<T...> fmt, T&&... args)
    -> err::compile_error
{
    return err::compile_error(lineNumber, fmt::vformat(fmt, fmt::make_format_args(args...)));
}

/*************************************************************************/

#endif /* OSBC_ERROR_H__ */

/*************************************************************************/
