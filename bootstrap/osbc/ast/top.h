/*************************************************************************/
/*************************************************************************/

#ifndef OSBC_AST_TOP_H__
#define OSBC_AST_TOP_H__

/*************************************************************************/

namespace ast
{
    /****************************************************************/
    // Top Level Statement Node
    class TLStatementNode : public Node
    {
    protected:
        TLStatementNode(int lineNumber) : Node(lineNumber) {}

    public:
        virtual ~TLStatementNode() {}
    };

    /****************************************************************/

    class ImportNode :
        public TLStatementNode,
        public std::enable_shared_from_this<ImportNode>
    {
    protected:
        struct private_tag__ { explicit private_tag__() = default; };

        PReferenceNode m_reference;

    public:
        ImportNode(private_tag__, int lineNumber, PReferenceNode reference)
            : TLStatementNode(lineNumber)
            , m_reference(reference)
        {
        }

        ~ImportNode() {}

        static PImportNode Create(int lineNumber, PReferenceNode reference)
        {
            return std::make_shared<ImportNode>(private_tag__(), lineNumber, reference);
        }

        PImportNode GetPtr() { return shared_from_this(); }

        PReferenceNode GetReference() const { return m_reference; }

        virtual void Accept(NodeVisitor &visitor) override { visitor.Visit(GetPtr()); }
    };

    /****************************************************************/

    class GlobalVariableNode :
        public TLStatementNode,
        public std::enable_shared_from_this<GlobalVariableNode>
    {
    protected:
        struct private_tag__ { explicit private_tag__() = default; };

        PVariableDeclStatementNode m_variable;

    public:
        GlobalVariableNode(private_tag__, PVariableDeclStatementNode variable)
            : TLStatementNode(variable->GetLineNumber())
            , m_variable(variable)
        {
        }

        static PGlobalVariableNode Create(PVariableDeclStatementNode variable)
        {
            return std::make_shared<GlobalVariableNode>(private_tag__(), variable);
        }

        PGlobalVariableNode GetPtr() { return shared_from_this(); }

        PVariableDeclStatementNode GetVariable() const { return m_variable; }

        virtual void Accept(NodeVisitor &visitor) override { visitor.Visit(GetPtr()); }
    };

    /****************************************************************/

    class ParameterDeclNode :
        public Node,
        public DeclNode,
        public std::enable_shared_from_this<ParameterDeclNode>
    {
    private:
        struct private_tag__ { explicit private_tag__() = default; };

        PassByType m_passBy;

    public:
        ParameterDeclNode(
            private_tag__,
            PassByType passBy,
            Token ident,
            PReferenceNode type)
            : Node(ident.lineNumber)
            , DeclNode(ident, type)
            , m_passBy(passBy)
        {
        }

        virtual ~ParameterDeclNode() { }

        static PParameterDeclNode Create(PassByType passBy, Token ident, PReferenceNode type)
        {
            return std::make_shared<ParameterDeclNode>(private_tag__(), passBy, ident, type);
        }

        PParameterDeclNode GetPtr() { return shared_from_this(); }

        PassByType GetPassBy() const { return m_passBy; }

        virtual void Accept(NodeVisitor &visitor) override { visitor.Visit(GetPtr()); }
    };

    typedef std::shared_ptr<ParameterDeclNode> PParameterDeclNode;

    /****************************************************************/

    class FunctionNode :
        public TLStatementNode,
        public DeclNode,
        public std::enable_shared_from_this<FunctionNode>
    {
    protected:
        struct private_tag__ { explicit private_tag__() = default; };
        
        std::vector<PParameterDeclNode> m_parameters;

        PSymbolTable m_symbolTable;
        PCompoundStatementNode m_body;

    public:
        FunctionNode(
            private_tag__,
            Token ident,
            std::vector<PParameterDeclNode> parameters,
            PReferenceNode returnType, 
            const PCompoundStatementNode &body)
            : TLStatementNode(ident.lineNumber)
            , DeclNode(ident, returnType)
            , m_parameters(parameters)
            , m_symbolTable()
            , m_body(body)
        {
        }

        virtual ~FunctionNode() {}

        static PFunctionNode Create(
            Token ident,
            std::vector<PParameterDeclNode> parameters,
            PReferenceNode returnType, 
            const PCompoundStatementNode &body)
        {
            return std::make_shared<FunctionNode>(private_tag__(), ident, parameters, returnType, body);
        }

        PFunctionNode GetPtr() { return shared_from_this(); }

        void SetSymbolTable(PSymbolTable symbolTable)
        {
            //ASSERT(!m_symbolTable, "Symbol table already set.");
            //ASSERT(symbolTable, "Invalid symbol table.");

            m_symbolTable = symbolTable;
        }

        PSymbolTable GetSymbolTable() const { return m_symbolTable; }

        const std::vector<PParameterDeclNode> &GetParameters() const { return m_parameters; }

        const PCompoundStatementNode &GetBody() const { return m_body; }

        virtual void Accept(NodeVisitor &visitor) override { visitor.Visit(GetPtr()); }
    };

    typedef std::shared_ptr<FunctionNode> PFunctionNode;

    /****************************************************************/
}

/*************************************************************************/

#endif /* OSBC_AST_TOP_H__ */

/*************************************************************************/
