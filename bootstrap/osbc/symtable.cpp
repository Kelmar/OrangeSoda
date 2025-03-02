/*************************************************************************/
/*************************************************************************/

#include "osbc.h"

#include "bootstrap.h"
#include "symtable.h"

#include <fmt/format.h>

/*************************************************************************/

SymbolTable::SymbolTable()
    : m_parent(nullptr)
    , m_symbols()
{
}

SymbolTable::SymbolTable(PSymbolTable parent)
    : m_parent(parent)
    , m_symbols()
{
}

/*************************************************************************/

PSymbol SymbolTable::Find(const std::string &name, Scoping scoping /* = Scoping::Normal */) const
{
    auto itr = m_symbols.find(name);

    if (itr != m_symbols.end())
        return itr->second;

    if (scoping != Scoping::LocalOnly && m_parent)
        return m_parent->Find(name, scoping);

    return nullptr;
}

/*************************************************************************/

PSymbol SymbolTable::Find(ast::PReferenceNode reference, Scoping scoping /* = Scoping::Normal */) const
{
    // TODO: Support more complex names.
    return Find(reference->GetIdent().literal, scoping);
}

/*************************************************************************/

PSymbol SymbolTable::Add(const std::string &ident)
{
    PSymbol rval = std::shared_ptr<Symbol>(new Symbol(this, ident));

    m_symbols[ident] = rval;

    return rval;
}

/*************************************************************************/

PSymbol SymbolTable::Add(const Token &ident)
{
    PSymbol rval = Add(ident.literal);
    
    rval->lineNumber = ident.lineNumber;

    return rval;
}

/*************************************************************************/
