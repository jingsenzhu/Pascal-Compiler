#include "utils/ast.hpp"
#include "codegen_context.hpp"

namespace spc
{
    
    llvm::Value *ProgramNode::codegen(CodegenContext &context)
    {
        context.is_subroutine = false;
        context.log() << "Entering main program" << std::endl;
        auto *funcT = llvm::FunctionType::get(context.getBuilder().getInt32Ty(), false);
        auto *mainFunc = llvm::Function::Create(funcT, llvm::Function::ExternalLinkage, "main", *context.getModule());
        auto *block = llvm::BasicBlock::Create(context.getModule()->getContext(), "entry", mainFunc);
        context.getBuilder().SetInsertPoint(block);

        context.log() << "Entering global const part" << std::endl;
        header->constList->codegen(context);
        context.log() << "Entering global type part" << std::endl;
        header->typeList->codegen(context);
        context.log() << "Entering global var part" << std::endl;
        header->varList->codegen(context);
        context.is_subroutine = true;
        context.log() << "Entering global routine part" << std::endl;
        header->subroutineList->codegen(context);
        context.is_subroutine = false;

        context.getBuilder().SetInsertPoint(block);
        context.log() << "Entering global body part" << std::endl;
        body->codegen(context);
        context.getBuilder().CreateRet(context.getBuilder().getInt32(0));

        llvm::verifyFunction(*mainFunc, &llvm::errs());

        // Optimizations
        if (context.fpm)
            context.fpm->run(*mainFunc);
        if (context.mpm)
            context.mpm->run(*context.getModule());
        return nullptr;
    }

