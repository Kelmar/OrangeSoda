/*************************************************************************/
/*************************************************************************/

#ifndef OSBC_LLVM_CODEGEN_H__
#define OSBC_LLVM_CODEGEN_H__

/*************************************************************************/

#include "ast.h"

/*************************************************************************/

class CodeGen
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

    llvm::Value *process(const ast::ReferenceNode &refNode);
    void process(const ast::ModuleNode &modNode);

    // Expressions
    llvm::Value *process(const ast::ConstantExpressionNode &exprNode);
    llvm::Value *process(const ast::ReferenceExpressionNode &exprNode);
    llvm::Value *process(const ast::BinaryExpressionNode &exprNode);
    llvm::Value *process(const ast::UnaryExpressionNode &exprNode);

    // Statements
    llvm::Value *process(const ast::CompoundStatementNode &stmtNode, llvm::Function *fn = nullptr);
    llvm::Value *process(const ast::AssignmentStatementNode &stmtNode);
    llvm::Value *process(const ast::WhileStatementNode &stmtNode);
    llvm::Value *process(const ast::IfStatementNode &stmtNode);

    // Top Level Statements
    llvm::Value *process(const ast::FunctionNode &stmtNode);

    llvm::Value *visit(ast::PNode node);

public:
    /* constructor */ CodeGen();
    virtual ~CodeGen();

    void generate(ast::PNode root);
};

/*************************************************************************/

#endif /* OSBC_CODEGEN_H__ */

/*************************************************************************/
