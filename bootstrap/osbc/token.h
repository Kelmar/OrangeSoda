/*************************************************************************/
/*************************************************************************/

#ifndef OS_TOKEN_H__
#define OS_TOKEN_H__

/*************************************************************************/

#include "bootstrap.h"

/*************************************************************************/
/**
 * @brief How a parameter should be passed to a function.
 */
enum class PassByType
{
    /// Parameter is passed by its normal pass by type.
    Default = 0,

    /// Parameter is passed by reference for input only.
    In,

    /// Parameter is passed by reference for output only.
    Out,

    /// Parameter is passed by reference for input and output.
    Ref
};

/*************************************************************************/

struct Token
{
    enum class Type : std::uint32_t
    {
        Null          = 0x0000'0000, // This is NOT the "null" keyword.
        EndOfFile     = 0x0000'0004,

        Bang          = '!',
        Percent       = '%',
        Ampersand     = '&',
        LeftParen     = '(',
        RightParen    = ')',
        Astrisk       = '*',
        Plus          = '+',
        Comma         = ',',
        Minus         = '-',
        Dot           = '.',
        Slash         = '/',
        Colon         = ':',
        Simicolon     = ';',
        LeftAngle     = '<',
        Assignment    = '=',
        RightAngle    = '>',
        Question      = '?',
        At            = '@',
        LeftSquare    = '[',
        Backslash     = '\\',
        RightSquare   = ']',
        Carrot        = '^',
        LeftCurl      = '{',
        Pipe          = '|',
        RightCurl     = '}',
        Tilde         = '~',

        // First 256 values are character literals.

        NULL_CONST    = 0x0000'1010, // 'null' keyword
        BOOL_CONST    = 0x0000'1011, // 'true'/'false' keywords
        CHAR_CONST    = 0x0000'1012,
        INT_CONST     = 0x0000'1013,
        STR_CONST     = 0x0000'1014,

        BASE_REF      = 0x0000'1015, // 'base' keyword
        THIS_REF      = 0x0000'1016, // 'this' keyword
        
        IDENT         = 0x0000'1020,

        /*
         * We don't support all these keywords yet, but we reserve them here
         * so that we'll generate an error if we see them, and not try and
         * treat them like a regular identifier.
         */ 

        // Keywords
        IF            = 0x0000'2000,
        IN            = 0x0000'2001,

        FOR           = 0x0000'3000,
        INT           = 0x0000'3001,
        OUT           = 0x0000'3002,
        REF           = 0x0000'3003,
        SET           = 0x0000'3004,
        VAR           = 0x0000'3005,

        BOOL          = 0x0000'4000,
        CHAR          = 0x0000'4001,
        ELSE          = 0x0000'4002,
        ENUM          = 0x0000'4003,
        VOID          = 0x0000'4004,

        BREAK         = 0x0000'5000,
        CLASS         = 0x0000'5001,
        CONST         = 0x0000'5002,
        WHILE         = 0x0000'5003,

        EXPORT        = 0x0000'6000,
        IMPORT        = 0x0000'6001,
        RETURN        = 0x0000'6002,
        STRING        = 0x0000'6003,
        SWITCH        = 0x0000'6004,

        CONTINUE      = 0x0000'8000,
        FUNCTION      = 0x0000'8001,

        INTERFACE     = 0x0000'9000,

        // Multi character tokens
        Equality      = 0x0001'0000, // ==
        NotEqual      = 0x0001'0001, // !=
        GreatEqual    = 0x0001'0002, // >=
        LessEqual     = 0x0001'0003, // <=
        LeftShift     = 0x0001'0004, // <<
        RightShift    = 0x0001'0005, // >>
        EOL_COMMENT   = 0x0001'0006, // //
        COMMENT_START = 0x0001'0007, // /*
        COMMENT_END   = 0x0001'0008, // */

        ERROR         = 0xFFFF'FF00, // Error in lex processing.
        UNKNOWN       = 0xFFFF'FF01, // Unknown token encountered?
    };

    int lineNumber;
    std::string literal;
    Type type;

    Token()
        : lineNumber(0)
        , literal()
        , type(Type::UNKNOWN)
    {
    }

    Token(int ln, std::string_view l, Type t)
        : lineNumber(ln)
        , literal(l)
        , type(t)
    {
    }

