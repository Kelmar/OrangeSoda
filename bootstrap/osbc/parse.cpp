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
ast::PReferenceNode Parser::ParseTypeReference(bool acceptVoid)
{
    switch (m_current.type)
    {
    case Token::Type::VOID:
        if (!acceptVoid)
            throw compile_error(m_current.lineNumber, "Void is invalid type for this declartion.");
        [[fallthrough]];

    case Token::Type::IDENT:
    case Token::Type::BOOL:
    case Token::Type::CHAR:
    case Token::Type::INT:
    case Token::Type::STRING:
        return ast::ReferenceNode::Create(Accept());

    default:
        throw compile_error(m_current.lineNumber, "Unexpected {0} token, type expected.", m_current.type);
    }
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

    // GCC 11 erroniously thinks we can get to this part.
    return 0;
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
    case Token::Type::CONST:
        body.push_back(ParseConstDecl());
        break;

    case Token::Type::VAR:
        for (auto decl : ParseVarDecl())
            body.push_back(decl);
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

    if (rval)
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

    ast::PReferenceNode typeReference = ParseTypeReference(false);

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
        returnType = ParseTypeReference(true);
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
 * @brief Parse a list of identifiers for variable defintions.
 * 
 * ident_list: ident
 *           | ident_list ',' ident
 */
std::vector<Token> Parser::ParseIdentList()
{
    std::vector<Token> rval;

    while (true)
    {
        rval.push_back(Accept(Token::Type::IDENT));

        if (m_current.type != (Token::Type)',')
            break;

        Accept(',');
    }

    return rval;
}

/*************************************************************************/
/**
 * @brief Parse a variable defintion.
 * 
 * variable: VAR ident_list ':' type_reference
 */
std::vector<ast::PVariableDeclStatementNode> Parser::ParseVarDecl()
{
    Accept(Token::Type::VAR);

    std::vector<Token> varNames;
    
    for (Token t : ParseIdentList())
        varNames.push_back(t);

    Accept(':');

    auto typeReference = ParseTypeReference(false);

    std::vector<ast::PVariableDeclStatementNode> rval;

    for (auto varName : varNames)
        rval.push_back(ast::VariableDeclStatementNode::Create(false, varName, typeReference, nullptr));

    return rval;
}

/*************************************************************************/
/**
 * @brief Parse a constant declaration.
 *
 * variable: CONST <ident> '=' constant_value
 *         | CONST <ident> ':' type_reference '=' constant_value
 */
ast::PVariableDeclStatementNode Parser::ParseConstDecl()
{
    Accept(Token::Type::CONST);

    Token varName = Accept(Token::Type::IDENT);

    Accept(':');

    auto typeReference = ParseTypeReference(false);

    Accept('=');

    auto initializer = ParseExpression();

    return ast::VariableDeclStatementNode::Create(true, varName, typeReference, initializer);
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
        for (auto decl : ParseVarDecl())
            mod->Add(ast::GlobalVariableNode::Create(decl));
        Accept(';');
        return true;

    case Token::Type::CONST:
        mod->Add(ast::GlobalVariableNode::Create(ParseConstDecl()));
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
