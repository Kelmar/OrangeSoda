/*************************************************************************/
/*************************************************************************/

#include "osbc.h"

#include <ranges>

#include "opcodes.h"

#include "lex.h"
#include "symbol.h"
#include "scope.h"
#include "parse.h"

/*************************************************************************/

namespace
{
    std::map<Token::Type, PassByType> s_passByMap =
    {
        { Token::Type::IN , PassByType::In  },
        { Token::Type::OUT, PassByType::Out },
        { Token::Type::REF, PassByType::Ref }
    };
}

/*************************************************************************/

Parser::Parser(const PLexer &lexer)
    : m_lexer(lexer)
    , m_current()
{
    Accept(); // Initialize m_current
}

Parser::~Parser()
{
}

/*************************************************************************/

ast::PExpressionNode Parser::ParseBinary(HigherExpr higher, const std::vector<Token::Type> &ops)
{
    ast::PExpressionNode lhs = higher(this); // Parse LHS

    while (std::find(ops.begin(), ops.end(), m_current.type) != ops.end())
    {
        int lineNumber = m_current.lineNumber;
        Token::Type opType = Accept(m_current.type).type;

        ast::PExpressionNode rhs = higher(this); // Parse RHS

        lhs = ast::BinaryExpressionNode::Create(lineNumber, opType, lhs, rhs);
    }

    return lhs;
}

/*************************************************************************/
/**
 * @brief Parse a name reference
 *
 * @details
 * name_reference: <ident>
 * 
 * Later a reference will be more than just a single ident.  Could be a fully
 * qualified name such as foo.bar.baz.
 * 
 * Unlike ParseReference(), these can only have doted notation, and not have
 * array or pointer references.
 */
ast::PReferenceNode Parser::ParseNameReference()
{
    Token ident = Accept(Token::Type::IDENT);

    return ast::ReferenceNode::Create(ident);
}

/*************************************************************************/
/**
 * @brief Parse a type reference
 *
 * @details
 * type_reference: <ident>
 * 
 * Later a reference will be more than just a single ident.  Could be a fully
 * qualified name such as foo.bar.baz.
 * 
 * Unlike ParseReference(), these can only have doted notation, and not have
 * array or pointer references.
 */
ast::PReferenceNode Parser::ParseTypeReference()
{
    Token ident = Accept(Token::Type::IDENT);

    return ast::ReferenceNode::Create(ident);
}

/*************************************************************************/
/**
 * @brief Parse a reference
 *
 * @details
 * reference: <ident>
 * 
 * Later a reference will be more than just an ident.  Could be a fully
 * qualified name such as foo.bar.baz or moo[5].moof
 */
ast::PReferenceNode Parser::ParseReference()
{
    Token ident = Accept(Token::Type::IDENT);

    return ast::ReferenceNode::Create(ident);
}

/*************************************************************************/
/**
 * @brief Parse a constant literal.
 * 
 * const_literal: NULL
 *              | <bool>
 *              | <char>
 *              | <int>
 *              | <string>
 */
ast::PConstantExpressionNode Parser::ParseConstantLiteral()
{
    switch (m_current.type)
    {
    case Token::Type::NULL_CONST:
    case Token::Type::BOOL_CONST:
    case Token::Type::CHAR_CONST:
    case Token::Type::INT_CONST:
    case Token::Type::STR_CONST:
        return ast::ConstantExpressionNode::Create(Accept());
        
    default:
        return nullptr;
    }
}

/*************************************************************************/
/**
 * @brief Parse a primary expression
 * 
 * @details
 * primary: const_literal
 *        | reference
 *        | call
 *        | '(' expression ')'
 */
ast::PExpressionNode Parser::ParsePrimary()
{
    ast::PExpressionNode rval = ParseConstantLiteral();

    if (rval)
        return rval;

    switch (m_current.type)
    {
    case Token::Type::IDENT: // Variable or named constant reference
        {
            auto ref = ParseReference();

            if (m_current.type == (Token::Type)'(')
                return ParseCallExpression(ref);
            else
                return ast::ReferenceExpressionNode::Create(ref);
        }
        break;

    case (Token::Type)'(': // Sub expression
        {
            Accept();
            rval = ParseExpression();
            Accept(')');
            return rval;
        }
        break;

    default:
        throw compile_error(m_current.lineNumber, "Expected primary expression");
        break;
    }
}

/*************************************************************************/
/*
 * unary: primary
 *      | '+' primary
 *      | '-' primary
 *      | '!' primary
 *      | '~' primary
 */
