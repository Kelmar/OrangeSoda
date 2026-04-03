/*************************************************************************/
/*************************************************************************/

#ifndef OSBC_AST_VISITOR_H__
#define OSBC_AST_VISITOR_H__

/*************************************************************************/

namespace ast
{
    /****************************************************************/

    class ReferenceNode;
    class ModuleNode;

    // Expressions
    class ConstantExpressionNode;
    class ReferenceExpressionNode;
    class CallExpressionNode;
    class BinaryExpressionNode;
    class UnaryExpressionNode;

    // Statements
    class StatementNode;

    class VariableDeclStatementNode;
    class CompoundStatementNode;
    class AssignmentStatementNode;
    class CallStatementNode;
    class ReturnStatementNode;
    class WhileStatementNode;
    class IfStatementNode;

    // Top Level Statements
    class ImportNode;
    class GlobalVariableNode;
    class ParameterDeclNode;
    class FunctionNode;

    /****************************************************************/

    typedef std::shared_ptr<ReferenceNode> PReferenceNode;
    typedef std::shared_ptr<ModuleNode> PModuleNode;
    
    // Expressions
    typedef std::shared_ptr<ConstantExpressionNode> PConstantExpressionNode;
    typedef std::shared_ptr<ReferenceExpressionNode> PReferenceExpressionNode;
    typedef std::shared_ptr<CallExpressionNode> PCallExpressionNode;
    typedef std::shared_ptr<BinaryExpressionNode> PBinaryExpressionNode;
    typedef std::shared_ptr<UnaryExpressionNode> PUnaryExpressionNode;

    // Statements
    typedef std::shared_ptr<StatementNode> PStatementNode;

    typedef std::shared_ptr<VariableDeclStatementNode> PVariableDeclStatementNode;
    typedef std::shared_ptr<CompoundStatementNode> PCompoundStatementNode;
    typedef std::shared_ptr<AssignmentStatementNode> PAssignmentStatementNode;
    typedef std::shared_ptr<CallStatementNode> PCallStatementNode;
    typedef std::shared_ptr<ReturnStatementNode> PReturnStatementNode;
    typedef std::shared_ptr<WhileStatementNode> PWhileStatementNode;
    typedef std::shared_ptr<IfStatementNode> PIfStatementNode;

    // Top Level Statements
    typedef std::shared_ptr<ImportNode> PImportNode;
    typedef std::shared_ptr<GlobalVariableNode> PGlobalVariableNode;
    typedef std::shared_ptr<ParameterDeclNode> PParameterDeclNode;
    typedef std::shared_ptr<FunctionNode> PFunctionNode;

    /****************************************************************/

    class Node;

    /****************************************************************/

    struct IModuleVisitor
    {
        virtual void Visit(PModuleNode) = 0;
    };

    /****************************************************************/

    struct IExpressionVisitor
    {
        virtual void Visit(PConstantExpressionNode) = 0;
        virtual void Visit(PReferenceExpressionNode) = 0;
        virtual void Visit(PCallExpressionNode) = 0;
        virtual void Visit(PBinaryExpressionNode) = 0;
        virtual void Visit(PUnaryExpressionNode) = 0;
    };

    /****************************************************************/

    struct IStatementVisitor
    {
        virtual void Visit(PVariableDeclStatementNode) = 0;
        virtual void Visit(PCompoundStatementNode) = 0;
        virtual void Visit(PAssignmentStatementNode) = 0;
        virtual void Visit(PCallStatementNode) = 0;
        virtual void Visit(PReturnStatementNode) = 0;
        virtual void Visit(PWhileStatementNode) = 0;
        virtual void Visit(PIfStatementNode) = 0;
    };

    /****************************************************************/

    struct ITopLevelVisitor
    {
        virtual void Visit(PImportNode) = 0;
        virtual void Visit(PGlobalVariableNode) = 0;
        virtual void Visit(PParameterDeclNode) = 0;
        virtual void Visit(PFunctionNode) = 0;
    };

    /****************************************************************/

    class NodeVisitor
        : public ITopLevelVisitor
        , public IStatementVisitor
        , public IExpressionVisitor
        , public IModuleVisitor
    {
    protected:
        NodeVisitor() { }

        template <typename TNode>
        void Visit(TNode &node)
        {
            static_assert(std::is_base_of<Node, TNode>::value, "TNode must be an AST node!");
            node.Accept(*this);
        }

        template <typename TNode>
        void Visit(std::shared_ptr<TNode> node)
        {
            static_assert(std::is_base_of<Node, TNode>::value, "TNode must be an AST node!");
            node->Accept(*this);
        }

        template <typename TNode>
        void VisitAll(const std::vector<std::shared_ptr<TNode>> &nodes)
        {
            static_assert(std::is_base_of<Node, TNode>::value, "TNode must be an AST node!");

            for (auto node : nodes)
            {
                ASSERT(node, "Invalid node in visitor list");
                node->Accept(*this);
            }
        }

    public:
        virtual ~NodeVisitor() { }

        // Root node
        virtual void Visit(PModuleNode node) = 0;

        // Expressions
        virtual void Visit(PReferenceNode node) = 0;
        virtual void Visit(PConstantExpressionNode node) = 0;
        virtual void Visit(PReferenceExpressionNode node) = 0;
        virtual void Visit(PCallExpressionNode node) = 0;
        virtual void Visit(PBinaryExpressionNode node) = 0;
        virtual void Visit(PUnaryExpressionNode node) = 0;

        // Statements
        virtual void Visit(PVariableDeclStatementNode node) = 0;
        virtual void Visit(PCompoundStatementNode node) = 0;
        virtual void Visit(PAssignmentStatementNode node) = 0;
        virtual void Visit(PCallStatementNode node) = 0;
        virtual void Visit(PReturnStatementNode node) = 0;
        virtual void Visit(PWhileStatementNode node) = 0;
        virtual void Visit(PIfStatementNode node) = 0;

        // Top Level Statements
        virtual void Visit(PImportNode node) = 0;
        virtual void Visit(PGlobalVariableNode node) = 0;
        virtual void Visit(PParameterDeclNode node) = 0;
        virtual void Visit(PFunctionNode node) = 0;
    };

    typedef std::shared_ptr<NodeVisitor> PNodeVisitor;
}

/*************************************************************************/

#endif /* OSBC_AST_VISITOR_H__ */

/*************************************************************************/
