/*************************************************************************/
/*************************************************************************/

#include "../osbc.h"
#include "llvm.h"

#include "codegen.h"

using namespace llvm;

/*************************************************************************/

namespace os_llvm
{

CodeGen::CodeGen(std::string_view sourceFileName)
    : m_currentFunction()
    , m_llvmFunction(nullptr)
    , m_llvmBlock(nullptr)
    , m_valueResult(nullptr)
{
    m_context = std::make_unique<LLVMContext>();
    m_module = std::make_unique<Module>(sourceFileName, *m_context);
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

    // Re-associate expressions.
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

llvm::Type *CodeGen::TranslateType(ast::PReferenceNode node)
{
    //(void)node;
    //return llvm::Type::getVoidTy(*m_context);

    Token ident = node->GetIdent();

    switch (ident.type)
    {
    case Token::Type::VOID:
        return llvm::Type::getVoidTy(*m_context);

    case Token::Type::INT:
        return llvm::Type::getInt32Ty(*m_context);

    case Token::Type::BOOL:
        return llvm::Type::getInt8Ty(*m_context);

    case Token::Type::CHAR:
        //return llvm::Type::getByteTy(*m_context);

    case Token::Type::STRING:
        //return llvm::Type::getStringTy(*m_context);

    default:
        std::string errMsg = fmt::format("BUG: Unsupported type: {0}", ident.literal);
        throw std::runtime_error(errMsg);
    }
}

/*************************************************************************/
// Root
/*************************************************************************/

void CodeGen::Visit(ast::PModuleNode node)
{
    VisitAll(node->GetStatements());
    m_module->print(llvm::errs(), nullptr);
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
    
    switch (resultType->constType)
    {
    case Token::Type::VOID:
        throw std::runtime_error("BUG: Got a void constant?");
        break;

    case Token::Type::BOOL_CONST:
        {
            APInt val = (literal == "true") ? APInt::getMaxValue(1) : APInt::getZero(1);
            m_valueResult = ConstantInt::get(*m_context, val);
        }
        break;

    case Token::Type::INT_CONST:
        {
            // APInt(bits, value, signed);
            APInt val(32, std::stoi(literal), true);
            m_valueResult = ConstantInt::get(*m_context, val);
        }
        break;

    case Token::Type::CHAR_CONST:
        {
            throw std::runtime_error("Character constants not supported yet.");

            /*
             * TODO: Not entirely sure how to handle character constants internally yet.    
             *
             * The language should probably treat all characters as UTF-8 or some other
             * similar encoding, but that can contain mutliple bytes per character.
             */

            // APInt(bits, value, signed);
            //APInt val(8, literal[0], true);
            //m_valueResult = ConstantInt::get(*m_context, val);
        }
        break;

    case Token::Type::STR_CONST:
        {
            throw std::runtime_error("String constants not supported yet.");

            // Same problem as character, each character can have multiple bytes.

            //m_valueResult = ConstantVector::get(*m_context, val);
        }
        break;

    default:
        std::string errMsg = fmt::format("Unknown constant token type {0}", *resultType);
        throw std::runtime_error(errMsg);
    }
}

/*************************************************************************/

void CodeGen::Visit(ast::PReferenceExpressionNode node)
{
    PSymbol symbol = node->GetSymbol();

    // TODO: Replace with actual symbol lookup.
    APInt val(32, 0, true);
    m_valueResult = ConstantInt::get(*m_context, val);
}

/*************************************************************************/

void CodeGen::Visit(ast::PCallExpressionNode node)
{
    (void)node;
#if 0
    auto statementNode = node->GetCall();

    auto func = statementNode->GetReference();

    // Push parameters to stack
    VisitAll(statementNode->GetParameters());

    m_valueResult = m_builder->CreateCall(func, args, "calltmp");
#endif
}

/*************************************************************************/

void CodeGen::Visit(ast::PBinaryExpressionNode node)
{
    Token::Type op = node->GetOperator();

    ast::PExpressionNode l = node->GetLeft();
    ast::PExpressionNode r = node->GetRight();

    ast::ExpressionNode *dbg_l = l.get();
    ast::ExpressionNode *dbg_r = r.get();

    (void)dbg_l;
    (void)dbg_r;

    l->Accept(*this);
    Value *left = m_valueResult;

    r->Accept(*this);
    Value *right = m_valueResult;

    if (!left || !right)
        throw std::runtime_error("Binary expression missing sides!");

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
        m_valueResult = m_builder->CreateOr(left, right, "ortmp");
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

    case Token::Type::LogicalAnd:
        m_valueResult = m_builder->CreateLogicalAnd(left, right, "landtmp");
        break;

    case Token::Type::LogicalOr:
        m_valueResult = m_builder->CreateLogicalOr(left, right, "lortmp");
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
    llvm::BasicBlock *parentBlock = m_llvmBlock;

    auto restore = defer([=, this] ()
    {
        m_llvmBlock = parentBlock;

        if (m_llvmBlock)
            m_builder->SetInsertPoint(m_llvmBlock);
    });

    m_llvmBlock = BasicBlock::Create(*m_context, "", m_llvmFunction);

    m_builder->SetInsertPoint(m_llvmBlock);

    VisitAll(node->GetStatements());

    if (parentBlock)
        return;

    auto type = m_llvmFunction->getReturnType();

    if (type->isVoidTy())
        m_builder->CreateRetVoid();
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
    auto expr = node->GetValue();

    if (expr)
    {
        expr->Accept(*this);
        m_builder->CreateRet(m_valueResult);
    }
    else
    {
        m_builder->CreateRetVoid();
    }
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
    m_currentFunction = node;

    llvm::Type *retType = TranslateType(node->GetType());

    auto params = node->GetParameters();

    std::vector<llvm::Type *> llvmParams;
    llvmParams.reserve(params.size());

    for (auto param : params)
        llvmParams.push_back(TranslateType(param->GetType()));

    FunctionType *funcType = FunctionType::get(retType, llvmParams, false);

    PSymbol name = node->GetSymbol();

    m_llvmFunction = Function::Create(funcType, Function::ExternalLinkage, name->name(), *m_module);

    int i = 0;
    for (auto &arg : m_llvmFunction->args())
    {
        auto parameter = params[i++];
        Token ident = parameter->GetIdent();
        arg.setName(ident.literal);

        parameter->codeGen = &arg;
    }

    node->GetBody()->Accept(*this);

    if (verifyFunction(*m_llvmFunction, &llvm::errs()))
        abort(); // Function has errors

    // Run function pass optimizations.
    //m_fpm->run(*m_currentFunction, *m_fam);

    m_currentFunction = nullptr; // Pedantic clear
    m_llvmFunction = nullptr;
}

/*************************************************************************/

}; // namespace os_llvm

/*************************************************************************/