ast::PExpressionNode Parser::ParseUnary()
{
    Token::Type op = Token::Type::Null; // Null indicates a NOP case
    int lineNumber = m_current.lineNumber;

    switch (m_current.type)
    {
    case (Token::Type)'+': // Effectively a do nothing operation
        Accept();
        break;

    case (Token::Type)'-':
        op = m_current.type;
        Accept();
        break;

    case (Token::Type)'!':
    case (Token::Type)'~':
        op = m_current.type;
        Accept();
        break;

    default:
        break;
    }

    ast::PExpressionNode sub = ParsePrimary();

    if (op != Token::Type::Null)
        sub = ast::UnaryExpressionNode::Create(lineNumber, op, sub);

    return sub;
}

/*************************************************************************/
/*
 * factor: unary
 *       | factor '*' unary
 *       | factor '/' unary
 *       | factor '%' unary
 */
ast::PExpressionNode Parser::ParseFactor()
{
    const std::vector<Token::Type> OPS =
    {
        (Token::Type)'*',
        (Token::Type)'/',
        (Token::Type)'%'
    };

    return ParseBinary(&Parser::ParseUnary, OPS);
}

/*************************************************************************/
/*
 * additive: factor
 *         | additive '+' factor
 *         | additive '-' factor
 */
ast::PExpressionNode Parser::ParseAdditive()
{
    const std::vector<Token::Type> OPS =
    {
        (Token::Type)'+',
        (Token::Type)'-'
    };

    return ParseBinary(&Parser::ParseFactor, OPS);
}

/*************************************************************************/
/*
 * shift: additive
 *      | shift '>>' additive
 *      | shift '<<' additive
 */
ast::PExpressionNode Parser::ParseShift()
{
    const std::vector<Token::Type> OPS =
    {
        Token::Type::LeftShift,
        Token::Type::RightShift
    };

    return ParseBinary(&Parser::ParseAdditive, OPS);
}

/*************************************************************************/
/*
 * relational: shift
 *           | relational '>' shift
 *           | relational '<' shift
 *           | relational '>=' shift
 *           | relational '<=' shift
 */
ast::PExpressionNode Parser::ParseRelational()
{
    const std::vector<Token::Type> OPS =
    {
        (Token::Type)'>',
        (Token::Type)'<',
        Token::Type::GreatEqual,
        Token::Type::LessEqual
    };

    return ParseBinary(&Parser::ParseShift, OPS);
}

/*************************************************************************/
/*
 * equality: relational
 *         | equality '==' relational
 *         | equality '!=' relational
 */
ast::PExpressionNode Parser::ParseEquality()
{
    const std::vector<Token::Type> OPS =
    {
        Token::Type::Equality,
        Token::Type::NotEqual
    };

    return ParseBinary(&Parser::ParseRelational, OPS);
}

/*************************************************************************/
/*
 * expression: equality
 */
ast::PExpressionNode Parser::ParseExpression()
{
    return ParseEquality();
}

/*************************************************************************/
/*
 * assignment: <ident> '=' expression
 */
ast::PStatementNode Parser::ParseAssignment(ast::PReferenceNode varRef)
{
    int lineNumber = m_current.lineNumber;
    
    Accept('=');
    ast::PExpressionNode expr = ParseExpression();
    
    return ast::AssignmentStatementNode::Create(lineNumber, varRef, expr);
}

/*************************************************************************/
/**
 * @brief Parse a list of parmeters for a function call.
 * 
 * param_list: 
 *           | param_list ',' expression
 *           | expression
 */
std::vector<ast::PExpressionNode> Parser::ParseCallParameters()
{
    std::vector<ast::PExpressionNode> rval;

    if (m_current.type == (Token::Type)')')
        return rval; // Empty list

    for (;;)
    {
        rval.push_back(ParseExpression());

        if (m_current.type != (Token::Type)',')
            break;

        Accept(',');
    }

    return rval;
}

/*************************************************************************/
/*
 * call: reference '(' parameter_list ')'
 */
ast::PCallStatementNode Parser::ParseCallStatement(ast::PReferenceNode funcRef)
{
    Accept('(');
    auto parameters = ParseCallParameters();
    Accept(')');

    return ast::CallStatementNode::Create(funcRef, parameters);
}

/*************************************************************************/
/*
 * call: reference '(' parameter_list ')'
 */
ast::PExpressionNode Parser::ParseCallExpression(ast::PReferenceNode funcRef)
{
    ast::PCallStatementNode call = ParseCallStatement(funcRef);
    return ast::CallExpressionNode::Create(call);
}

/*************************************************************************/
/*
 * simple: assignment
 *       | call
 */
ast::PStatementNode Parser::ParseSimpleStatement()
{
    ast::PReferenceNode refNode = ParseReference();

    switch (m_current.type)
    {
    case (Token::Type)'=': // Assignment
        return ParseAssignment(refNode);

    case (Token::Type)'(': // Function call
        return ParseCallStatement(refNode);

    default:
        throw compile_error(m_current.lineNumber, "Excepted assignment or function call");
    }
}

