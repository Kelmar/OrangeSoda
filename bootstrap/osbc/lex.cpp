/*************************************************************************/
/*************************************************************************/

#include "bootstrap.h"
#include "lex.h"

#include <fmt/format.h>

/*************************************************************************/
/*************************************************************************/

namespace
{
    const std::map<std::string, Token::Type> s_keywords =
    {
        { "if"       , Token::Type::IF         },
        { "in"       , Token::Type::IN         },
        { "for"      , Token::Type::FOR        },
        { "out"      , Token::Type::OUT        },
        { "ref"      , Token::Type::REF        },
        { "set"      , Token::Type::SET        },
        { "var"      , Token::Type::VAR        },
        { "base"     , Token::Type::BASE_REF   },
        { "else"     , Token::Type::ELSE       },
        { "enum"     , Token::Type::ENUM       },
        { "null"     , Token::Type::NULL_CONST },
        { "this"     , Token::Type::THIS_REF   },
        { "true"     , Token::Type::BOOL_CONST },
        { "break"    , Token::Type::BREAK      },
        { "class"    , Token::Type::CLASS      },
        { "const"    , Token::Type::CONST      },
        { "false"    , Token::Type::BOOL_CONST },
        { "while"    , Token::Type::WHILE      },
        { "export"   , Token::Type::EXPORT     },
        { "import"   , Token::Type::IMPORT     },
        { "return"   , Token::Type::RETURN     },
        { "switch"   , Token::Type::SWITCH     },
        { "continue" , Token::Type::CONTINUE   },
        { "function" , Token::Type::FUNCTION   },
        { "interface", Token::Type::INTERFACE  }
    };
}

/*************************************************************************/
/*************************************************************************/

Lexer::Lexer(const std::string &filename)
    : m_file(&std::cin)
    , m_ownFile(false)
    , m_backlog()
    , m_current()
    , m_lookAhead()
    , m_line()
    , m_position(0)
    , m_lineNumber(0)
    , m_eof(false)
{
    if (!filename.empty())
    {
        m_file = new std::fstream(filename, std::ios::in);

        if (m_file->fail())
            throw std::runtime_error(fmt::format("Unable to open file '{0}' for reading.", filename));

        m_ownFile = true;
    }

    ReadLine();
}

Lexer::~Lexer()
{
    if (m_ownFile)
        delete m_file;
}

/*************************************************************************/

void Lexer::PushBack(const Token &t)
{
    //ASSERT(t.type != Token::Type::Null);

    if (m_lookAhead.type != Token::Type::Null)
        m_backlog.push(m_lookAhead);

    m_lookAhead = m_current;
    m_current = t;
}

/*************************************************************************/

void Lexer::ReadLine()
{
    if (m_eof)
        return;

    if (!std::getline(*m_file, m_line))
    {
        m_line = "";
        m_eof = true;
    }

    ++m_lineNumber;
    m_position = 0;
}

/*************************************************************************/

void Lexer::ReadChars(std::function<bool (int)> predicate)
{
    while (m_position < m_line.length() && predicate(m_line[m_position]))
        ++m_position;
}

/*************************************************************************/

void Lexer::SkipWhiteSpace()
{
    while (!m_eof)
    {
        ReadChars(isspace);
        
        if (m_position < m_line.length())
            break;

        ReadLine();
    }
}

/*************************************************************************/
/*
 * Read a set of ocatal digits from the stream.
 */
Lexer::ErrorCode Lexer::ReadOctal(int *res, int minDigits /*= 1*/, int maxDigits /*= -1*/)
{
    int rval = 0;
    int cnt = 0;

    while (m_position < m_line.length())
    {
        char c = m_line[m_position];

        if (c < '0' || c > '7')
            break;

        ++m_position;
        ++cnt;
        rval = rval * 8 + (c - '0');

        if (maxDigits > 0 && (cnt >= maxDigits))
            break;
    }

    if (cnt < minDigits)
        return ErrorCode::InvalidSequence;

    *res = rval;
    return ErrorCode::NoError;
}

/*************************************************************************/
/*
 * Read a set of hex digits from the stream.
 */
