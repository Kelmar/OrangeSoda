/*************************************************************************/
/*************************************************************************/

#include "bootstrap.h"
#include "scope.h"

#include <fmt/format.h>

/*************************************************************************/

uint CodeScope::s_uniqueLabelId = 0;

/*************************************************************************/

PSymbol CodeScope::createLabel()
{
    int id = s_uniqueLabelId++;
    (void)id;

#if 0
    PSymbol symbol(new Symbol());

    symbol->name = fmt::format("__gen_label_{0:8X}", id);
    symbol->lineNumber = -1;
    symbol->useType = Symbol::UseType::Label;
    symbol->exporting = false;
    symbol->isConst = true;
    symbol->isSpecial = true;
    symbol->baseType = nullptr;

    //m_symbols[symbol->name] = symbol;

    return symbol;
#endif

    return nullptr;
}

/*************************************************************************/

void CodeScope::insertLabel(PSymbol symbol)
{
    (void)symbol;
    //symbol->pointer = (int)m_ops.size();
}

/*************************************************************************/

void CodeScope::emit(OpCode opCode, PSymbol arg /* = nullptr */)
{
    m_ops.push_back(CodeOp(opCode, arg));
}

/*************************************************************************/

void CodeScope::write()
{
    if (m_parent)
    {
        for (auto op : m_ops)
            m_parent->emit(op.opCode, op.arg);
    }
    else
    {
#if 0
        for (auto op : m_ops)
        {
            if (op.arg)
                fmt::print("{0} {1}\r\n", op.opCode, op.arg->pointer);
            else
                fmt::print("{0}\r\n", op.opCode);
        }
#endif
    }
}

/*************************************************************************/
