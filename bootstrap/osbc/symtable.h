/*************************************************************************/
/*************************************************************************/

#ifndef OS_SYMBOL_TABLE_H__
#define OS_SYMBOL_TABLE_H__

/*************************************************************************/

#include "ast.h"
#include "symbol.h"

/*************************************************************************/
/**
 * @brief Defines how the symbol table resolves symbols in different scopes.
 */
enum class Scoping
{
    /// Scope searches up parent chain.
    Normal,

    /// Scope is limited only to locally defined symbols.
    LocalOnly
};

/*************************************************************************/

typedef std::shared_ptr<class SymbolTable> PSymbolTable;

class SymbolTable
{
private:
    PSymbolTable m_parent;
    std::map<std::string, PSymbol> m_symbols;

public:
    /* constructor */ SymbolTable();
    /* constructor */ SymbolTable(PSymbolTable parent);
    
    virtual ~SymbolTable() { }

    PSymbolTable Parent() const { return m_parent; }
    bool IsEmpty() const { return m_symbols.empty(); }

    // Find a symbol in the current SymbolTable scope.
    PSymbol Find(const std::string &ident, Scoping scoping = Scoping::Normal) const;
    PSymbol Find(ast::PReferenceNode reference, Scoping scoping = Scoping::Normal) const;

    PSymbol Add(const std::string &ident);
    PSymbol Add(const Token &ident);
};

/*************************************************************************/

#endif /* OS_SYMBOL_TABLE_H__ */

/*************************************************************************/
