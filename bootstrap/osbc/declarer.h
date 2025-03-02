/*************************************************************************/
/*************************************************************************/

#ifndef OS_DECLARER_H__
#define OS_DECLARER_H__

/*************************************************************************/

#include "bootstrap.h"

#include "ast.h"
#include "scope.h"

#include "symtable.h"

/*************************************************************************/
/**
 * @brief Declaration pass
 *
 * @details
 * This pass walks the AST and adds the symbol defintions to the symbol tables.
 * 
 * It does not attempt to resolve any symbols at this stage, other than to 
 * validate that the symbol isn't already defined.
 * 
 * The main purpose is to get all the declarations done upfront so the Resolver
 * can go about making sure that all references to the various symbols are
 * actually defined.
 */
class Declarer : public ast::NodeVisitor
{
private:
    PSymbolTable m_symbolTable;

    PSymbol AddPrimitive(const std::string &name);
    PSymbol AddBuiltIn(const std::string &name, PSymbol baseType);

    void LoadBuiltIns();

    void VerifyUndefined(const Token &ident, Scoping scoping = Scoping::Normal);

public:
    Declarer();
    virtual ~Declarer();
    
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

#endif /* OS_DECLARER_H__ */

/*************************************************************************/
