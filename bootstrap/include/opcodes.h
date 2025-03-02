/*************************************************************************/
/*************************************************************************/

#ifndef OS_BOOTSTRAP_OPCODES_H__
#define OS_BOOTSTRAP_OPCODES_H__

/*************************************************************************/

#include <cstdint>

enum class OpCode : uint8_t
{
    // Control operations
    NOP = 0x00, // No operation
    BRK = 0x01, // Break point
    STP = 0x02, // Stop (exit)

    // Load operations
    LDC = 0x10, // Load constant
    LDV = 0x11, // Load variable

    // Store operations
    STV = 0x20, // Store variable
    POP = 0x21, // Pop value off stack and discard

    // ALU operations
    AND = 0x40, // Bitwise AND
    OR  = 0x41, // Bitwise OR
    XOR = 0x42, // Bitwise XOR
    NOT = 0x43, // Bitwise invert
    ADD = 0x44, // Add
    SUB = 0x45, // Subtract
    MUL = 0x46, // Multiply
    DIV = 0x47, // Divide
    MOD = 0x48, // Modulo
    SHL = 0x49, // Shift left
    SHR = 0x4A, // Shift right
    NEG = 0x4B, // Invert sign bit
    INC = 0x4C, // Increment by one
    DEC = 0x4D, // Decrement by one

    // ALU Compare ops
    EQU = 0x50, // Equals
    NEQ = 0x51, // Not equals
    GT  = 0x52, // Greater Than
    LT  = 0x53, // Less Than
    GTE = 0x54, // Gearter Than or Equals
    LTE = 0x55, // Less Than or Equals

    // Flow operations
    // Branch operations are relative
    BRA = 0xB0, // Unconditional branch
    CBR = 0xB1, // Conditional branch
    
    // Jump operations are absolute
    JMP = 0xC0, // Jump absolute
    JSR = 0xC1, // Jump subroutine
    RTS = 0xC2, // Return from subroutine
    SYS = 0xC3, // System (native) call
};

/*************************************************************************/

template <>
struct fmt::formatter<OpCode> : formatter<string_view>
{
    // Defined in scope.cpp
    auto format(OpCode opCode, format_context &ctx) const
        -> format_context::iterator
    {
        string_view name;

        switch (opCode)
        {
        case OpCode::NOP: name = "NOP"; break;
        case OpCode::BRK: name = "BRK"; break;
        case OpCode::STP: name = "STP"; break;
        case OpCode::LDC: name = "LDC"; break;
        case OpCode::LDV: name = "LDV"; break;
        case OpCode::STV: name = "STV"; break;
        case OpCode::POP: name = "POP"; break;
        case OpCode::AND: name = "AND"; break;
        case OpCode::OR : name = "OR" ; break;
        case OpCode::XOR: name = "XOR"; break;
        case OpCode::NOT: name = "NOT"; break;
        case OpCode::ADD: name = "ADD"; break;
        case OpCode::SUB: name = "SUB"; break;
        case OpCode::MUL: name = "MUL"; break;
        case OpCode::DIV: name = "DIV"; break;
        case OpCode::MOD: name = "MOD"; break;
        case OpCode::SHL: name = "SHL"; break;
        case OpCode::SHR: name = "SHR"; break;
        case OpCode::NEG: name = "NEG"; break;
        case OpCode::INC: name = "INC"; break;
        case OpCode::DEC: name = "DEC"; break;
        case OpCode::EQU: name = "EQU"; break;
        case OpCode::NEQ: name = "NEQ"; break;
        case OpCode::GT : name = "GT "; break;
        case OpCode::LT : name = "LT "; break;
        case OpCode::GTE: name = "GTE"; break;
        case OpCode::LTE: name = "LTE"; break;
        case OpCode::BRA: name = "BRA"; break;
        case OpCode::CBR: name = "CBR"; break;
        case OpCode::JMP: name = "JMP"; break;
        case OpCode::JSR: name = "JSR"; break;
        case OpCode::RTS: name = "RTS"; break;
        case OpCode::SYS: name = "SYS"; break;

        default:
            name = "]] !!!BUG!!! UNKNOWN OP CODE [[";
            break;
        }

        return formatter<string_view>::format(name, ctx);
    }
};

/*************************************************************************/

#endif /* OS_BOOTSTRAP_OPCODES_H__ */

/*************************************************************************/
