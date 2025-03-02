/*************************************************************************/
/*************************************************************************/

#ifndef OS_SCOPE_H__
#define OS_SCOPE_H__

/*************************************************************************/

#include "bootstrap.h"
#include "opcodes.h"

#include "symbol.h"
#include "token.h"

/*************************************************************************/

typedef std::shared_ptr<class CodeScope> PCodeScope;

class CodeScope
{
private:
    // Remove copy/move consturctors
    CodeScope(const CodeScope &) = delete;
    CodeScope(CodeScope &&) = delete;

    const CodeScope &operator =(const CodeScope &) = delete;
    const CodeScope &operator =(CodeScope &&) = delete;

private:
    struct CodeOp
    {
        OpCode opCode;
        PSymbol arg;

        CodeOp(OpCode o, PSymbol a)
            : opCode(o)
            , arg(a)
        {
        }

        CodeOp(const CodeOp &rhs)
            : opCode(rhs.opCode)
            , arg(rhs.arg)
        {
        }

        CodeOp(CodeOp &&rhs) noexcept
            : opCode(std::move(rhs.opCode))
            , arg(std::move(rhs.arg))
        {
        }

        const CodeOp &operator =(const CodeOp &rhs)
        {
            opCode = rhs.opCode;
            arg = rhs.arg;
            return *this;
        }

        const CodeOp &operator =(CodeOp &&rhs) noexcept
        {
            opCode = std::move(rhs.opCode);
            arg = std::move(rhs.arg);
            return *this;
        }
    };

    static uint s_uniqueLabelId;

private:
    PCodeScope m_parent;
    std::string m_labelIdent;
    PSymbol m_label;

    std::vector<CodeOp> m_ops;

public:
    // Inheritance constructor
    CodeScope(PCodeScope parent, std::string_view labelIdent)
        : m_parent(parent)
        , m_labelIdent(labelIdent)
        , m_label()
    {
    }

    CodeScope(PCodeScope parent = nullptr)
        : m_parent(parent)
        , m_label()
    {
    }

    PCodeScope parent() const { return m_parent; }

    // Create a label that will be filled in with an address later.
    PSymbol createLabel();

    // Define the pointer of a label/function.
    void insertLabel(PSymbol symbol);

    // Emit an op code to this code scope.
    void emit(OpCode opCode, PSymbol arg = nullptr);

    // Output all generated code
    void write();
};

/*************************************************************************/

#endif /* OS_SCOPE__ */

/*************************************************************************/
