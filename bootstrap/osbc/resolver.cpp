/*************************************************************************/
/*************************************************************************/

#include "osbc.h"

#include "resolver.h"

/*************************************************************************/

Resolver::Resolver()
    : ast::NodeVisitor()
    , m_pass(0)
    , m_symbolTable()
{
}

Resolver::~Resolver()
{
}

/*************************************************************************/

PSymbol Resolver::FindOrDie(int lineNumber, ast::PReferenceNode ref, const std::string &errMessage)
{
    auto sym = m_symbolTable->Find(ref);

    if (!sym)
    {
        throw compile_error(
            lineNumber,
            "{0} '{1}'",
            errMessage,
            ref->ToString()
        );
    }

    return sym;
}

/*************************************************************************/

void Resolver::ResolveBase(std::shared_ptr<ast::DeclNode> node)
{
    PSymbol symbol = node->GetSymbol();
    //ASSERT(symbol, "Symbol not declared on node.");

    auto typeRef = node->GetType();
    symbol->baseType = m_symbolTable->Find(typeRef);

    if (!symbol->baseType)
    {
        Token ident = node->GetIdent();

        throw compile_error(
            ident.lineNumber,
            "Undeclared type '{0}' for '{1}'",
            typeRef->ToString(),
            ident.literal
        );
    }
}

/*************************************************************************/
// Root
/*************************************************************************/

void Resolver::Visit(ast::PModuleNode node)
{
    m_symbolTable = node->GetSymbolTable();
    //ASSERT(m_symbolTable, "Symbol table not initialized!");

    m_pass = 0;
    VisitAll(node->GetStatements());

    m_pass = 1;
    VisitAll(node->GetStatements());

    m_pass = 2;
    VisitAll(node->GetStatements());
}

/*************************************************************************/
// Expressions
/*************************************************************************/

void Resolver::Visit(ast::PReferenceNode node)
{
    fmt::println("ReferenceNode: {0} {1}", node->GetLineNumber(), node->GetIdent().literal);
    (void)node;
}

/*************************************************************************/

void Resolver::Visit(ast::PConstantExpressionNode node)
{
    std::string typeName = "void";
    Token token = node->GetToken();

    switch (token.type)
    {
    case Token::Type::VOID:
        typeName = "void";
        break;
     
    case Token::Type::NULL_CONST:
        typeName = "null";
        break;

    case Token::Type::BOOL_CONST:
        typeName = "bool";
        break;

    case Token::Type::CHAR_CONST:
        typeName = "char";
        break;

    case Token::Type::INT_CONST:
        typeName = "int";
        break;

    case Token::Type::STR_CONST:
        typeName = "string";
        break;

    default:
        fmt::println(stderr, "BUG: Unknown constant type: {0}", token.type);
        abort();
        break;
    }

    node->SetResultType(m_symbolTable->Find(typeName));
}

/*************************************************************************/

void Resolver::Visit(ast::PReferenceExpressionNode node)
{
    PSymbol sym = FindOrDie(node->GetLineNumber(), node->GetReference(), "Use of undeclared variable");

    node->SetSymbol(sym);

    node->SetResultType(sym->baseType);
    node->SetConstant(sym->isConst);
}

/*************************************************************************/

void Resolver::Visit(ast::PCallExpressionNode node)
{
    auto callStmt = node->GetCall();
    callStmt->Accept(*this);

    auto funcRef = callStmt->GetReference();
    auto funcSym = m_symbolTable->Find(funcRef);

    //ASSERT(funcSym);
    //ASSERT(funcSym->baseType);

    node->SetResultType(funcSym->baseType);
}

/*************************************************************************/

void Resolver::Visit(ast::PBinaryExpressionNode node)
{
    auto lhs = node->GetLeft();
    auto rhs = node->GetRight();

    lhs->Accept(*this);
    rhs->Accept(*this);

    auto lhsType = lhs->GetResultType();
    auto rhsType = rhs->GetResultType();

    if (lhsType != rhsType)
    {
        throw compile_error(
            node->GetLineNumber(),
            "Unable to perform {0} on two different types.",
            node->GetOperator()
        );
    }

    node->SetConstant(lhs->IsConstant() && rhs->IsConstant());

    node->SetResultType(lhsType);
}

/*************************************************************************/

void Resolver::Visit(ast::PUnaryExpressionNode node)
{
    auto sub = node->GetSub();

    sub->Accept(*this);

    node->SetConstant(sub->IsConstant());

    node->SetResultType(sub->GetResultType());
}

/*************************************************************************/
// Statements
/*************************************************************************/

void Resolver::Visit(ast::PVariableDeclStatementNode node)
{
    ResolveBase(node);
    PSymbol sym = node->GetSymbol();

    auto initializer = node->GetInitializer();
    
    if (initializer)
    {
        initializer->Accept(*this);

        if (sym->isConst && !initializer->IsConstant())
        {
            throw compile_error(
                node->GetLineNumber(),
                "Initializer for constant '{0}' must evaluate to a constant.",
                sym->name()
            );
        }
    }
    else if (sym->isConst)
    {
        throw compile_error(
            node->GetLineNumber(),
            "Initlializer required for constant '{0}'.",
            sym->name()
        );
    }
}

