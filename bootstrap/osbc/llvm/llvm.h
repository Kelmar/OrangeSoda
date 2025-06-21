/*************************************************************************/
/*************************************************************************/

#ifndef OSBC_LLVM_H__
#define OSBC_LLVM_H__

/*************************************************************************/

#include "llvm-19/llvm/ADT/APInt.h"
#include "llvm-19/llvm/ADT/APFloat.h"
#include "llvm-19/llvm/ADT/STLExtras.h"

#include "llvm-19/llvm/IR/BasicBlock.h"
#include "llvm-19/llvm/IR/Constants.h"
#include "llvm-19/llvm/IR/DerivedTypes.h"
#include "llvm-19/llvm/IR/Function.h"
#include "llvm-19/llvm/IR/IRBuilder.h"
#include "llvm-19/llvm/IR/Instructions.h"
#include "llvm-19/llvm/IR/LLVMContext.h"
#include "llvm-19/llvm/IR/Module.h"
#include "llvm-19/llvm/IR/PassManager.h"
#include "llvm-19/llvm/IR/Type.h"
#include "llvm-19/llvm/IR/Verifier.h"

#include "llvm-19/llvm/Passes/PassBuilder.h"
#include "llvm-19/llvm/Passes/StandardInstrumentations.h"

#include "llvm-19/llvm/Support/TargetSelect.h"

#include "llvm-19/llvm/Target/TargetMachine.h"

#include "llvm-19/llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm-19/llvm/Transforms/Scalar.h"
#include "llvm-19/llvm/Transforms/Scalar/GVN.h"
#include "llvm-19/llvm/Transforms/Scalar/Reassociate.h"
#include "llvm-19/llvm/Transforms/Scalar/SimplifyCFG.h"

/*************************************************************************/

#endif /* OSBC_LLVM_H__ */

/*************************************************************************/
