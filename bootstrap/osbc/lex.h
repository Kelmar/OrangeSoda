/*************************************************************************/
/*************************************************************************/

#ifndef OS_LEX_H__
#define OS_LEX_H__

/*************************************************************************/

#include "bootstrap.h"
#include "token.h"

/*************************************************************************/

class Lexer
{
public:
    enum class ErrorCode
    {
        /// @brief No error detected in operation
        NoError = 0,

        /// @brief Invalid/unknown character escape sequence
        InvalidSequence,

        /// @brief Unexpected character encountered (E.g. missing quote)
        UnexpectedCharacter,

        /// @brief Reach end of line before expected
        UnexpectedEOL,

        /// @brief Reached the end of file before expected
        UnexpectedEOF,
    };

private:
    std::istream *m_file;
    bool m_ownFile;

    std::stack<Token> m_backlog; // Pushed back tokens.

    Token m_current;
    Token m_lookAhead;

    std::string m_line;  // Last read line from the file.
    size_t m_position;   // Where in the line we currently are.
    int m_lineNumber; // The current line number we're on.

    bool m_eof; // Set when we have reached the end of the file.

    void ReadLine(); // Read another line from the file

    // Advance m_position while predicate returns true.
    void ReadChars(std::function<bool (int)> predicate);

    void SkipWhiteSpace();

    inline
    Token Result(size_t start, Token::Type type) const
    {
        std::string_view view(m_line);
        return Token(m_lineNumber, view.substr(start, m_position - start), type);
    }

    inline
    Token Error(size_t start, ErrorCode error) const
    {
        (void)error;
        return Result(start, Token::Type::ERROR);
    }

    ErrorCode ReadOctal(int *res, int minDigits = 1, int maxDigits = -1);
    ErrorCode ReadHex(int *res, int minDigits = 1, int maxDigits = -1);

    ErrorCode ParseEscapedChar(int *res);

    Token GetWord();
    Token GetNumber();
    Token GetChar();
    Token GetString();
    Token GetSpecial();

    Token GetTokenRaw();
    Token GetToken();

public:
    /* constructor */ Lexer(const std::string &filename = "");
    virtual ~Lexer(void);

    const Token &Current() const { return m_current; }
    const Token &LookAhead() const { return m_lookAhead; }

    // Pushes a token back into the lexer so that it becomes the next token read.
    void PushBack(const Token &);

    bool EndOfFile(void) const { return m_eof; }

    Token Get();
};

typedef std::shared_ptr<Lexer> PLexer;

/*************************************************************************/

#endif /* OS_LEX_H__ */

/*************************************************************************/
