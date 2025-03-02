/*************************************************************************/
/*************************************************************************/

#ifndef OSBC_AST_EXPR_H__
#define OSBC_AST_EXPR_H__

/*************************************************************************/

#include "../token.h"

/*************************************************************************/

namespace ast
{
    /****************************************************************/

    class ExpressionNode : public Node
    {
    private:
        PSymbol m_resultType;
        bool m_isConstant;

    protected:
        ExpressionNode(int lineNumber) 
            : Node(lineNumber)
            , m_resultType()
            , m_isConstant(false)
        {
        }

    public:
        virtual ~ExpressionNode() { }

        PSymbol GetResultType() const { return m_resultType; }

        void SetConstant(bool value) { m_isConstant = value; }

        virtual bool IsConstant() const { return m_isConstant; }

        void SetResultType(const PSymbol &resultType)
        {
            m_resultType = resultType;
        }
    };

    typedef std::shared_ptr<ExpressionNode> PExpressionNode;
    
    /****************************************************************/

    // Sort of a pseudo expression node, this holds a reference to a name.
    // A reference could appear at the begining of a statement or inside of an expression.
    // These often hold complex names.  E.g.  foo.bar.baz or moo[5].moof
    // For our bootstrap compiler it is just a simple name for now.

    // Note, the line number is of the actual usage, for the line number of the declaration use the value returned in the symbol.
    class ReferenceNode :
        public Node,
        public std::enable_shared_from_this<ReferenceNode>
    {
    private:
        struct private_tag__ { explicit private_tag__() = default; };

        Token m_ident;

    public:
        /* constructor */ ReferenceNode(private_tag__, Token ident)
            : Node(ident.lineNumber)
            , m_ident(ident)
        {
        }

        static PReferenceNode Create(Token ident)
        {
            return std::make_shared<ReferenceNode>(private_tag__(), ident);
        }

        const Token &GetIdent() const { return m_ident; }
        
        virtual void Accept(NodeVisitor &visitor) override { visitor.Visit(shared_from_this()); }

        std::string ToString() const
        {
            return m_ident.literal;
        }
    };

    /****************************************************************/

    class ConstantExpressionNode : 
        public ExpressionNode,
        public std::enable_shared_from_this<ConstantExpressionNode>
    {
    private:
        struct private_tag__ { explicit private_tag__() = default; };

        Token m_token;

    public:
        ConstantExpressionNode(private_tag__, Token token)
            : ExpressionNode(token.lineNumber)
            , m_token(token)
        {
        }

        virtual ~ConstantExpressionNode() { }

        static PConstantExpressionNode Create(Token token)
        {
            return std::make_shared<ConstantExpressionNode>(private_tag__(), token);
        }

        Token GetToken() const { return m_token; }

        virtual bool IsConstant() const override { return true; }

        virtual void Accept(NodeVisitor &visitor) override { visitor.Visit(shared_from_this()); }
    };

    typedef std::shared_ptr<ConstantExpressionNode> PConstantExpressionNode;

    /****************************************************************/

    class ReferenceExpressionNode :
        public ExpressionNode,
        public std::enable_shared_from_this<ReferenceExpressionNode>
    {
    private:
        struct private_tag__ { explicit private_tag__() = default; };

        PReferenceNode m_ref;
        PSymbol m_symbol;

    public:
        ReferenceExpressionNode(private_tag__, const PReferenceNode &ref)
            : ExpressionNode(ref->GetLineNumber())
            , m_ref(ref)
            , m_symbol()
        {
        }

        static PReferenceExpressionNode Create(const PReferenceNode &ref)
        {
            return std::make_shared<ReferenceExpressionNode>(private_tag__(), ref);
        }

        const PReferenceNode &GetReference() const { return m_ref; }

        void SetSymbol(PSymbol symbol)
        {
            //ASSERT(!m_symbol, "Symbol already set.");
            //ASSERT(symbol, "Invalid symbol.");

            m_symbol = symbol;
        }

        PSymbol GetSymbol() const { return m_symbol; }

        virtual void Accept(NodeVisitor &visitor) override { visitor.Visit(shared_from_this()); }
    };

    /****************************************************************/

    class CallStatementNode;
    typedef std::shared_ptr<CallStatementNode> PCallStatementNode;

    class CallExpressionNode : 
        public ExpressionNode,
        public std::enable_shared_from_this<CallExpressionNode>
    {
    private:
        struct private_tag__ { explicit private_tag__() = default; };

        PCallStatementNode m_call;

    public:
        CallExpressionNode(private_tag__, const PCallStatementNode &call);

        static PCallExpressionNode Create(const PCallStatementNode &call)
        {
            return std::make_shared<CallExpressionNode>(private_tag__(), call);
        }

        const PCallStatementNode GetCall() const { return m_call; }

        virtual bool IsConstant() const override { return false; }

        virtual void Accept(NodeVisitor &visitor) override { visitor.Visit(shared_from_this()); }
    };

    /****************************************************************/

    class BinaryExpressionNode :
        public ExpressionNode,
        public std::enable_shared_from_this<BinaryExpressionNode>
    {
    private:
        struct private_tag__ { explicit private_tag__() = default; };

        Token::Type m_operator;
        PExpressionNode m_left;
        PExpressionNode m_right;

    public:
        BinaryExpressionNode(
            private_tag__,
            int lineNumber,
            Token::Type op,
            PExpressionNode left,
            PExpressionNode right)
            : ExpressionNode(lineNumber)
            , m_operator(op)
            , m_left(left)
            , m_right(right)
        {
        }

        virtual ~BinaryExpressionNode() { }

        static PBinaryExpressionNode Create(
            int lineNumber,
            Token::Type op,
            PExpressionNode left,
            PExpressionNode right)
        {
            return std::make_shared<BinaryExpressionNode>(private_tag__(), lineNumber, op, left, right);
        }

        Token::Type GetOperator() const { return m_operator; }
        
        const PExpressionNode GetLeft() const { return m_left; }
        const PExpressionNode GetRight() const { return m_right; }

        virtual void Accept(NodeVisitor &visitor) override { visitor.Visit(shared_from_this()); }
    };

    /****************************************************************/

    class UnaryExpressionNode :
        public ExpressionNode,
        public std::enable_shared_from_this<UnaryExpressionNode>
    {
    private:
        struct private_tag__ { explicit private_tag__() = default; };

        Token::Type m_operator;
        PExpressionNode m_sub;

    public:
        UnaryExpressionNode(private_tag__, int lineNumber, Token::Type op, PExpressionNode sub)
            : ExpressionNode(lineNumber)
            , m_operator(op)
            , m_sub(sub)
        {
        }

        static PUnaryExpressionNode Create(int lineNumber, Token::Type op, PExpressionNode sub)
        {
            return std::make_shared<UnaryExpressionNode>(private_tag__(), lineNumber, op, sub);
        }

        Token::Type GetOperator() const { return m_operator; }

        const PExpressionNode GetSub() const { return m_sub; }

        virtual void Accept(NodeVisitor &visitor) override { visitor.Visit(shared_from_this()); }
    };

    /****************************************************************/
}

/*************************************************************************/

#endif /* OSBC_AST_EXPR_H__ */

/*************************************************************************/