Lexer::ErrorCode Lexer::ReadHex(int *res, int minDigits /*= 1*/, int maxDigits /*= -1*/)
{
    int rval = 0;
    int cnt = 0;

    while (m_position < m_line.length())
    {
        char c = m_line[m_position];
        int val = -1;

        if ((c >= '0') && (c <= '9'))
            val = c - '0';
        else if ((c >= 'A') && (c <= 'F'))
            val = c - 'A' + 10;
        else if ((c >= 'a') && (c <= 'f'))
            val = c - 'a' + 10;
        else
            break;

        //ASSERT(val >= 0 && val <= 15);

        ++m_position;
        ++cnt;
        rval = rval * 16 + val;

        if (maxDigits > 0 && (cnt >= maxDigits))
            break;
    }

    if (cnt < minDigits)
        return ErrorCode::InvalidSequence;

    *res = rval;
    return ErrorCode::NoError;
}

/*************************************************************************/
/*
 * Parse a single escaped character.
 */
Lexer::ErrorCode Lexer::ParseEscapedChar(int *res)
{
    if (++m_position >= m_line.length())
        return ErrorCode::UnexpectedEOL;

    int parsed = 0;
    ErrorCode err = ErrorCode::NoError;

    switch (m_line[m_position])
    {
    case '\'': parsed = '\''; break; // Single quote
    case '\"': parsed = '\"'; break; // Double quote
    case '\?': parsed = '\?'; break; // Question mark
    case '\\': parsed = '\\'; break; // Backslash

    case 'a': parsed = '\a'; break; // Bell
    case 'b': parsed = '\b'; break; // Backspace
    case 'e': parsed = 27  ; break; // Escape
    case 'f': parsed = '\f'; break; // Form feed
    case 'n': parsed = '\n'; break; // Line feed
    case 'r': parsed = '\r'; break; // Carriage return
    case 't': parsed = '\t'; break; // Horizontal tab
    case 'v': parsed = '\v'; break; // Vertical tab

    // Octal code (1 to 3 digits)
    case '0': case '1': case '2': case '3':
    case '4': case '5': case '6': case '7':
        err = ReadOctal(&parsed, 1, 3);
        break;

    case 'x': // Aribtrary hex code (any number of digits)
        ++m_position; // Eat 'x'
        err = ReadHex(&parsed);
        break;

#if 0
    // We don't support unicode (yet)
    case 'u': // UTF-16 character (4 hex digits)
        ++m_position; // Eat 'u'
        err = readHex(line, &parsed, 4, 4);
        break;

    case 'U': // UTF-32 character (8 hex digits)
        ++m_position; // Eat 'U'
        err = readHex(line, &parsed, 8, 8);
        break;
#endif

#if 0
    /*
     * These are new as of C++23, not sure if we'll support them directly in Orange Soda as we may
     * opt to use the curly brace {} as a string insert sequence, much like C# and other languages.
     *
     * \o{n...}
     * \x{n...}
     * \u{n...}
     * \N{name}
     */
    case 'o': // Aribtrary octal code (any number of digits)
        break;

    case 'N': // Named unicode character
        break;
#endif

    default:
        return ErrorCode::InvalidSequence;
    }

    ++m_position;

    if (err == ErrorCode::NoError)
        *res = parsed;

    return err;
}

/*************************************************************************/

Token Lexer::GetWord()
{
    size_t start = m_position;

    // Read ident or reserved word
    ReadChars([] (int c) { return isalnum(c) || c == '_'; });

    auto rval = Result(start, Token::Type::IDENT);

    auto it = s_keywords.find(rval.literal);

    if (it != s_keywords.end())
        rval.type = it->second;

    return rval;
}

/*************************************************************************/

Token Lexer::GetNumber()
{
    size_t start = m_position;

    // Read digits
    ReadChars(isdigit);

    return Result(start, Token::Type::INT_CONST);
}

/*************************************************************************/

Token Lexer::GetChar()
{
    ErrorCode err = ErrorCode::NoError;
    size_t start = m_position;
    int parsed = 0;

    ++m_position;

    switch (m_line[m_position])
    {
    case 0:
        // Error, unexpected end of line.
        return Error(start, ErrorCode::UnexpectedEOL);

    case '\\':
        err = ParseEscapedChar(&parsed);
        break;

    case '\'': // Empty char, treat as NULL character
        break;

    default:
        parsed = m_line[m_position];
        ++m_position; // Consume character for literal.
        break;
    }

    (void)parsed;

    if (m_line[m_position] != '\'')
    {
        // Error, expected closing single quote.
        return Error(start, ErrorCode::UnexpectedCharacter);
    }

    ++m_position;

    if (err != ErrorCode::NoError)
        return Error(start, err);

    return Result(start, Token::Type::CHAR_CONST);
}

