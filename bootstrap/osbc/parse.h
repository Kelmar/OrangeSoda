/*************************************************************************/
/*************************************************************************/

#ifndef OS_PARSE_H__
#define OS_PARSE_H__

/*************************************************************************/

#include "bootstrap.h"

#include "opcodes.h"

#include "ast.h"
#include "lex.h"
#include "symbol.h"
#include "scope.h"

/*************************************************************************/
/**
 * @brief Syntax parser.
 * 
 * This stage just parses the syntax and builds an abstract syntax tree; but
 * does not resolve any names/identifiers.
 */
class Parser
{
private:
    PLexer m_lexer;
    Token m_current;

    typedef std::function<ast::PExpressionNode (Parser *)> HigherExpr;

protected:
    /**
     * @brief Accept the current token, fetching the next token from the lexer.
     * @returns The previous token
     */
    inline
    Token Accept()
    {
        Token rval = m_current;
        m_current = m_lexer->Get();
        return rval;
    }

    /**
     * @brief Accept the current token provided it matches the given type.
     * @returns The previous token
     */
    inline
    Token Accept(Token::Type type)
    { 
        if (m_current.type != type)
            throw compile_error(m_current.lineNumber, "Unexpected {0} token, expecting {1}", m_current.type, type);

        return Accept();
    }

    /**
     * @brief Accept the current token provided it matches the given type.
     * @returns The previous token
     */
    inline
    Token Accept(char type)
    {
        return Accept((Token::Type)type);
    }

    /**
     * @brief Accept the current token provide it matches one of the given types.
     * @returns The previous token
     */
    template <typename TContainer>
    Token Accept(const TContainer &items)
    {
        static_assert(std::is_same<typename TContainer::value_type, Token::Type>::value, "Can only check container of Token::Type");

        if (!items.contains(m_current.type))
            Error(fmt::format("Unexpected {0} token, expecting {1}", m_current.type, items));

        return Accept();
    }

    /**
     * @brief Check to see if we've reached the end of input scanning for whatever reason.
     */
    inline
    bool EndOfFile() const
    {
        return
            (m_current.type == (Token::Type)0) ||
            (m_current.type == Token::Type::EndOfFile) ||
            (m_current.type > Token::Type::ERROR)
        ;
    }

protected:
    ast::PImportNode ParseImportStatement();

    ast::PReferenceNode ParseNameReference();
    ast::PReferenceNode ParseTypeReference(bool acceptVoid);
    ast::PReferenceNode ParseReference();

    ast::PConstantExpressionNode ParseConstantLiteral();

    ast::PExpressionNode ParseBinary(HigherExpr sub, const std::vector<Token::Type> &ops);

    ast::PExpressionNode ParsePrimary();

    ast::PExpressionNode ParseUnary();
    ast::PExpressionNode ParseFactor();
    ast::PExpressionNode ParseAdditive();
    ast::PExpressionNode ParseShift();
    ast::PExpressionNode ParseRelational();
    ast::PExpressionNode ParseEquality();

    ast::PExpressionNode ParseExpression();

    ast::PStatementNode ParseAssignment(ast::PReferenceNode varRef);

    std::vector<ast::PExpressionNode> ParseCallParameters();

    ast::PCallStatementNode ParseCallStatement(ast::PReferenceNode funcRef);
    ast::PExpressionNode ParseCallExpression(ast::PReferenceNode funcRef);

    ast::PStatementNode ParseSimpleStatement();

    ast::PStatementNode ParseReturnStatement();

    ast::PStatementNode ParseIfStatement();

    ast::PStatementNode ParseWhileStatement();

    bool ParseStatement(std::vector<ast::PStatementNode> &body);

    std::vector<ast::PStatementNode> ParseStatementList();

    ast::PCompoundStatementNode ParseCompoundStatement();

    ast::PParameterDeclNode ParseParamDecl();
    std::vector<ast::PParameterDeclNode> ParseFunctionParameters();
    ast::PTLStatementNode ParseFunction();

    ast::PVariableDeclStatementNode ParseVarDecl();

    /// @brief Parse a top level statement
    bool ParseTopLevelStatement(ast::PModuleNode &mod);

    void ParseImports(ast::PModuleNode root);

    void ParseModule(ast::PModuleNode root);

public:
    /* constructor */ Parser(const PLexer &);
    virtual          ~Parser();

    const Token &Current() const { return m_current; }

    ast::PModuleNode Execute();
};

/*************************************************************************/

#endif /* OS_PARSE_H__ */

/*************************************************************************/
