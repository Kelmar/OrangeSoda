/*************************************************************************/
/*************************************************************************/

#include "osbc.h"
#include "codegen.h"
#include "opcodes.h"

/*************************************************************************/

namespace
{
    std::map<Token::Type, OpCode> s_uni_op_map =
    {
        { (Token::Type)'-', OpCode::NEG },
        { (Token::Type)'!', OpCode::NOT },
        { (Token::Type)'~', OpCode::NOT }
    };

    std::map<Token::Type, OpCode> s_bin_op_map =
    {
        { (Token::Type)'+'       , OpCode::ADD },
        { (Token::Type)'-'       , OpCode::SUB },
        { (Token::Type)'*'       , OpCode::MUL },
        { (Token::Type)'/'       , OpCode::DIV },
        { (Token::Type)'%'       , OpCode::MOD },
        { (Token::Type)'&'       , OpCode::AND },
        { (Token::Type)'|'       , OpCode::OR  },
        { (Token::Type)'^'       , OpCode::XOR },
        { (Token::Type)'>'       , OpCode::GT  },
        { (Token::Type)'<'       , OpCode::LT  },
        { Token::Type::Equality  , OpCode::EQU },
        { Token::Type::NotEqual  , OpCode::NEQ },
        { Token::Type::LessEqual , OpCode::LTE },
        { Token::Type::GreatEqual, OpCode::GTE },
        { Token::Type::LeftShift , OpCode::SHL },
        { Token::Type::RightShift, OpCode::SHR }
    };
}

/*************************************************************************/

CodeGen::CodeGen()
    : m_symbolTable()
    , m_autoLabelId(0)
{
}

CodeGen::~CodeGen()
{
}

/*************************************************************************/

std::string CodeGen::NewAutoLabel()
{
    std::string newLabel = fmt::format("__auto_lab_{0}", m_autoLabelId);
    ++m_autoLabelId;
    return newLabel;
}

/*************************************************************************/
// Root
/*************************************************************************/

void CodeGen::Visit(ast::PModuleNode node)
{
    m_symbolTable = node->GetSymbolTable();

    VisitAll(node->GetStatements());
}

/*************************************************************************/
// Expressions
/*************************************************************************/

void CodeGen::Visit(ast::PReferenceNode node)
{
    (void)node;
}

/*************************************************************************/

void CodeGen::Visit(ast::PConstantExpressionNode node)
{
    fmt::println("  LDC {0}", node->GetToken().literal);
}

/*************************************************************************/

void CodeGen::Visit(ast::PReferenceExpressionNode node)
{
    std::string op = "LDV";

    if (node->IsConstant())
        op = "LDC";

    fmt::println("  {0} {1}", op, node->GetReference()->GetIdent().literal);
}

/*************************************************************************/

void CodeGen::Visit(ast::PCallExpressionNode node)
{
    node->GetCall()->Accept(*this);
}

/*************************************************************************/

void CodeGen::Visit(ast::PBinaryExpressionNode node)
{
    node->GetLeft()->Accept(*this);
    node->GetRight()->Accept(*this);

    auto op = s_bin_op_map[node->GetOperator()];

    fmt::println("  {0}", op);
}

/*************************************************************************/

void CodeGen::Visit(ast::PUnaryExpressionNode node)
{
    node->GetSub()->Accept(*this);

    auto op = s_uni_op_map[node->GetOperator()];

    fmt::println("  {0}", op);
}

/*************************************************************************/
// Statements
/*************************************************************************/

void CodeGen::Visit(ast::PVariableDeclStatementNode node)
{
    if (node->IsConstant())
    {
        // TODO: Perform constant evaluation here.
    }
}

/*************************************************************************/

void CodeGen::Visit(ast::PCompoundStatementNode node)
{
    m_symbolTable = node->GetSymbolTable();

    VisitAll(node->GetStatements());

    m_symbolTable = m_symbolTable->Parent();
}

/*************************************************************************/

void CodeGen::Visit(ast::PAssignmentStatementNode node)
{
    node->GetExpression()->Accept(*this);
    fmt::println("  STV {0}", node->GetReference()->GetIdent().literal);
}

/*************************************************************************/

void CodeGen::Visit(ast::PCallStatementNode node)
{
    for (auto param : node->GetParameters())
        param->Accept(*this);

    fmt::println("  JSR {0}", node->GetReference()->GetIdent().literal);
}

/*************************************************************************/

void CodeGen::Visit(ast::PReturnStatementNode node)
{
    auto expr = node->GetValue();
    
    if (expr)
        expr->Accept(*this);

    fmt::println("  RTS");
}

/*************************************************************************/

void CodeGen::Visit(ast::PWhileStatementNode node)
{
    std::string whileTop = NewAutoLabel();
    std::string whileExit = NewAutoLabel();

    fmt::println("{0}:", whileTop);

    node->GetCondition()->Accept(*this);
    fmt::println("  NOT");
    fmt::println("  CBR {0}", whileExit);

    node->GetBody()->Accept(*this);
    fmt::println("  BRA {0}", whileTop);
    fmt::println("{0}:", whileExit);
}

/*************************************************************************/

void CodeGen::Visit(ast::PIfStatementNode node)
{
    std::string falseTop = NewAutoLabel();

    node->GetCondition()->Accept(*this);
    fmt::println("  NOT");
    fmt::println("  CBR {0}", falseTop);

    node->GetTruePart()->Accept(*this);

    auto falsePart = node->GetFalsePart();

    if (falsePart)
    {
        std::string ifEnd = NewAutoLabel();
        fmt::println("  BRA {0}", ifEnd);
        fmt::println("{0}:", falseTop);

        falsePart->Accept(*this);
        fmt::println("{0}:", ifEnd);
    }
    else
    {
        fmt::println("{0}:", falseTop);
    }
}

/*************************************************************************/
// Top Level Statements
/*************************************************************************/

void CodeGen::Visit(ast::PImportNode node)
{
    (void)node;
}

/*************************************************************************/

void CodeGen::Visit(ast::PGlobalVariableNode node)
{
    (void)node;
}

/*************************************************************************/

void CodeGen::Visit(ast::PParameterDeclNode node)
{
    (void)node;
}

/*************************************************************************/

void CodeGen::Visit(ast::PFunctionNode node)
{
    fmt::println("{0}:", node->GetIdent().literal);

    m_symbolTable = node->GetSymbolTable();

    auto body = node->GetBody();

    body->Accept(*this);

    if (!body->HasReturn())
        fmt::println("  RTS");

    m_symbolTable = m_symbolTable->Parent();

    fmt::println("");
}

/*************************************************************************/
