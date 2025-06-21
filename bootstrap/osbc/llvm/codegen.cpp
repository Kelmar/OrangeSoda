/*************************************************************************/
/*************************************************************************/

#include "../osbc.h"
#include "llvm.h"

#include "codegen.h"

using namespace llvm;

/*************************************************************************/

namespace os_llvm
{

CodeGen::CodeGen()
{
    m_context = std::make_unique<LLVMContext>();
    m_module = std::make_unique<Module>("my cool jit", *m_context);
    m_builder = std::make_unique<IRBuilder<>>(*m_context);

#if 0
    m_fpm = std::make_unique<FunctionPassManager>();
    m_lam = std::make_unique<LoopAnalysisManager>();
    m_fam = std::make_unique<FunctionAnalysisManager>();
    m_cgam = std::make_unique<CGSCCAnalysisManager>();
    m_mam = std::make_unique<ModuleAnalysisManager>();
    m_pic = std::make_unique<PassInstrumentationCallbacks>();

    m_si = std::make_unique<StandardInsturmentations>(m_context, /* debugg logging*/true);

    m_si->registerCallbacks(*m_pic, *m_mam);

    // Add transform passes
    // Do simple "peephole" optimizations and bit-twiddling.
    m_fam->addPass(InstCombinePass());

    // Reassociate expressions.
    m_fam->addPass(ReassociatePass());

    // Eliminate common sub-expressions.
    f_fam->addPass(GVNPass());

    PassBuilder pb;
    pb.registerModuleAnalyses(*m_mam);
    pb.registerFunctionAnalyses(*m_fam);
    pb.crossRegisterProxies(*m_lam, *m_fam, *m_cgam, *m_mam);

#endif
}

CodeGen::~CodeGen()
{
}

/*************************************************************************/

#if 0
Value *CodeGen::process(const ast::ReferenceNode &refNode)
{
    (void)refNode;
    return nullptr;
}
#endif

/*************************************************************************/

void CodeGen::Visit(ast::PModuleNode node)
{
    VisitAll(node->GetStatements());
}

/*************************************************************************/
// Expressions
/*************************************************************************/

void CodeGen::Visit(ast::PReferenceNode node)
{
    (void)node;
}

/*************************************************************************/

void CodeGen::Visit(ast::PConstantExpressionNode node)
{
    const std::string &literal = node->GetToken().literal;
    
    PSymbol resultType = node->GetResultType();
    (void)resultType;

#if 0
    switch (resultType)
    {
    case Token::Type::BOOL_CONST:
        {
            APInt val = literal == "true" ? APInt::getMaxValue(1) : APInt::getZero(1);
            //return ConstantInt::get(*m_context, val);
        }
        break;

    case Token::Type::INT_CONST:
        {
            // APInt(bits, value, signed);
            APInt val(32, std::stoi(literal), true);
            //return ConstantInt::get(*m_context, val);
        }
        break;

    case Token::Type::CHAR_CONST:
        {
            throw std::runtime_error("Character constants not supported yet.");


            //APInt val(8, literal[0], true);
            //llvm::ConstantInt::get(*m_context, val);
        }
        break;

    case Token::Type::STR_CONST:
        {
            throw std::runtime_error("String constants not supported yet.");
        }
        break;

    default:
        std::string errMsg = fmt::format("Unknown constant token type {0}", resultType);
        throw std::runtime_error(errMsg);
    }
#endif
}

/*************************************************************************/

void CodeGen::Visit(ast::PReferenceExpressionNode node)
{
    (void)node;
}

/*************************************************************************/

void CodeGen::Visit(ast::PCallExpressionNode node)
{
    (void)node;
}

/*************************************************************************/

void CodeGen::Visit(ast::PBinaryExpressionNode node)
{
    (void)node;

#if 0
    auto l = node->GetLeft()->Accept(*this);
    auto r = node->GetRight()->Accept(*this);

    Token::Type op = node->GetOperator();

    switch (op)
    {
    case (Token::Type)'+': return m_builder->CreateAdd(l, r, "addtmp");
    case (Token::Type)'-': return m_builder->CreateSub(l, r, "subtmp");
    case (Token::Type)'*': return m_builder->CreateMul(l, r, "multmp");
    case (Token::Type)'/': return m_builder->CreateSDiv(l, r, "divtmp");
    case (Token::Type)'%': return m_builder->CreateSRem(l, r, "modtmp");
    case (Token::Type)'&': return m_builder->CreateAnd(l, r, "andtmp");
    case (Token::Type)'|': return m_builder->CreateOr (l, r, "ortmp");
    case (Token::Type)'^': return m_builder->CreateXor(l, r, "xortmp");
    case Token::Type::LeftShift: return m_builder->CreateShl(l, r, "shltmp");
    case Token::Type::RightShift: return m_builder->CreateAShr(l, r, "shrtmp");

    case (Token::Type)'>': return m_builder->CreateCmp(CmpInst::ICMP_SGT, l, r, "gttmp");
    case (Token::Type)'<': return m_builder->CreateCmp(CmpInst::ICMP_SLT, l, r, "lttmp");
    case Token::Type::Equality: return m_builder->CreateCmp(CmpInst::ICMP_EQ, l, r, "eqtmp");
    case Token::Type::NotEqual: return m_builder->CreateCmp(CmpInst::ICMP_NE, l, r, "neqtmp");
    case Token::Type::LessEqual: return m_builder->CreateCmp(CmpInst::ICMP_SLE, l, r, "letmp");
    case Token::Type::GreatEqual: return m_builder->CreateCmp(CmpInst::ICMP_SGE, l, r, "getmp");

    default:
        fmt::print("Invalid operator {0}\r\n", op);
        abort();
    }
#endif
}

/*************************************************************************/

void CodeGen::Visit(ast::PUnaryExpressionNode node)
{
    (void)node;

#if 0
    auto sub = node->GetSub()->Accept(*this);

    Token::Type op = node->GetOperator();

    switch (op)
    {
    case (Token::Type)'-': return m_builder->CreateNeg(sub, "negtmp");

    case (Token::Type)'!':
    case (Token::Type)'~': return m_builder->CreateNot(sub, "nottmp");

    case (Token::Type)'+': // Effectively a do nothing operator
    default:
        return sub;
    }
#endif
}

/*************************************************************************/

void CodeGen::Visit(ast::PVariableDeclStatementNode node)
{
    if (node->IsConstant())
    {
        // TODO: Perform constant evaluation here.
    }
}

/*************************************************************************/

void CodeGen::Visit(ast::PCompoundStatementNode node)
{
    (void)node;

#if 0
    BasicBlock *bb = BasicBlock::Create(*m_context, "entry", fn);

    m_builder->SetInsertPoint(bb);

    VisitAll(node->GetStatements());
#endif
}

/*************************************************************************/

void CodeGen::Visit(ast::PAssignmentStatementNode node)
{
    (void)node;

#if 0
    auto expr = node->GetExpression()->Accept(*this);
    auto to = node->GetReference()->Accept(*this);
    
    //return m_builder->CreateStore(expr, to);
#endif
}

/*************************************************************************/

void CodeGen::Visit(ast::PCallStatementNode node)
{
    (void)node;
}

/*************************************************************************/

void CodeGen::Visit(ast::PReturnStatementNode node)
{
    (void)node;
}

/*************************************************************************/

void CodeGen::Visit(ast::PWhileStatementNode node)
{
    (void)node;
}

/*************************************************************************/

void CodeGen::Visit(ast::PIfStatementNode node)
{
    (void)node;

#if 0
    Value *cond = node->GetCondition()->Accept(*this);

    if (!cond)
        return nullptr;

    Value *trueVal = ConstantInt::get(*m_context, APInt::getZero(1));
    Value *ifCond = m_builder->CreateICmpNE(cond, trueVal, "ifcond");

    llvm::Function *func = m_builder->GetInsertBlock()->getParent();

    BasicBlock *thenBlock = BasicBlock::Create(*m_context, "then", func);
    BasicBlock *elseBlock = BasicBlock::Create(*m_context, "else");

    BasicBlock *mergeBlock = BasicBlock::Create(*m_context, "ifcont");

    m_builder->CreateCondBr(ifCond, thenBlock, elseBlock);

    // Then part
    m_builder->SetInsertPoint(thenBlock);

    Value *then = node->GetTruePart()->Accept(*this);

    if (!then)
        return;

    m_builder->CreateBr(mergeBlock);
    // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
    thenBlock = m_builder->GetInsertBlock();

    // Else part
    auto elsePart = node->GetFalsePart();
    Value *else_ = nullptr;

    if (elsePart)
    {
        m_builder->func->insert(func->end(), elseBlock);
        m_builder->SetInsertPoint(elseBlock);
        
        if (elsePart)
            else_ = elsePart->Accept(*this);

        m_builder->CreateBr(mergeBlock);
        // Codegen of 'Else' can change the current block, update ElseBB for the PHI.
        else_ = m_builder->GetInsertBlock();
    }

    // Emit the merged block
    func->insert(func->end(), mergeBlock);
    m_builder->SetInsertPoint(mergeBlock);

    PHINode *pn = m_builder->CreatePHI(llvm::Type::getInt32Ty(*m_context), 2, "iftmp");

    pn->addIncoming(then, thenBlock);

    if (else_)
        pn->addIncoming(else_, elseBlock);
#endif
}

/*************************************************************************/
// Top Level Statements
/*************************************************************************/

void CodeGen::Visit(ast::PImportNode node)
{
    (void)node;
}

/*************************************************************************/

void CodeGen::Visit(ast::PGlobalVariableNode node)
{
    (void)node;
}

/*************************************************************************/

void CodeGen::Visit(ast::PParameterDeclNode node)
{
    (void)node;
}

/*************************************************************************/

void CodeGen::Visit(ast::PFunctionNode node)
{
    (void)node;

#if 0
    llvm::Type *retType = llvm::Type::getVoidTy(*m_context);
    std::vector<llvm::Type *> args;

    FunctionType *ft = FunctionType::get(retType, args, false);

    PSymbol name = stmtNode.GetName();

    Function *fn = Function::Create(ft, Function::ExternalLinkage, name->name, *m_module);

    node->GetBody()->Accept(*this);

    verifyFunction(*fn);

    // Run function pass optimizations.
    //m_fpm->run(*fn, *m_fam);
#endif
}

/*************************************************************************/

#if 0
void CodeGen::generate(ast::PNode root)
{
    visit(root);
    m_module->print(llvm::errs(), nullptr);
}
#endif

/*************************************************************************/

}; // namespace os_llvm

/*************************************************************************/
