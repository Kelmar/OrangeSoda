/*************************************************************************/
/*************************************************************************/

#ifndef OSBC_CODEGEN_H__
#define OSBC_CODEGEN_H__

/*************************************************************************/

#include "ast.h"

/*************************************************************************/

class CodeGen : public ast::NodeVisitor
{
private:
    PSymbolTable m_symbolTable;
    int m_autoLabelId;

    std::string NewAutoLabel();

public:
    /* constructor */ CodeGen();
    virtual ~CodeGen();

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

#endif /* OSBC_CODEGEN_H__ */

/*************************************************************************/
