/*************************************************************************/
/*************************************************************************/

#include "osbc.h"

#include "declarer.h"

/*************************************************************************/

Declarer::Declarer()
    : ast::NodeVisitor()
    , m_symbolTable()
{
}

Declarer::~Declarer()
{
}

/*************************************************************************/
/**
 * @brief Add a primitive built in type.
 */
PSymbol Declarer::AddPrimitive(const std::string &name)
{
    PSymbol symbol = m_symbolTable->Add(name);

    symbol->lineNumber = -1;
    symbol->useType = Symbol::UseType::Primitive;
    symbol->exporting = false;
    symbol->isConst = true;
    symbol->isSpecial = true;
    symbol->baseType = nullptr;

    return symbol;
}

/*************************************************************************/
/**
 * @brief Add in a predefined symbol. 
 */
PSymbol Declarer::AddBuiltIn(const std::string &literal, PSymbol baseType)
{
    //ASSERT(baseType);

    PSymbol symbol = m_symbolTable->Add(literal);

    symbol->lineNumber = -1;
    symbol->useType = Symbol::UseType::Function;
    symbol->exporting = false;
    symbol->isConst = true;
    symbol->isSpecial = true;
    symbol->baseType = baseType;

    return symbol;
}

/*************************************************************************/

void Declarer::LoadBuiltIns()
{
    //fmt::print("Declarer setting up built in functions and types.\r\n");

    // Minimal viable built in types
    PSymbol voidType = AddPrimitive("void");
    PSymbol boolType = AddPrimitive("bool");
    PSymbol charType = AddPrimitive("char");
    PSymbol intType = AddPrimitive("int");
    PSymbol stringType = AddPrimitive("string");

    (void)charType;
    (void)stringType;

    // Set of minimal viable functions for writing a compiler.
    AddBuiltIn("_open", intType);   // Open file
    AddBuiltIn("_flush", voidType); // Flush file
    AddBuiltIn("_close", voidType); // Close file
    AddBuiltIn("_read", intType);   // Read from file
    AddBuiltIn("_write", boolType); // Write to file
}

/*************************************************************************/

void Declarer::VerifyUndefined(const Token &ident, Scoping scoping /* = Scoping::Normal */)
{
    auto decl = m_symbolTable->Find(ident.literal, scoping);

    if (decl)
    {
        throw compile_error(
            ident.lineNumber,
            "Symbol '{0}' already defined.  (See previous definition on line {1})", 
            ident.literal, 
            decl->lineNumber);
    }
}

/*************************************************************************/
// Root
/*************************************************************************/

void Declarer::Visit(ast::PModuleNode node)
{
    m_symbolTable = node->GetSymbolTable();

    LoadBuiltIns();

    VisitAll(node->GetImports());

    VisitAll(node->GetStatements());
}

/*************************************************************************/
// Expressions
/*************************************************************************/

void Declarer::Visit(ast::PReferenceNode node)
{
    (void)node;
}

/*************************************************************************/

void Declarer::Visit(ast::PConstantExpressionNode node)
{
    (void)node;
}

/*************************************************************************/

void Declarer::Visit(ast::PReferenceExpressionNode node)
{
    (void)node;
}

/*************************************************************************/

void Declarer::Visit(ast::PCallExpressionNode node)
{
    (void)node;
}

/*************************************************************************/

void Declarer::Visit(ast::PBinaryExpressionNode node)
{
    (void)node;
}

/*************************************************************************/

void Declarer::Visit(ast::PUnaryExpressionNode node)
{
    (void)node;
}

/*************************************************************************/
// Statements
/*************************************************************************/

void Declarer::Visit(ast::PVariableDeclStatementNode node)
{
    Token ident = node->GetIdent();

    VerifyUndefined(ident);

    auto symbol = m_symbolTable->Add(ident);
    
    symbol->useType = Symbol::UseType::Variable;
    symbol->exporting = false;
    symbol->isConst = node->IsConstant();
    symbol->isSpecial = false;

    node->SetSymbol(symbol);
}

/*************************************************************************/

void Declarer::Visit(ast::PCompoundStatementNode node)
{
    PSymbolTable localSym = std::make_shared<SymbolTable>(m_symbolTable);

    node->SetSymbolTable(localSym);
    m_symbolTable = localSym;

    VisitAll(node->GetStatements());

    m_symbolTable = localSym->Parent();
}

/*************************************************************************/

void Declarer::Visit(ast::PAssignmentStatementNode node)
{
    (void)node;
}

/*************************************************************************/

void Declarer::Visit(ast::PCallStatementNode node)
{
    (void)node;
}

/*************************************************************************/

void Declarer::Visit(ast::PReturnStatementNode node)
{
    (void)node;
}

/*************************************************************************/

void Declarer::Visit(ast::PWhileStatementNode node)
{
    node->GetBody()->Accept(*this);
}

/*************************************************************************/

void Declarer::Visit(ast::PIfStatementNode node)
{
    node->GetTruePart()->Accept(*this);

    auto falsePart = node->GetFalsePart();

    if (falsePart)
        falsePart->Accept(*this);
}

/*************************************************************************/
// Top Level Statements
/*************************************************************************/

void Declarer::Visit(ast::PImportNode node)
{
    (void)node;
    
#if 0
    // TODO: Add import here.
    Token ident = node.GetReference()->GetIdent();
    
    fmt::print("Importing {0}\r\n", ident.literal);
#endif
}

/*************************************************************************/

void Declarer::Visit(ast::PGlobalVariableNode node)
{
    auto var = node->GetVariable();

    var->Accept(*this);
}

/*************************************************************************/

void Declarer::Visit(ast::PParameterDeclNode node)
{
    Token ident = node->GetIdent();

    // Note that like C#, it is possible to define a parameter with a name that
    // shadows a parent scope.
    //
    // Effectively what this means is that we only care if a parameter name is
    // already defined within this specific function or not.
    VerifyUndefined(ident, Scoping::LocalOnly);

    auto symbol = m_symbolTable->Add(ident);

    symbol->useType = Symbol::UseType::Parameter;
    symbol->passBy = node->GetPassBy();
    symbol->exporting = false;
    symbol->isConst = false;
    symbol->isSpecial = false;

    node->SetSymbol(symbol);
}

/*************************************************************************/

void Declarer::Visit(ast::PFunctionNode node)
{
    Token ident = node->GetIdent();

    VerifyUndefined(ident);

    auto symbol = m_symbolTable->Add(ident);

    symbol->useType = Symbol::UseType::Function;
    symbol->exporting = false;
    symbol->isConst = true;
    symbol->isSpecial = false;
    
    node->SetSymbol(symbol);

    PSymbolTable funcSym = std::make_shared<SymbolTable>(m_symbolTable);

    node->SetSymbolTable(funcSym);
    m_symbolTable = funcSym;

    for (auto param : node->GetParameters())
    {
        Visit(param);
        symbol->AddParameter(param->GetSymbol());
    }

    node->GetBody()->Accept(*this);

    m_symbolTable = funcSym->Parent();
}

/*************************************************************************/