/*************************************************************************/
/*
 * return: RETURN
 *       | RETURN expression
 */
ast::PStatementNode Parser::ParseReturnStatement()
{
    int lineNumber = m_current.lineNumber;

    Accept(Token::Type::RETURN);

    ast::PExpressionNode value;

    if (m_current.type != (Token::Type)';')
        value = ParseExpression();

    return ast::ReturnStatementNode::Create(lineNumber, value);
}

/*************************************************************************/
/*
 * if: IF '(' expression ')' compound ELSE compound
 *   | IF '(' expression ')' compound
 */
ast::PStatementNode Parser::ParseIfStatement()
{
    int lineNumber = m_current.lineNumber;

    Accept(Token::Type::IF);
    Accept('(');
    ast::PExpressionNode condition = ParseExpression();
    Accept(')');

    ast::PCompoundStatementNode truePart = ParseCompoundStatement();
    ast::PCompoundStatementNode falsePart = nullptr;

    if (m_current.type == Token::Type::ELSE)
    {
        Accept();

        falsePart = ParseCompoundStatement();
    }

    return ast::IfStatementNode::Create(lineNumber, condition, truePart, falsePart);
}

/*************************************************************************/
/*
 * while: WHILE '(' expression ')' compound
 */
ast::PStatementNode Parser::ParseWhileStatement()
{
    int lineNumber = m_current.lineNumber;

    Accept(Token::Type::WHILE);

    Accept('(');

    ast::PExpressionNode condition = ParseExpression();
    
    Accept(')');

    auto body = ParseCompoundStatement();

    return ast::WhileStatementNode::Create(lineNumber, condition, body);
}

/*************************************************************************/
/*
 * statement: if
 *          | for
 *          | while
 *          | compound
 *          | simple ';'
 *          | ';'
 */
bool Parser::ParseStatement(std::vector<ast::PStatementNode> &body)
{
    ast::PStatementNode rval;

    switch (m_current.type)
    {
    case Token::Type::VAR:
        rval = ParseVarDecl();
        break;

    case Token::Type::IF:
        rval = ParseIfStatement();
        break;

    //case Token::Type::FOR:
        //rval = parseForStatement();
        //break;

    case Token::Type::WHILE:
        rval = ParseWhileStatement();
        break;

    case (Token::Type)'{':
        rval = ParseCompoundStatement();
        break;

    case Token::Type::RETURN:
        rval = ParseReturnStatement();
        Accept(';');
        break;
        
    case Token::Type::IDENT:
        rval = ParseSimpleStatement();
        Accept(';');
        break;

    case (Token::Type)';': // Empty statement (do nothing)
        Accept();
        return true; // Avoid ASSERT and push_back(), this is a valid case.

    default:
        return false;
    }

    ASSERT(rval != nullptr, "Invalid statement returned from sub parse");

    body.push_back(rval);

    return true;
}

/*************************************************************************/
/*
 * statement_list:
 *               | statement
 *               : statement_list statement
 */
std::vector<ast::PStatementNode> Parser::ParseStatementList()
{
    std::vector<ast::PStatementNode> rval;

    while (!EndOfFile() && ParseStatement(rval))
        ;

    return rval;
}

/*************************************************************************/
/*
 * compound: '{' statement_list '}'
 */
ast::PCompoundStatementNode Parser::ParseCompoundStatement()
{
    auto rval = ast::CompoundStatementNode::Create(m_current.lineNumber);

    Accept('{');
    rval->AddStatements(ParseStatementList());
    Accept('}');

    return rval;
}

/*************************************************************************/
/**
 * @brief Parse a single parameter declaration for a function.
 * 
 * @details
 * param_decl: <ident> ':' type_reference
 *           | IN <ident> ':' type_reference
 *           | OUT <ident> ':' type_reference
 *           | REF <ident> ':' type_reference
 */
ast::PParameterDeclNode Parser::ParseParamDecl()
{
    PassByType passBy  = PassByType::Default;

    auto itr = s_passByMap.find(m_current.type);

    if (itr != s_passByMap.end())
    {
        passBy = itr->second;
        Accept(m_current.type);
    }

    Token ident = Accept(Token::Type::IDENT);

    Accept(':');

    ast::PReferenceNode typeReference = ParseTypeReference();

    return ast::ParameterDeclNode::Create(passBy, ident, typeReference);
}

/*************************************************************************/
/**
 * @brief Parse list of parameter declarations for a function.
 * 
 * @details
 * func_params:
 *            | func_params ',' param_decl
 *            | param_decl
 */
