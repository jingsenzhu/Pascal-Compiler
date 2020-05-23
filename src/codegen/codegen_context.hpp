#ifndef CODEGEN_CONTEXT_H
#define CODEGEN_CONTEXT_H

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>

static llvm::LLVMContext llvm_context;

namespace spc
{

    class CodegenContext
    {
    private:
        /* data */
    public:
        CodegenContext(/* args */);
        ~CodegenContext();
    };
    
    CodegenContext::CodegenContext(/* args */)
    {
    }
    
    CodegenContext::~CodegenContext()
    {
    }

    class CodegenException : public std::exception {
    public:
        explicit CodegenException(const std::string &description, const unsigned &id = 0) : description(description), id(id) {};
        const char *what() const noexcept {
            return ("Error " + std::to_string(id) + ": " + description).c_str();
        }
    private:
        unsigned id;
        std::string description;
    };
    
    
} // namespace spc


#endif
