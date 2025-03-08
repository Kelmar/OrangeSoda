/*************************************************************************/
/*************************************************************************/

#ifndef OSBC_AST_STMT_H__
#define OSBC_AST_STMT_H__

/*************************************************************************/

#include "../token.h"

/*************************************************************************/

namespace ast
{
    /****************************************************************/

    enum class StatementType
    {
        VaribleDeclStatement,
        CompoundStatement,
        AssignmentStatement,
        CallStatement,
        ReturnStatement,
        WhileStatement,
        IfStatement,
    };

    /****************************************************************/

    class StatementNode : public Node
    {
    protected:
        StatementNode(int lineNumber) : Node(lineNumber) { }

    public:
        virtual ~StatementNode() { }

        virtual StatementType GetStatementType() const = 0;
    };

    typedef std::shared_ptr<StatementNode> PStatementNode;

    /****************************************************************/

    class VariableDeclStatementNode : 
        public StatementNode,
        public DeclNode,
        public std::enable_shared_from_this<VariableDeclStatementNode>
    {
    private:
        struct private_tag__ { explicit private_tag__() = default; };
    
        bool m_isConst;
        PExpressionNode m_initializer;

    public:
        VariableDeclStatementNode(
            private_tag__,
            bool isConst,
            Token ident,
            PReferenceNode type,
            PExpressionNode initializer = nullptr)
            : StatementNode(ident.lineNumber)
            , DeclNode(ident, type)
            , m_isConst(isConst)
            , m_initializer(initializer)
        {
        }

        static PVariableDeclStatementNode Create(
            bool isConst,
            Token ident,
            PReferenceNode type,
            PExpressionNode initializer = nullptr)
        {
            return std::make_shared<VariableDeclStatementNode>(private_tag__(), isConst, ident, type, initializer);
        }

        virtual StatementType GetStatementType() const override { return StatementType::VaribleDeclStatement; }

        bool IsConstant() const { return m_isConst; }

        PExpressionNode GetInitializer() const { return m_initializer; }

        virtual void Accept(NodeVisitor &visitor) override { visitor.Visit(shared_from_this()); }  
    };

    typedef std::shared_ptr<VariableDeclStatementNode> PVariableDeclStatementNode;

    /****************************************************************/

    class CompoundStatementNode :
        public StatementNode,
        public std::enable_shared_from_this<CompoundStatementNode>
    {
    private:
        struct private_tag__ { explicit private_tag__() = default; };

        std::vector<PStatementNode> m_statements;

        PCodeScope m_scope;
        PSymbolTable m_symbolTable;

        bool m_hasVarDecl;
        bool m_hasReturn;

    public:

        // lineNumber is start of compound statement.
        CompoundStatementNode(private_tag__, int lineNumber)
            : StatementNode(lineNumber)
            , m_statements()
            , m_scope()
            , m_symbolTable()
            , m_hasVarDecl(false)
            , m_hasReturn(false)
        {
        }

        static PCompoundStatementNode Create(int lineNumber)
        {
            return std::make_shared<CompoundStatementNode>(private_tag__(), lineNumber);
        }

        virtual StatementType GetStatementType() const override { return StatementType::CompoundStatement; }

        void SetScope(PCodeScope scope)
        {
            m_scope = scope;
        }

        PCodeScope GetScope() const { return m_scope; }

        bool HasReturn() const { return m_hasReturn; }

        bool HasVarDecl() const { return m_hasVarDecl; }

        void SetSymbolTable(PSymbolTable symbolTable)
        {
            //ASSERT(!m_symbolTable, "Symbol table already set.");
            //ASSERT(symbolTable, "Invalid symbol table.");

            m_symbolTable = symbolTable;
        }

        PSymbolTable GetSymbolTable() const { return m_symbolTable; }

        const std::vector<PStatementNode> &GetStatements() const { return m_statements; }

        void AddStatement(const PStatementNode &node)
        {
            m_hasVarDecl |= node->GetStatementType() == StatementType::VaribleDeclStatement;
            m_hasReturn |= node->GetStatementType() == StatementType::ReturnStatement;

            m_statements.push_back(node);
        }

        void AddStatements(const std::vector<PStatementNode> &nodes)
        {
            for (auto n : nodes)
                AddStatement(n);
        }

        virtual void Accept(NodeVisitor &visitor) override { visitor.Visit(shared_from_this()); }
    };

    typedef std::shared_ptr<CompoundStatementNode> PCompoundStatementNode;

    /****************************************************************/

    class AssignmentStatementNode :
        public StatementNode,
        public std::enable_shared_from_this<AssignmentStatementNode>
    {
    private:
        struct private_tag__ { explicit private_tag__() = default; };

        PReferenceNode m_reference;
        PExpressionNode m_expression;

    public:
        AssignmentStatementNode(private_tag__, int lineNumber, PReferenceNode ref, PExpressionNode expr)
            : StatementNode(lineNumber)
            , m_reference(ref)
            , m_expression(expr)
        {
        }

        static PAssignmentStatementNode Create(int lineNumber, PReferenceNode ref, PExpressionNode expr)
        {
            return std::make_shared<AssignmentStatementNode>(private_tag__(), lineNumber, ref, expr);
        }

        virtual StatementType GetStatementType() const override { return StatementType::AssignmentStatement; }

        PReferenceNode GetReference() const { return m_reference; }
        PExpressionNode GetExpression() const { return m_expression; }

        virtual void Accept(NodeVisitor &visitor) override { visitor.Visit(shared_from_this()); }
    };

