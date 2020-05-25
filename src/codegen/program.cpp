#include "utils/ast.hpp"
#include "codegen_context.hpp"

namespace spc
{
    
    llvm::Value *ProgramNode::codegen(CodegenContext &context)
    {
        context.is_subroutine = false;
        auto *funcT = llvm::FunctionType::get(context.getBuilder().getInt32Ty(), false);
        auto *mainFunc = llvm::Function::Create(funcT, llvm::Function::ExternalLinkage, "main", *context.getModule());
        auto *block = llvm::BasicBlock::Create(context.getModule()->getContext(), "entry", mainFunc);
        context.getBuilder().SetInsertPoint(block);

        header->constList->codegen(context);
        header->typeList->codegen(context);
        context.is_subroutine = true;
        header->subroutineList->codegen(context);
        context.is_subroutine = false;

        context.getBuilder().SetInsertPoint(block);
        body->codegen(context);
        context.getBuilder().CreateRet(context.getBuilder().getInt32(0));

        llvm::verifyFunction(*mainFunc, &llvm::errs());

        return nullptr;
    }

    llvm::Value *RoutineNode::codegen(CodegenContext &context)
    {
        std::string oldTrace = context.trace;
        context.trace = name->name;
        std::vector<llvm::Type *> types;
        std::vector<std::string> names;
        for (auto &p : params->getChildren()) 
        {
            types.push_back(p->type->getLLVMType(context));
            names.push_back(p->name->name);
        }
        auto *funcTy = llvm::FunctionType::get(this->retType->getLLVMType(context), types, false);
        auto *func = llvm::Function::Create(funcTy, llvm::Function::ExternalLinkage, name->name, *context.getModule());
        auto *block = llvm::BasicBlock::Create(context.getModule()->getContext(), "entry", func);
        context.getBuilder().SetInsertPoint(block);

        auto index = 0;
        for (auto &arg : func->args()) 
        {
            auto *type = arg.getType();
            if (!type->isIntegerTy(32) && !type->isIntegerTy(8) && !type->isDoubleTy())
                throw CodegenException("Unknown function param type");
            auto *local = context.builder.CreateAlloca(type);
            context.setLocal(name->name + "_" + names[index++], local);
            context.getBuilder().CreateStore(&arg, local);
        }

        header->constList->codegen(context);
        header->typeList->codegen(context);
        header->varList->codegen(context);

        if (retType->type != Type::Void)  // set the return variable
        {  
            auto *type = retType->getLLVMType(context);
            if (!type->isIntegerTy(32) && !type->isIntegerTy(8) && !type->isDoubleTy())
                throw CodegenException("Unknown function param type");
            auto *local = context.builder.CreateAlloca(type);
            context.setLocal(name->name, local);
        }

        header->subroutineList->codegen(context);

        block = llvm::BasicBlock::Create(context.getModule()->getContext(), "back", func);
        context.getBuilder().SetInsertPoint(block);
        body->codegen(context);

        if (retType->type != Type::Void) 
        {
            auto *local = context.getLocal(name->name);
            auto *ret = context.getBuilder().CreateLoad(local);
            context.getBuilder().CreateRet(ret);
        } 
        else 
        {
            context.getBuilder().CreateRetVoid();
        }

        // ? terminator needed
        llvm::verifyFunction(*func, &llvm::errs());

        // context.resetLocal();
        context.trace = oldTrace;

        return nullptr;
    }

} // namespace spc
