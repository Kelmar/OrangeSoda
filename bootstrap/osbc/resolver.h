/*************************************************************************/
/*************************************************************************/

#ifndef OS_RESOLVER_H__
#define OS_RESOLVER_H__

/*************************************************************************/

#include "bootstrap.h"

#include "ast.h"
#include "scope.h"

/*************************************************************************/
/**
 * @brief Name resolving.
 *
 * @details
 * This pass validates that all used references are declared.
 * 
 * Also handles type validation and other correctness checks.
 */
class Resolver : public ast::NodeVisitor
{
private:
    /// @brief We need to make three passes over the top level statements.
    int m_pass;

    PSymbol m_voidType;

    PSymbolTable m_symbolTable;

    /// @brief Function that we're currently validating.
    ast::PFunctionNode m_currentFun;

    PSymbol FindOrDie(int lineNumber, ast::PReferenceNode ref, const std::string &errFormat);

    void ResolveBase(std::shared_ptr<ast::DeclNode> decl);

public:
    Resolver();
    virtual ~Resolver();
    
    virtual void Visit(ast::PModuleNode node) override;

    // Expressions
    virtual void Visit(ast::PReferenceNode node) override;
    virtual void Visit(ast::PConstantExpressionNode node) override;
    virtual void Visit(ast::PReferenceExpressionNode node) override;
    virtual void Visit(ast::PCallExpressionNode node) override;
    virtual void Visit(ast::PBinaryExpressionNode node) override;
    virtual void Visit(ast::PUnaryExpressionNode node) override;

    // Statements
    virtual void Visit(ast::PVariableDeclStatementNode node) override;
    virtual void Visit(ast::PCompoundStatementNode node) override;
    virtual void Visit(ast::PAssignmentStatementNode node) override;
    virtual void Visit(ast::PCallStatementNode node) override;
    virtual void Visit(ast::PReturnStatementNode node) override;
    virtual void Visit(ast::PWhileStatementNode node) override;
    virtual void Visit(ast::PIfStatementNode node) override;

    // Top Level Statements
    virtual void Visit(ast::PImportNode node) override;
    virtual void Visit(ast::PGlobalVariableNode node) override;
    virtual void Visit(ast::PParameterDeclNode node) override;
    virtual void Visit(ast::PFunctionNode node) override;
};

/*************************************************************************/

#endif /* OS_RESOLVER_H__ */

/*************************************************************************/
