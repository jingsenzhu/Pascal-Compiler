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

#include <string>
#include <list>
#include <map>

static llvm::LLVMContext llvm_context;

namespace spc
{
    class ArrayTypeNode;
    class RecordTypeNode;

    class CodegenContext
    {
    private:
        std::unique_ptr<llvm::Module> _module;
        llvm::IRBuilder<> builder;
        std::map<std::string, llvm::Type *> aliases;
        std::map<std::string, std::shared_ptr<ArrayTypeNode>> arrAliases;
        std::map<std::string, std::shared_ptr<RecordTypeNode>> recAliases;
        std::map<std::string, llvm::Value*> locals;
        std::map<std::string, llvm::Value*> consts;
    public:
        bool is_subroutine;
        std::list<std::string> traces;
        llvm::Function *printfFunc, *scanfFunc, *absFunc, *fabsFunc, *sqrtFunc, *strcpyFunc, *getsFunc;

        CodegenContext(const std::string &module_id)
            : builder(llvm::IRBuilder<>(llvm_context)), _module(std::make_unique<llvm::Module>(module_id, llvm_context)), is_subroutine(false)
        {
            auto printfTy = llvm::FunctionType::get(llvm::Type::getInt32Ty(llvm_context), {llvm::Type::getInt8PtrTy(llvm_context)}, true);
            printfFunc = llvm::Function::Create(printfTy, llvm::Function::ExternalLinkage, "printf", *_module);

            auto scanfTy = llvm::FunctionType::get(llvm::Type::getInt32Ty(llvm_context), {llvm::Type::getInt8PtrTy(llvm_context)}, true);
            scanfFunc = llvm::Function::Create(scanfTy, llvm::Function::ExternalLinkage, "scanf", *_module);

            auto absTy = llvm::FunctionType::get(llvm::Type::getInt32Ty(llvm_context), {llvm::Type::getInt32Ty(llvm_context)}, false);
            absFunc = llvm::Function::Create(absTy, llvm::Function::ExternalLinkage, "abs", *_module);

            auto fabsTy = llvm::FunctionType::get(llvm::Type::getDoubleTy(llvm_context), {llvm::Type::getDoubleTy(llvm_context)}, false);
            fabsFunc = llvm::Function::Create(fabsTy, llvm::Function::ExternalLinkage, "fabs", *_module);

            auto sqrtTy = llvm::FunctionType::get(llvm::Type::getDoubleTy(llvm_context), {llvm::Type::getDoubleTy(llvm_context)}, false);
            sqrtFunc = llvm::Function::Create(sqrtTy, llvm::Function::ExternalLinkage, "sqrt", *_module);

            auto strcpyTy = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(llvm_context), {llvm::Type::getInt8PtrTy(llvm_context), llvm::Type::getInt8PtrTy(llvm_context)}, false);
            strcpyFunc = llvm::Function::Create(strcpyTy, llvm::Function::ExternalLinkage, "strcpy", *_module);

            auto getsTy = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(llvm_context), {llvm::Type::getInt8PtrTy(llvm_context)}, false);
            getsFunc = llvm::Function::Create(getsTy, llvm::Function::ExternalLinkage, "strcpy", *_module);

            printfFunc->setCallingConv(llvm::CallingConv::C);
            scanfFunc->setCallingConv(llvm::CallingConv::C);
            absFunc->setCallingConv(llvm::CallingConv::C);
            fabsFunc->setCallingConv(llvm::CallingConv::C);
            sqrtFunc->setCallingConv(llvm::CallingConv::C);
            strcpyTy->setCallingConv(llvm::CallingConv::C);
            getsTy->setCallingConv(llvm::CallingConv::C);
        }
        ~CodegenContext() = default;

        std::string getTrace() 
        {
            if (traces.empty()) return "main";
            return traces.back(); 
        }

        llvm::Value *getLocal(const std::string &key) 
        {
            auto V = locals.find(key);
            if (V == locals.end())
                return nullptr;
            return V->second;
        };
        bool setLocal(const std::string &key, llvm::Value *value) 
        {
            if (getLocal(key))
                return false;
            locals[key] = value;
            return true;
        }
        llvm::Value *getConst(const std::string &key) 
        {
            auto V = consts.find(key);
            if (V == consts.end())
                return nullptr;
            return V->second;
        };
        bool setConst(const std::string &key, llvm::Value *value) 
        {
            if (getConst(key))
                return false;
            consts[key] = value;
            return true;
        }
        std::shared_ptr<ArrayTypeNode> getArrayAlias(const std::string &key) 
        {
            auto V = arrAliases.find(key);
            if (V == arrAliases.end())
                return nullptr;
            return V->second;
        }
        bool setArrayAlias(const std::string &key, const std::shared_ptr<ArrayTypeNode> &value) 
        {
            if (getArrayAlias(key))
                return false;
            assert(value != nullptr);
            arrAliases[key] = value;
            return true;
        }
        std::shared_ptr<RecordTypeNode> getRecordAlias(const std::string &key) 
        {
            auto V = recAliases.find(key);
            if (V == recAliases.end())
                return nullptr;
            return V->second;
        }
        bool setRecordAlias(const std::string &key, const std::shared_ptr<RecordTypeNode> &value) 
        {
            if (getRecordAlias(key))
                return false;
            assert(value != nullptr);
            recAliases[key] = value;
            return true;
        }
        llvm::Type *getAlias(const std::string &key) 
        {
            auto V = aliases.find(key);
            if (V == aliases.end())
                return nullptr;
            return V->second;
        }
        bool setAlias(const std::string &key, llvm::Type *value) 
        {
            if (getAlias(key))
                return false;
            aliases[key] = value;
            return true;
        }
        llvm::IRBuilder<> &getBuilder()
        {
            return builder;
        }
        std::unique_ptr<llvm::Module> &getModule() {
            return _module;
        }
        void dump() 
        {
            _module->print(llvm::errs(), nullptr);
        }

    };

    class CodegenException : public std::exception {
    public:
        explicit CodegenException(const std::string &description) : description(description) {};
        const char *what() const noexcept {
            return description.c_str();
        }
    private:
        // unsigned id;
        std::string description;
    };
    
    
} // namespace spc


#endif