    Token(const Token &rhs)
        : lineNumber(rhs.lineNumber)
        , literal(rhs.literal)
        , type(rhs.type)
    {
    }

    Token(Token &&rhs) noexcept
        : lineNumber(std::move(rhs.lineNumber))
        , literal(std::move(rhs.literal))
        , type(std::move(rhs.type))
    {
    }

    const Token &operator =(const Token &rhs)
    {
        lineNumber = rhs.lineNumber;
        literal = rhs.literal;
        type = rhs.type;

        return *this;
    }

    const Token &operator =(Token &&rhs) noexcept
    {
        lineNumber = std::move(rhs.lineNumber);
        literal = std::move(rhs.literal);
        type = std::move(rhs.type);

        return *this;
    }
};

/*************************************************************************/

template <>
struct fmt::formatter<Token::Type> : fmt::formatter<std::string>
{
    // Defined in lex.cpp
    auto format(Token::Type type, format_context &ctx) const
        -> format_context::iterator
    {
        std::string name;
        int typeVal = static_cast<int>(type);

        switch (type)
        {
        case Token::Type::EndOfFile: name = "end of file"; break;

        case Token::Type::NULL_CONST: name = "null"; break;
        case Token::Type::BOOL_CONST: name = "boolean constant"; break;
        case Token::Type::CHAR_CONST: name = "character constant"; break;
        case Token::Type::INT_CONST: name = "integer constant"; break;
        case Token::Type::STR_CONST: name = "string constant"; break;
        
        case Token::Type::BASE_REF: name = "base"; break;
        case Token::Type::THIS_REF: name = "this"; break;

        case Token::Type::IDENT: name = "identifier"; break;

        // Keywords
        case Token::Type::IF: name = "if"; break;
        case Token::Type::IN: name = "in"; break;
        case Token::Type::FOR: name = "for"; break;
        case Token::Type::INT: name = "int"; break;
        case Token::Type::OUT: name = "out"; break;
        case Token::Type::REF: name = "ref"; break;
        case Token::Type::SET: name = "set"; break;
        case Token::Type::VAR: name = "var"; break;
        case Token::Type::BOOL: name = "bool"; break;
        case Token::Type::CHAR: name = "char"; break;
        case Token::Type::ELSE: name = "else"; break;
        case Token::Type::ENUM: name = "enum"; break;
        case Token::Type::VOID: name = "void"; break;
        case Token::Type::BREAK: name = "break"; break;
        case Token::Type::CLASS: name = "class"; break;
        case Token::Type::CONST: name = "const"; break;
        case Token::Type::WHILE: name = "while"; break;
        case Token::Type::EXPORT: name = "export"; break;
        case Token::Type::IMPORT: name = "import"; break;
        case Token::Type::RETURN: name = "return"; break;
        case Token::Type::STRING: name = "string"; break;
        case Token::Type::SWITCH: name = "switch"; break;
        case Token::Type::CONTINUE: name = "continue"; break;
        case Token::Type::FUNCTION: name = "function"; break;
        case Token::Type::INTERFACE: name = "interface"; break;

        // Multi character tokens
        case Token::Type::Equality     : name = "\"==\""; break;
        case Token::Type::NotEqual     : name = "\"!=\""; break;
        case Token::Type::GreatEqual   : name = "\">=\""; break;
        case Token::Type::LessEqual    : name = "\"<=\""; break;
        case Token::Type::LeftShift    : name = "\"<<\""; break;
        case Token::Type::RightShift   : name = "\">>\""; break;
        case Token::Type::EOL_COMMENT  : name = "\"//\""; break;
        case Token::Type::COMMENT_START: name = "\"/*\""; break;
        case Token::Type::COMMENT_END  : name = "\"*/\""; break;

        case Token::Type::ERROR: name = "]] !!!!BUG!!!! LEX ERROR! [["; break;
        case Token::Type::UNKNOWN: name = "]] !!!!BUG!!!! UNKNOWN LEX ERROR [["; break;
        default:
            if (typeVal < 127 && typeVal > 31)
            {
                name = '"';
                name += (char)typeVal;
                name += '"';
            }
            else
                name = "]] !!!BUG!!! UNKNOWN TOKEN [[";
                
            break;
        }

        return formatter<std::string>::format(name, ctx);
    }
};

/*************************************************************************/

#endif /* OS_TOKEN_H__ */

/*************************************************************************/
