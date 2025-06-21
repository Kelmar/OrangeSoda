/*************************************************************************/
/*************************************************************************/

#ifndef OSBC_LLVM_CODEGEN_H__
#define OSBC_LLVM_CODEGEN_H__

/*************************************************************************/

#include "llvm.h"
#include "../ast.h"

/*************************************************************************/

namespace os_llvm
{

/*************************************************************************/

class CodeGen : public ast::NodeVisitor
{
private:
    std::unique_ptr<llvm::LLVMContext> m_context;
    std::unique_ptr<llvm::Module> m_module;
    std::unique_ptr<llvm::IRBuilder<>> m_builder;

#if 0
    // Compiler passes
    std::unique<llvm::FunctionPassManager> m_fpm;
    std::unique<llvm::LoopAnalysisManager> m_lam;
    std::unique<llvm::FunctionAnalysisManager> m_fam;
    std::unique<llvm::CGSCCAnalysisManager> m_cgam;
    std::unique<llvm::ModuleAnalysisManager> m_mam;
    std::unique<llvm::PassInstrumentationCallbacks> m_pic;

    std::unique<llvm::StandardInsturmentations> m_si;
#endif

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

}; // namespace os_llvm

/*************************************************************************/

#endif /* OSBC_CODEGEN_H__ */

/*************************************************************************/
