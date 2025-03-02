/*************************************************************************/
/*************************************************************************/

#include "../osbc.h"

#include "../ast.h"

/*************************************************************************/

using namespace ast;

/*************************************************************************/

CallExpressionNode::CallExpressionNode(private_tag__, const PCallStatementNode &call)
    : ExpressionNode(call->GetLineNumber())
    , m_call(call)
{
}

/*************************************************************************/