/*************************************************************************/

Token Lexer::GetString()
{
    size_t start = m_position;
    std::string parsed;

    while ((m_position < m_line.length()) && (m_line[++m_position] != '\"'))
    {
        if (m_line[m_position] == '\\')
        {
            int c;

            ErrorCode err = ParseEscapedChar(&c);

            if (err != ErrorCode::NoError)
                return Error(start, err);

            parsed += (char)c;
        }
        else
            parsed += m_line[m_position];
    }

    (void)parsed;

    ++m_position;

    return Result(start, Token::Type::STR_CONST);
}

/*************************************************************************/

Token Lexer::GetSpecial()
{
    // Read a special token
    char c = m_line[m_position];
    Token::Type type = (Token::Type)c;
    size_t start = m_position;

    ++m_position;

    switch (c)
    {
    case '=':
        if (m_line[m_position] == '=')
        {
            type = Token::Type::Equality;
            ++m_position;
        }
        break;

    case '!':
        if (m_line[m_position] == '=')
        {
            type = Token::Type::NotEqual;
            ++m_position;
        }
        break;

    case '>':
        switch (m_line[m_position])
        {
        case '=':
            type = Token::Type::GreatEqual;
            ++m_position;
            break;

        case '>':
            type = Token::Type::RightShift;
            ++m_position;
            break;
        }
        break;

    case '<':
        switch (m_line[m_position])
        {
        case '=':
            type = Token::Type::LessEqual;
            ++m_position;
            break;

        case '<':
            type = Token::Type::LeftShift;
            ++m_position;
            break;
        }
        break;

    case '*':
        if (m_line[m_position] == '/')
        {
            type = Token::Type::COMMENT_END;
            ++m_position;
        }
        break;

    case '/':
        switch (m_line[m_position])
        {
        case '/':
            type = Token::Type::EOL_COMMENT;
            ++m_position;
            break;

        case '*':
            type = Token::Type::COMMENT_START;
            ++m_position;
            break;
        }

        break;
    
    }

    return Result(start, type);
}

/*************************************************************************/
/**
 * @brief Read a token from the input stream, regardless of backlog state.
 * 
 * @details
 * Reads a token from the current line/file.  Does not account for backlog
 * tokens, comments, or any thing else.
 */
Token Lexer::GetTokenRaw()
{
    SkipWhiteSpace();

    if (m_eof && (m_position >= m_line.length()))
        return Token(m_lineNumber, "", Token::Type::EndOfFile);

    if (isdigit(m_line[m_position]))
        return GetNumber();

    if (isalpha(m_line[m_position]) || m_line[m_position] == '_')
        return GetWord();

    if (m_line[m_position] == '\"')
        return GetString();

    if (m_line[m_position] == '\'')
        return GetChar();

    return GetSpecial();
}

/*************************************************************************/
/**
 * @brief Read a token from the backlog or input stream.
 * 
 * @details
 * This function reads tokens from the backlog and handles stripping out comments.
 */
Token Lexer::GetToken()
{
    if (!m_backlog.empty())
    {
        Token rval = m_backlog.top();
        m_backlog.pop();
        return rval;
    }

    bool inComment = false;

    for (;;)
    {
        if (m_eof)
            return Token(m_lineNumber, "", Token::Type::EndOfFile);

        if (inComment)
        {
            size_t idx = m_line.find("*/");

            if (idx == std::string::npos)
            {
                // End of block not found, skip this line
                ReadLine();
                continue;
            }
            else
            {
                m_position = idx + 2;
                inComment = false;
            }
        }
        else
        {
            Token rval = GetTokenRaw();

            if (rval.type == Token::Type::COMMENT_START)
            {
                inComment = true;
                continue;
            }

            if (rval.type == Token::Type::EOL_COMMENT)
            {
                // Skip remainder of this line
                ReadLine();
                continue;
            }

            return rval;
        }
    }
}

/*************************************************************************/

Token Lexer::Get()
{
    if (m_current.type == Token::Type::EndOfFile)
        return m_current;

    if ((m_lookAhead.type == Token::Type::Null) ||
        (m_lookAhead.type == Token::Type::UNKNOWN)
       )
    {
        m_lookAhead = GetToken();
    }

    m_current = m_lookAhead;
    m_lookAhead = GetToken();

    return m_current;
}

/*************************************************************************/