    llvm::Value *RoutineNode::codegen(CodegenContext &context)
    {
        context.log() << "Entering function " + name->name << std::endl;

        if (context.getModule()->getFunction(name->name) != nullptr)
            throw CodegenException("Duplicate function definition: " + name->name);

        context.traces.push_back(name->name);

        std::vector<llvm::Type *> types;
        std::vector<std::string> names;
        for (auto &p : params->getChildren()) 
        {
            auto *ty = p->type->getLLVMType(context);
            if (ty == nullptr)
                throw CodegenException("Unsupported function param type");
            types.push_back(ty);
            names.push_back(p->name->name);
            if (ty->isArrayTy())
            {
                if (p->type->type == Type::String)
                    context.setArrayEntry(name->name + "." + p->name->name, 0, 255);
                else if (p->type->type == Type::Array)
                {
                    auto arrTy = cast_node<ArrayTypeNode>(p->type);
                    assert(arrTy != nullptr);
                    context.setArrayEntry(name->name + "." + p->name->name, arrTy);
                    arrTy->insertNestedArray(name->name + "." + p->name->name, context);
                }
                else if (p->type->type == Type::Alias)
                {
                    std::string aliasName = cast_node<AliasTypeNode>(p->type)->name->name;
                    std::shared_ptr<ArrayTypeNode> a;
                    for (auto rit = context.traces.rbegin(); rit != context.traces.rend(); rit++)
                        if ((a = context.getArrayAlias(*rit + "." + aliasName)) != nullptr)
                            break;
                    if (a == nullptr) a = context.getArrayAlias(aliasName);
                    assert(a != nullptr && "Fatal error: array type not found!");
                    context.setArrayEntry(name->name + "." + p->name->name, a);
                    a->insertNestedArray(name->name + "." + p->name->name, context);
                }
            }
            else if (ty->isStructTy())
            {
                assert(is_ptr_of<RecordTypeNode>(p->type) || is_ptr_of<AliasTypeNode>(p->type));
                if (is_ptr_of<RecordTypeNode>(p->type))
                {
                    auto recTy = cast_node<RecordTypeNode>(p->type);
                    context.setRecordAlias(name->name + "." + p->name->name, recTy);
                    recTy->insertNestedRecord(name->name + "." + p->name->name, context);
                }
                else
                {
                    std::string aliasName = cast_node<AliasTypeNode>(p->type)->name->name;
                    std::shared_ptr<RecordTypeNode> recTy = nullptr;
                    for (auto rit = context.traces.rbegin(); rit != context.traces.rend(); rit++)
                    {
                        if ((recTy = context.getRecordAlias(*rit + "." + aliasName)) != nullptr)
                            break;
                    }
                    if (recTy == nullptr) recTy = context.getRecordAlias(aliasName);
                    if (recTy == nullptr) assert(0);
                    context.setRecordAlias(name->name + "." + p->name->name, recTy);
                    recTy->insertNestedRecord(name->name + "." + p->name->name, context);
                }
            }
        }
        llvm::Type *retTy = this->retType->getLLVMType(context);
        if (retTy == nullptr) throw CodegenException("Unsupported function return type");
        if (retTy->isArrayTy())
        {
            if (!retTy->getArrayElementType()->isIntegerTy(8) || retTy->getArrayNumElements() != 256)
                throw CodegenException("Not support array as function return type");
            retTy = context.getBuilder().getInt8PtrTy();
            context.setArrayEntry(name->name + "." + name->name, 0, 255);
        }
        else if (retTy->isStructTy())
        {
            assert(is_ptr_of<RecordTypeNode>(this->retType) || is_ptr_of<AliasTypeNode>(this->retType));
            if (is_ptr_of<RecordTypeNode>(this->retType))
            {
                auto recTy = cast_node<RecordTypeNode>(this->retType);
                context.setRecordAlias(name->name + "." + name->name, recTy);
                recTy->insertNestedRecord(name->name + "." + name->name, context);
            }
            else
            {
                std::string aliasName = cast_node<AliasTypeNode>(this->retType)->name->name;
                std::shared_ptr<RecordTypeNode> recTy = nullptr;
                for (auto rit = context.traces.rbegin(); rit != context.traces.rend(); rit++)
                {
                    if ((recTy = context.getRecordAlias(*rit + "." + aliasName)) != nullptr)
                        break;
                }
                if (recTy == nullptr) recTy = context.getRecordAlias(aliasName);
                if (recTy == nullptr) assert(0);
                context.setRecordAlias(name->name + "." + name->name, recTy);
                recTy->insertNestedRecord(name->name + "." + name->name, context);
            }
        }
        auto *funcTy = llvm::FunctionType::get(retTy, types, false);
        auto *func = llvm::Function::Create(funcTy, llvm::Function::ExternalLinkage, name->name, *context.getModule());
        auto *block = llvm::BasicBlock::Create(context.getModule()->getContext(), "entry", func);
        context.getBuilder().SetInsertPoint(block);

        auto index = 0;
        for (auto &arg : func->args())
        {
            auto *type = arg.getType();
            // if (!type->isIntegerTy(32) && !type->isIntegerTy(8) && !type->isDoubleTy())
            //     throw CodegenException("Unknown function param type");
            auto *local = context.getBuilder().CreateAlloca(type);
            context.setLocal(name->name + "." + names[index++], local);
            context.getBuilder().CreateStore(&arg, local);
        }

        context.log() << "Entering const part of function " << name->name << std::endl;
        header->constList->codegen(context);
        context.log() << "Entering type part of function " << name->name << std::endl;
        header->typeList->codegen(context);
        context.log() << "Entering var part of function " << name->name << std::endl;
        header->varList->codegen(context);

        context.log() << "Entering routine part of function " << name->name << std::endl;
        header->subroutineList->codegen(context);

        context.getBuilder().SetInsertPoint(block);
        if (retType->type != Type::Void)  // set the return variable
        {  
            auto *type = retType->getLLVMType(context);
            // if (!type->isIntegerTy(32) && !type->isIntegerTy(8) && !type->isDoubleTy())
            //     throw CodegenException("Unknown function return type");

            llvm::Value *local;
            if (type == nullptr) throw CodegenException("Unknown function return type");
            else if (type->isArrayTy())
            {
                if (type->getArrayElementType()->isIntegerTy(8) && type->getArrayNumElements() == 256) // String
                {
                    // llvm::ConstantInt *space = llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), 256);
                    // local = context.getBuilder().CreateAlloca(context.getBuilder().getInt8Ty(), space);
                    local = context.getBuilder().CreateAlloca(type);
                }
                else
                    throw CodegenException("Unknown function return type");
            }
            else
                local = context.getBuilder().CreateAlloca(type);
            assert(local != nullptr && "Fatal error: Local variable alloc failed!");
            context.setLocal(name->name + "." + name->name, local);
        }


        // block = llvm::BasicBlock::Create(context.getModule()->getContext(), "back", func);
        // context.getBuilder().SetInsertPoint(block);
        context.log() << "Entering body part of function " << name->name << std::endl;
        body->codegen(context);

        if (retType->type != Type::Void) 
        {
            auto *local = context.getLocal(name->name + "." + name->name);
            llvm::Value *ret = context.getBuilder().CreateLoad(local);
            if (ret->getType()->isArrayTy())
            {
                // assert(ret->getType()->isArrayTy());
                llvm::Value *tmpStr = context.getTempStrPtr();
                llvm::Value *zero = llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), 0, false);
                llvm::Value *retPtr = context.getBuilder().CreateInBoundsGEP(local, {zero, zero});
                context.log() << "\tSysfunc STRCPY" << std::endl;
                context.getBuilder().CreateCall(context.strcpyFunc, {tmpStr, retPtr});
                context.log() << "\tSTRING return" << std::endl;
                context.getBuilder().CreateRet(tmpStr);
            }
            else
                context.getBuilder().CreateRet(ret);
        } 
        else 
        {
            context.getBuilder().CreateRetVoid();
        }

        llvm::verifyFunction(*func, &llvm::errs());

        if (context.fpm)
            context.fpm->run(*func);

        // context.resetLocal();
        context.traces.pop_back();  

        context.log() << "Leaving function " << name->name << std::endl;

        return nullptr;
    }

} // namespace spc