/*************************************************************************/

void Resolver::Visit(ast::PCompoundStatementNode node)
{
    m_symbolTable = node->GetSymbolTable();

    VisitAll(node->GetStatements());

    m_symbolTable = m_symbolTable->Parent();
}

/*************************************************************************/

void Resolver::Visit(ast::PAssignmentStatementNode node)
{
    auto ref = node->GetReference();
    auto sym = FindOrDie(node->GetLineNumber(), ref, "Assignment to undeclared variable");

    if (!sym)
    {
        abort();
    }

    if (sym->useType != Symbol::UseType::Variable || sym->isConst)
    {
        std::string useName;

        switch (sym->useType)
        {
        case Symbol::UseType::Function: useName = "function"; break;
        case Symbol::UseType::Variable: useName = "constant"; break;
        case Symbol::UseType::Parameter: useName = "parameter"; break;
        case Symbol::UseType::Primitive: useName = "primative"; break;
        case Symbol::UseType::Label: useName = "label"; break;
        case Symbol::UseType::Struct: useName = "struct"; break;
        case Symbol::UseType::Enum: useName = "enum"; break;
        case Symbol::UseType::Set: useName = "set"; break;
        default:
            throw std::logic_error("BUG: Unknown use type!");
        }

        throw compile_error(
            node->GetLineNumber(),
            "Cannot assign a value to {0} '{1}'",
            useName,
            sym->name()
        );
    }

    auto expr = node->GetExpression();
    expr->Accept(*this);

    auto exprType = expr->GetResultType();

    if (!exprType)
    {
        abort();
    }

    if (exprType != sym->baseType)
    {
        throw compile_error(
            node->GetLineNumber(),
            "Cannot assign value of '{0}' to variable '{1}' of type '{2}'",
            exprType->name(),
            sym->name(),
            sym->baseType->name()
        );
    }
}

/*************************************************************************/

void Resolver::Visit(ast::PCallStatementNode node)
{
    auto ref = node->GetReference();
    auto sym = FindOrDie(node->GetLineNumber(), ref, "Call to undeclared function");

    auto paramSymbols = sym->GetParameters();
    int i = 0;

    auto nodeParams = node->GetParameters();

    if (paramSymbols.size() != nodeParams.size())
    {
        throw compile_error(
            node->GetLineNumber(),
            "Parameter count mismatch for call to function '{0}'",
            sym->name()
        );
    }

    for (auto paramNode : nodeParams)
    {
        paramNode->Accept(*this);

        auto exprType = paramNode->GetResultType();

        if (exprType != paramSymbols[i]->baseType)
        {
            throw compile_error(
                paramNode->GetLineNumber(),
                "Cannot pass value of type '{0}' to parameter '{1}' of type '{2}'",
                exprType->name(),
                paramSymbols[i]->name(),
                paramSymbols[i]->baseType->name()
            );
        }

        ++i;
    }
}

/*************************************************************************/

void Resolver::Visit(ast::PReturnStatementNode node)
{
    auto expr = node->GetValue();

    if (expr)
        expr->Accept(*this);
}

/*************************************************************************/

void Resolver::Visit(ast::PWhileStatementNode node)
{
    node->GetCondition()->Accept(*this);
    node->GetBody()->Accept(*this);
}

/*************************************************************************/

void Resolver::Visit(ast::PIfStatementNode node)
{
    node->GetCondition()->Accept(*this);
    node->GetTruePart()->Accept(*this);

    auto falsePart = node->GetFalsePart();

    if (falsePart)
        falsePart->Accept(*this);
}

/*************************************************************************/
// Top Level Statements
/*************************************************************************/

void Resolver::Visit(ast::PImportNode node)
{
    (void)node;
}

/*************************************************************************/

void Resolver::Visit(ast::PGlobalVariableNode node)
{
    if (m_pass == 0)
    {
        // Only process on first pass.
        auto varDecl = node->GetVariable();
        varDecl->Accept(*this);
    }
}

/*************************************************************************/

void Resolver::Visit(ast::PParameterDeclNode node)
{
    ResolveBase(node);
}

/*************************************************************************/

void Resolver::Visit(ast::PFunctionNode node)
{
    switch (m_pass)
    {
    case 0:
        // Fist pass for resolving all return values.
        ResolveBase(node);
        break;

    case 1:
        // Second pass for resolving parameter values.
        m_symbolTable = node->GetSymbolTable();

        VisitAll(node->GetParameters());

        m_symbolTable = m_symbolTable->Parent();
        break;

    case 2:
        // Third pass for resolving body items.
        m_symbolTable = node->GetSymbolTable();

        node->GetBody()->Accept(*this);

        m_symbolTable = m_symbolTable->Parent();
        break;
    }
}

/*************************************************************************/