std::vector<ast::PParameterDeclNode> Parser::ParseFunctionParameters()
{
    std::vector<ast::PParameterDeclNode> rval;

    if (m_current.type == (Token::Type)')')
        return rval; // Empty list

    for (;;)
    {
        rval.push_back(ParseParamDecl());

        if (m_current.type != (Token::Type)',')
            break;

        Accept(',');
    }

    return rval;
}

/*************************************************************************/
/**
 * @brief Parse a function declaration
 * 
 * @details
 * function: FUNCTION <ident> '(' ')' compound
 *         | FUNCTION <ident> '(' ')' ':' type_reference compound
 */
ast::PTLStatementNode Parser::ParseFunction()
{
    Accept(Token::Type::FUNCTION);

    Token ident = Accept(Token::Type::IDENT);

    // TODO: Parse parameter defintions.
    Accept('(');
    auto parameters = ParseFunctionParameters();
    Accept(')');

    ast::PReferenceNode returnType;

    if (m_current.type == (Token::Type)':')
    {
        Accept(); // Return type specified.
        returnType = ParseTypeReference();
    }
    else
    {
        // Hack for now, imply decl of "void"
        Token t(-1, "void", Token::Type::VOID);
        returnType = ast::ReferenceNode::Create(t);
    }

    auto body = ParseCompoundStatement();

    return ast::FunctionNode::Create(ident, parameters, returnType, body);
}

/*************************************************************************/
/**
 * @brief Parse a variable or constant defintion.
 * 
 * variable: VAR <ident>
 *         | VAR <ident> ':' type_reference
 *         | CONST <ident> '=' constant_value
 *         | CONST <ident> ':' type_reference '=' constant_value ';'
 */
ast::PVariableDeclStatementNode Parser::ParseVarDecl()
{
    //ASSERT(m_current.type == Token::Type::VAR || m_current.type == Token::Type::CONST);

    Token::Type varType = m_current.type;
    bool isConst = varType == Token::Type::CONST;

    Accept();

    Token varName = Accept(Token::Type::IDENT);
    ast::PExpressionNode initializer;
    ast::PReferenceNode typeReference;

    if (m_current.type == (Token::Type)':')
    {
        // Type specified
        Accept();
        typeReference = ParseTypeReference();
    }

    // For our bootstrap compiler we're going to require the type declaration.

    if (!typeReference)
        throw compile_error(varName.lineNumber, "{0} '{1}' requires type in this compiler.", varType, varName.literal);

    if (m_current.type == (Token::Type)'=')
    {
        Accept('=');
        initializer = ParseExpression();
    }

    return ast::VariableDeclStatementNode::Create(isConst, varName, typeReference, initializer);
}

/*************************************************************************/
/**
 * @brief Parse a top level statement
 *
 * toplevel: function
 *         | variable
 *         | struct
 *         | enum
 *         | set
 */
bool Parser::ParseTopLevelStatement(ast::PModuleNode &mod)
{
    switch (m_current.type)
    {
    case Token::Type::FUNCTION:
        mod->Add(ParseFunction());
        return true;

    case Token::Type::VAR:
    case Token::Type::CONST:
        mod->Add(ast::GlobalVariableNode::Create(ParseVarDecl()));
        Accept(';');
        return true;

    default:
        throw compile_error(m_current.lineNumber, "Unexpected {0} token", m_current.type);
        break;
    }
}

/*************************************************************************/
/*
 * import: IMPORT name_reference
 */
ast::PImportNode Parser::ParseImportStatement()
{
    if (m_current.type != Token::Type::IMPORT)
        return nullptr;

    int lineNumber = m_current.lineNumber;

    Accept(Token::Type::IMPORT);

    return ast::ImportNode::Create(lineNumber, ParseNameReference());
}

/*************************************************************************/
/**
 * @brief Parse block of imports at top of file.
 * 
 * imports: import ';'
 *        | import ';' | imports
 */
void Parser::ParseImports(ast::PModuleNode root)
{
    while (!EndOfFile())
    {
        auto import = ParseImportStatement();

        if (!import)
            break;

        Accept(';');

        root->AddImport(import);
    }
}

/*************************************************************************/
/**
 * @brief Parse a module
 * 
 * module: toplevel
 *       | toplevel module
 */
void Parser::ParseModule(ast::PModuleNode root)
{
    while (!EndOfFile() && ParseTopLevelStatement(root))
        ;
}

/*************************************************************************/
/**
 * @brief Parse whole file
 * 
 * file: imports module
 */
ast::PModuleNode Parser::Execute()
{
    ast::PModuleNode rval = ast::ModuleNode::Create();

    ParseImports(rval);

    ParseModule(rval);

    return rval;
}

/*************************************************************************/
