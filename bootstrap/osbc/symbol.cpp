/*************************************************************************/
/*************************************************************************/

#include "bootstrap.h"
#include "symbol.h"
#include "symtable.h"

#include <fmt/format.h>

/*************************************************************************/

bool Symbol::isGlobal() const
{
    return m_parent->Parent() == nullptr;
}

/*************************************************************************/
/*************************************************************************/

auto fmt::formatter<Symbol::UseType>::format(Symbol::UseType useType, format_context &ctx) const
    -> format_context::iterator
{
    string_view name;

    switch (useType)
    {
    case Symbol::UseType::Function: name = "Function"; break;
    case Symbol::UseType::Variable: name = "Variable"; break;
    case Symbol::UseType::Primitive: name = "Primitive"; break;
    case Symbol::UseType::Label: name = "Label"; break;
    case Symbol::UseType::Struct: name = "Struct"; break;
    case Symbol::UseType::Enum: name = "Enum"; break;
    case Symbol::UseType::Set: name = "Set"; break;
    default: name = "]]] BUG: UNKNOWN USE TYPE! [[["; break;
    }

    return formatter<string_view>::format(name, ctx);
}

/*************************************************************************/