    /****************************************************************/

    class CallStatementNode :
        public StatementNode,
        public std::enable_shared_from_this<CallStatementNode>
    {
    private:
        struct private_tag__ { explicit private_tag__() = default; };

        PReferenceNode m_reference;
        std::vector<PExpressionNode> m_parameters;

    public:
        CallStatementNode(private_tag__, PReferenceNode reference, const std::vector<PExpressionNode> &params)
            : StatementNode(reference->GetLineNumber())
            , m_reference(reference)
            , m_parameters(params)
        {
        }

        static PCallStatementNode Create(PReferenceNode reference, const std::vector<PExpressionNode> &params)
        {
            return std::make_shared<CallStatementNode>(private_tag__(), reference, params);
        }

        virtual StatementType GetStatementType() const override { return StatementType::CallStatement; }

        /// Function being called.
        PReferenceNode GetReference() const { return m_reference; }

        const std::vector<PExpressionNode> &GetParameters() const { return m_parameters; }

        virtual void Accept(NodeVisitor &visitor) override { visitor.Visit(shared_from_this()); }
    };

    /****************************************************************/

    class ReturnStatementNode :
        public StatementNode,
        public std::enable_shared_from_this<ReturnStatementNode>
    {
    private:
        struct private_tag__ { explicit private_tag__() = default; };

        PExpressionNode m_value;

    public:
        ReturnStatementNode(private_tag__, int lineNumber, PExpressionNode value)
            : StatementNode(lineNumber)
            , m_value(value)
        {
        }

        static PReturnStatementNode Create(int lineNumber, PExpressionNode value)
        {
            return std::make_shared<ReturnStatementNode>(private_tag__(), lineNumber, value);
        }

        virtual StatementType GetStatementType() const override { return StatementType::ReturnStatement; }

        PExpressionNode GetValue() const { return m_value; }

        virtual void Accept(NodeVisitor &visitor) override { visitor.Visit(shared_from_this()); }
    };

    /****************************************************************/

    class WhileStatementNode :
        public StatementNode,
        public std::enable_shared_from_this<WhileStatementNode>
    {
    private:
        struct private_tag__ { explicit private_tag__() = default; };

        PExpressionNode m_condition;
        PCompoundStatementNode m_body;

    public:
        WhileStatementNode(private_tag__, int lineNumber, PExpressionNode condition, PCompoundStatementNode body)
             : StatementNode(lineNumber)
             , m_condition(condition)
             , m_body(body)
        {
        }

        static PWhileStatementNode Create(int lineNumber, PExpressionNode condition, PCompoundStatementNode body)
        {
            return std::make_shared<WhileStatementNode>(private_tag__(), lineNumber, condition, body);
        }

        virtual StatementType GetStatementType() const override { return StatementType::WhileStatement; }

        PExpressionNode GetCondition() const { return m_condition; }

        PStatementNode GetBody() const { return m_body; }

        virtual void Accept(NodeVisitor &visitor) override { visitor.Visit(shared_from_this()); }
    };

    /****************************************************************/

    class IfStatementNode :
        public StatementNode,
        public std::enable_shared_from_this<IfStatementNode>
    {
    private:
        struct private_tag__ { explicit private_tag__() = default; };

        PExpressionNode m_condition;
        PCompoundStatementNode m_truePart;
        PCompoundStatementNode m_falsePart;

    public:
        IfStatementNode(
            private_tag__,
            int lineNumber,
            PExpressionNode condition, 
            PCompoundStatementNode truePart,
            PCompoundStatementNode falsePart)
             : StatementNode(lineNumber)
             , m_condition(condition)
             , m_truePart(truePart)
             , m_falsePart(falsePart)
        {
        }

        static PIfStatementNode Create(
            int lineNumber,
            PExpressionNode condition, 
            PCompoundStatementNode truePart,
            PCompoundStatementNode falsePart)
        {
            return std::make_shared<IfStatementNode>(
                private_tag__(),
                lineNumber,
                condition, 
                truePart,
                falsePart);
        }

        virtual StatementType GetStatementType() const override { return StatementType::IfStatement; }

        PExpressionNode GetCondition() const { return m_condition; }

        PCompoundStatementNode GetTruePart() const { return m_truePart; }

        PCompoundStatementNode GetFalsePart() const { return m_falsePart; }

        virtual void Accept(NodeVisitor &visitor) override { visitor.Visit(shared_from_this()); }
    };

    /****************************************************************/
}

/*************************************************************************/

#endif /* OSBC_AST_STMT_H__ */

/*************************************************************************/
