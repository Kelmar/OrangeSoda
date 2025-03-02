/*************************************************************************/
/*************************************************************************/

#include "osbc.h"
#include "llvm/llvm.h"

#include "llvm/codegen.h"

using namespace llvm;

/*************************************************************************/

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

Value *CodeGen::process(const ast::ReferenceNode &refNode)
{
    (void)refNode;
    return nullptr;
}

/*************************************************************************/

void CodeGen::process(const ast::ModuleNode &modNode)
{
    for (auto item : modNode.GetStatements())
        visit(item);
}

/*************************************************************************/

Value *CodeGen::process(const ast::ConstantExpressionNode &exprNode)
{
    const std::string &literal = exprNode.GetLiteral();

    switch (exprNode.GetConstType())
    {
    case Token::Type::BOOL_CONST:
        {
            APInt val = literal == "true" ? APInt::getMaxValue(1) : APInt::getZero(1);
            return ConstantInt::get(*m_context, val);
        }
        break;

    case Token::Type::INT_CONST:
        {
            // APInt(bits, value, signed);
            APInt val(32, std::stoi(literal), true);
            return ConstantInt::get(*m_context, val);
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
        throw std::runtime_error(fmt::format("Unknown constant token type {0}", exprNode.GetConstType()));
    }

    return nullptr;
}

/*************************************************************************/

Value *CodeGen::process(const ast::ReferenceExpressionNode &exprNode)
{
    (void)exprNode;
    return nullptr;    
}

/*************************************************************************/

Value *CodeGen::process(const ast::BinaryExpressionNode &exprNode)
{
    Value *l = visit(exprNode.GetLeft());
    Value *r = visit(exprNode.GetRight());

    switch (exprNode.GetOperator())
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
        fmt::print("Invalid operator {0}\r\n", exprNode.GetOperator());
        abort();
    }

    return nullptr;
}

/*************************************************************************/

Value *CodeGen::process(const ast::UnaryExpressionNode &exprNode)
{
    Value *sub = visit(exprNode.GetSub());

    switch (exprNode.GetOperator())
    {
    case (Token::Type)'-': return m_builder->CreateNeg(sub, "negtmp");

    case (Token::Type)'!':
    case (Token::Type)'~': return m_builder->CreateNot(sub, "nottmp");

    case (Token::Type)'+': // Effectively a do nothing operator
    default:
        return sub;
    }
}

/*************************************************************************/

Value *CodeGen::process(const ast::CompoundStatementNode &stmtNode, Function *fn /* = nullptr */)
{
    BasicBlock *bb = BasicBlock::Create(*m_context, "entry", fn);

    m_builder->SetInsertPoint(bb);
    
    for (auto statement : stmtNode.GetItems())
    {
        auto val = visit(statement);
        (void)val;
    }

    return nullptr;
}


/*************************************************************************/

Value *CodeGen::process(const ast::AssignmentStatementNode &stmtNode)
{
    Value *to = visit(stmtNode.GetReference());
    Value *expr = visit(stmtNode.GetExpression());
    
    return m_builder->CreateStore(expr, to);
}

/*************************************************************************/

Value *CodeGen::process(const ast::WhileStatementNode &stmtNode)
{
    (void)stmtNode;
    return nullptr;
}

/*************************************************************************/

Value *CodeGen::process(const ast::IfStatementNode &stmtNode)
{
    Value *cond = visit(stmtNode.GetCondition());

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

    Value *then = visit(stmtNode.GetTruePart());
    if (!then)
        return nullptr;

    m_builder->CreateBr(mergeBlock);
    // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
    thenBlock = m_builder->GetInsertBlock();

    // Else part
    auto elsePart = stmtNode.GetFalsePart();
    Value *else_ = nullptr;

    if (elsePart)
    {
        m_builder->
        func->insert(func->end(), elseBlock);
        m_builder->SetInsertPoint(elseBlock);
        
        if (elsePart)
            else_ = visit(elsePart);

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

    return pn;
}

/*************************************************************************/

Value *CodeGen::process(const ast::FunctionNode &stmtNode)
{
    llvm::Type *retType = llvm::Type::getVoidTy(*m_context);
    std::vector<llvm::Type *> args;

    FunctionType *ft = FunctionType::get(retType, args, false);

    PSymbol name = stmtNode.GetName();

    Function *fn = Function::Create(ft, Function::ExternalLinkage, name->name, *m_module);

    process(*stmtNode.GetBody(), fn);

    verifyFunction(*fn);

    // Run function pass optimizations.
    //m_fpm->run(*fn, *m_fam);

    return nullptr;
}

/*************************************************************************/

Value *CodeGen::visit(ast::PNode node)
{
    if (!node)
        return nullptr;

    switch (node->GetType())
    {
    case ast::Node::Type::Reference:
        return process(static_cast<ast::ReferenceNode&>(*node));

    case ast::Node::Type::Module:
        process(static_cast<ast::ModuleNode&>(*node));
        return nullptr;

    // Expressions
    case ast::Node::Type::ConstantExpression:
        return process(static_cast<ast::ConstantExpressionNode&>(*node));

    case ast::Node::Type::ReferenceExpression:
        return process(static_cast<ast::ReferenceExpressionNode&>(*node));

    case ast::Node::Type::BinaryExpression:
        return process(static_cast<ast::BinaryExpressionNode&>(*node));

    case ast::Node::Type::UnaryExpression:
        return process(static_cast<ast::UnaryExpressionNode&>(*node));

    // Statements
    case ast::Node::Type::CompoundStatement:
        return process(static_cast<ast::CompoundStatementNode&>(*node));

    case ast::Node::Type::AssignmentStatement:
        return process(static_cast<ast::AssignmentStatementNode&>(*node));

    case ast::Node::Type::WhileStatement:
        return process(static_cast<ast::WhileStatementNode&>(*node));

    case ast::Node::Type::IfStatement:
        return process(static_cast<ast::IfStatementNode&>(*node));

    // Top Level Statements
    case ast::Node::Type::FunctionStatement:
        return process(static_cast<ast::FunctionNode&>(*node));

    default:
        throw std::runtime_error("Unknown node type!");
    }
}

/*************************************************************************/

void CodeGen::generate(ast::PNode root)
{
    visit(root);
    m_module->print(llvm::errs(), nullptr);
}

/*************************************************************************/
