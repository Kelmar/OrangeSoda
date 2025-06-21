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

    node->GetLeft()->Accept(*this);
    Value *left = m_valueResult;

    node->GetRight()->Accept(*this);
    Value *right = m_valueResult;

    Token::Type op = node->GetOperator();

    switch (op)
    {
    case (Token::Type)'+':
        m_valueResult = m_builder->CreateAdd(left, right, "addtmp");
        break;

    case (Token::Type)'-': 
        m_valueResult = m_builder->CreateSub(left, right, "subtmp");
        break;

    case (Token::Type)'*':
        m_valueResult = m_builder->CreateMul(left, right, "multmp");
        break;

    case (Token::Type)'/': 
        m_valueResult = m_builder->CreateSDiv(left, right, "divtmp");
        break;

    case (Token::Type)'%': 
        m_valueResult = m_builder->CreateSRem(left, right, "modtmp");
        break;

    case (Token::Type)'&':
        m_valueResult = m_builder->CreateAnd(left, right, "andtmp");
        break;

    case (Token::Type)'|': 
        m_valueResult = m_builder->CreateOr (left, right, "ortmp");
        break;

    case (Token::Type)'^': 
        m_valueResult = m_builder->CreateXor(left, right, "xortmp");
        break;

    case Token::Type::LeftShift: 
        m_valueResult = m_builder->CreateShl(left, right, "shltmp");
        break;

    case Token::Type::RightShift: 
        m_valueResult = m_builder->CreateAShr(left, right, "shrtmp");
        break;

    case (Token::Type)'>': 
        m_valueResult = m_builder->CreateCmp(CmpInst::ICMP_SGT, left, right, "gttmp");
        break;

    case (Token::Type)'<': 
        m_valueResult = m_builder->CreateCmp(CmpInst::ICMP_SLT, left, right, "lttmp");
        break;

    case Token::Type::Equality:
        m_valueResult = m_builder->CreateCmp(CmpInst::ICMP_EQ, left, right, "eqtmp");
        break;

    case Token::Type::NotEqual:
        m_valueResult = m_builder->CreateCmp(CmpInst::ICMP_NE, left, right, "neqtmp");
        break;

    case Token::Type::LessEqual: 
        m_valueResult = m_builder->CreateCmp(CmpInst::ICMP_SLE, left, right, "letmp");
        break;

    case Token::Type::GreatEqual: 
        m_valueResult = m_builder->CreateCmp(CmpInst::ICMP_SGE, left, right, "getmp");
        break;

    default:
        throw std::runtime_error(fmt::format("Invalid operator {0}\r\n", op));
    }
}

/*************************************************************************/

void CodeGen::Visit(ast::PUnaryExpressionNode node)
{
    node->GetSub()->Accept(*this);

    Token::Type op = node->GetOperator();

    switch (op)
    {
    case (Token::Type)'-':
        m_valueResult = m_builder->CreateNeg(m_valueResult, "negtmp");
        break;

    case (Token::Type)'!':
    case (Token::Type)'~': 
        m_valueResult = m_builder->CreateNot(m_valueResult, "nottmp");
        break;

    case (Token::Type)'+': // Effectively a do nothing operator
    default:
        break;
    }
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
    // Restore parent block on exit.
    auto parentBlock = m_currentBlock;
    auto restore = defer([=, this] () { m_currentBlock = parentBlock; });

    m_currentBlock = BasicBlock::Create(*m_context, "entry", m_currentFunction);

    m_builder->SetInsertPoint(m_currentBlock);

    VisitAll(node->GetStatements());

    //m_blockResult = m_currentBlock;
}

/*************************************************************************/

void CodeGen::Visit(ast::PAssignmentStatementNode node)
{
    node->GetExpression()->Accept(*this);
    Value *expr = m_valueResult;
    (void)expr;

#if 0
    auto to = node->GetReference()->Accept(*this);
    
    StoreInst *inst = m_builder->CreateStore(expr, to);
    AddInstruction(inst);
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
    node->GetCondition()->Accept(*this);
    Value *cond = m_valueResult;

    if (!cond)
        return;

    Value *trueVal = ConstantInt::get(*m_context, APInt::getZero(1));
    Value *ifCond = m_builder->CreateICmpNE(cond, trueVal, "ifcond");

    llvm::Function *func = m_builder->GetInsertBlock()->getParent();

    BasicBlock *thenBlock = BasicBlock::Create(*m_context, "then", func);
    BasicBlock *elseBlock = BasicBlock::Create(*m_context, "else");

    BasicBlock *mergeBlock = BasicBlock::Create(*m_context, "ifcont");

    m_builder->CreateCondBr(ifCond, thenBlock, elseBlock);

    // Then part
    m_builder->SetInsertPoint(thenBlock);

    node->GetTruePart()->Accept(*this);
    Value *then = m_valueResult;

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
        //InsertInstruction(elseBlock);
        func->insert(func->end(), elseBlock);
        m_builder->SetInsertPoint(elseBlock);
        
        //if (elsePart) // Not sure why this if was here...
        {
            elsePart->Accept(*this);
            else_ = m_valueResult;
        }

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
    llvm::Type *retType = llvm::Type::getVoidTy(*m_context);
    std::vector<llvm::Type *> args;

    FunctionType *funcType = FunctionType::get(retType, args, false);

    PSymbol name = node->GetSymbol();

    m_currentFunction = Function::Create(funcType, Function::ExternalLinkage, name->name(), *m_module);

    node->GetBody()->Accept(*this);

    if (verifyFunction(*m_currentFunction))
        abort(); // Function has errors

    // Run function pass optimizations.
    //m_fpm->run(*m_currentFunction, *m_fam);

    m_currentFunction = nullptr; // Pedantic clear
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
