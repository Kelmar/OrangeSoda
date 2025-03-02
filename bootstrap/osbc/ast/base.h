/*************************************************************************/
/*************************************************************************/

#ifndef OSBC_AST_BASE_H__
#define OSBC_AST_BASE_H__

/*************************************************************************/

#include "../token.h"
#include "../symbol.h"
#include "../scope.h"

/*************************************************************************/

namespace ast
{
    /****************************************************************/

    class Node
    {
    private:
        int m_lineNumber;

    protected:
        /* constructor */ Node(int lineNumber) 
            : m_lineNumber(lineNumber)
        {
        }

    public:
        virtual ~Node() { }

        virtual void Accept(NodeVisitor &visitor) = 0;

        int GetLineNumber() const { return m_lineNumber; }
    };

    typedef std::shared_ptr<Node> PNode;

    class ReferenceNode;
    typedef std::shared_ptr<ReferenceNode> PReferenceNode;

    /****************************************************************/
    /**
     * @brief Interface for nodes that are declarations.
     */
    class DeclNode
    {
    private:
        Token m_ident;
        PSymbol m_symbol;
        PReferenceNode m_type;

    protected:
        /* constructor */ DeclNode(Token ident, PReferenceNode type)
            : m_ident(ident)
            , m_symbol()
            , m_type(type)
        {
        }

    public:
        const Token &GetIdent() const { return m_ident; }

        const PSymbol &GetSymbol() const { return m_symbol; }

        void SetSymbol(PSymbol symbol)
        {
            //ASSERT(!m_symbol, "Symbol already declared.");
            //ASSERT(symbol, "Invalid symbol");

            m_symbol = symbol;
        }

        const PReferenceNode &GetType() const { return m_type; }
    };

    /****************************************************************/

    class TLStatementNode;
    typedef std::shared_ptr<TLStatementNode> PTLStatementNode;

    class ImportNode;
    typedef std::shared_ptr<ImportNode> PImportNode;

    /****************************************************************/

    typedef std::shared_ptr<class ModuleNode> PModuleNode;

    class ModuleNode : public Node, public std::enable_shared_from_this<ModuleNode>
    {
    private:
        struct private_tag__ { explicit private_tag__() = default; };

        std::vector<PTLStatementNode> m_statements;
        std::vector<PImportNode> m_imports;

        PSymbolTable m_symbolTable;
        PCodeScope m_scope;

    public:
        /* constructor */ ModuleNode(private_tag__)
            : Node(-1)
            , m_statements()
            , m_imports()
            , m_symbolTable(std::make_shared<SymbolTable>())
            , m_scope()
        {
        }

        static PModuleNode Create() { return std::make_shared<ModuleNode>(private_tag__()); }

        PModuleNode GetPtr() { return shared_from_this(); }

        PSymbolTable GetSymbolTable() const { return m_symbolTable; }

        void SetScope(PCodeScope scope)
        {
            m_scope = scope;
        }

        PCodeScope GetScope() const { return m_scope; }

        void AddImport(PImportNode &import)
        {
            m_imports.push_back(import);
        }

        void AddImports(auto importList)
        {
            for (auto import : importList)
                m_imports.push_back(import);
        }

        void Add(const PTLStatementNode &node)
        {
            m_statements.push_back(node);
        }

        const std::vector<PImportNode> &GetImports() const { return m_imports; }

        const std::vector<PTLStatementNode> &GetStatements() const { return m_statements; }

        virtual void Accept(NodeVisitor &visitor) override { visitor.Visit(GetPtr()); }
    };

    /****************************************************************/
}

/*************************************************************************/

#endif /* OSBC_AST_BASE_H__ */

/*************************************************************************/
