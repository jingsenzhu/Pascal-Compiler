#include "utils/ast.hpp"
#include "codegen_context.hpp"

namespace spc
{

    llvm::Value *VarDeclNode::createGlobalArray(CodegenContext &context)
    {
        std::shared_ptr<ArrayTypeNode> arrTy = cast_node<ArrayTypeNode>(this->type);
        auto *ty = arrTy->itemType->getLLVMType();
        llvm::Constant *z; // zero
        if (ty->isIntegerTy()) 
            z = llvm::ConstantInt::get(ty, 0);
        else if (ty->isDoubleTy())
            z = llvm::ConstantFP::get(ty, 0.0);
        else 
            throw CodegenException("Unknown type");

        llvm::ConstantInt *startIdx = llvm::dyn_cast<llvm::ConstantInt>(arrTy->range_start->codegen(context));
        if (!startIdx)
            throw CodegenException("Start index invalid");
        else if (startIdx->getSExtValue() != 0)
            throw CodegenException("Start index must be zero!");
        
        int len = 0;
        llvm::ConstantInt *endIdx = llvm::dyn_cast<llvm::ConstantInt>(arrTy->range_end->codegen(context));
        if (!endIdx || endIdx->getSExtValue() < 0)
            throw CodegenException("End index invalid");
        else if (endIdx->getBitWidth() <= 32)
            len = endIdx->getSExtValue() + 1;
        else
            throw CodegenException("End index overflow");

        llvm::ArrayType* arr = llvm::ArrayType::get(ty, len);
        std::vector<llvm::Constant *> initVector;
        for (int i = 0; i < len; i++)
            initVector.push_back(z);
        auto *variable = llvm::ConstantArray::get(arr, initVector);

        return new llvm::GlobalVariable(*context.getModule(), variable->getType(), false, llvm::GlobalVariable::ExternalLinkage, variable, this->name->name);
    }

    llvm::Value *VarDeclNode::createArray(CodegenContext &context)
    {
        std::shared_ptr<ArrayTypeNode> arrTy = cast_node<ArrayTypeNode>(this->type);
        auto *ty = arrTy->itemType->getLLVMType(context);
        llvm::Constant *constant;
        if (ty->isIntegerTy()) 
            constant = llvm::ConstantInt::get(ty, 0);
        else if (ty->isDoubleTy())
            constant = llvm::ConstantFP::get(ty, 0.0);
        else 
            throw CodegenException("Unknown type");

        llvm::ConstantInt *startIdx = llvm::dyn_cast<llvm::ConstantInt>(arrTy->range_start->codegen(context));
        if (!startIdx)
            throw CodegenException("Start index invalid");
        else if (startIdx->getSExtValue() != 0)
            throw CodegenException("Start index must be zero!");
        
        int len = 0;
        llvm::ConstantInt *endIdx = llvm::dyn_cast<llvm::ConstantInt>(arrTy->range_end->codegen(context));
        if (!endIdx || endIdx->getSExtValue() < 0)
            throw CodegenException("End index invalid");
        else if (endIdx->getBitWidth() <= 32)
            len = endIdx->getSExtValue() + 1;
        else
            throw CodegenException("End index overflow");

        llvm::ConstantInt *space = llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), len);
        auto *local = context.builder.CreateAlloca(ty, space);
        auto success = context.setLocal(context.trace + "_" + this->name->name, local);
        if (!success) throw CodegenException("Duplicate identifier in var section of function " + context.trace + ": " + this->name->name);
        return local;
    }
    
    llvm::Value *VarDeclNode::codegen(CodegenContext &context)
    {
        if (context.is_subroutine)
        {
            if (type->type != Type::Array)
            {
                auto *local = context.builder.CreateAlloca(type->getLLVMType(context));
                auto success = context.setLocal(context.trace + "_" + name->name, local);
                if (!success) throw CodegenException("Duplicate identifier in function " + context.trace + ": " + name->name);
                return local;
            }
            else
                return createArray(context);
        }
        else
        {
            if (type->type != Type::Array)
            {
                auto *ty = type->getLLVMType(context);
                llvm::Constant *constant;
                if (ty->isIntegerTy()) 
                    constant = llvm::ConstantInt::get(ty, 0);
                else if (ty->isDoubleTy())
                    constant = llvm::ConstantFP::get(ty, 0.0);
                else 
                    throw CodegenException("Unknown type");
                return new llvm::GlobalVariable(*context.getModule(), type, false, llvm::GlobalVariable::ExternalLinkage, constant, name->name);
            }
            else
                return createGlobalArray(context);
        }
    }

    llvm::Value *ConstDeclNode::codegen(CodegenContext &context)
    {
        if (context.is_subroutine)
        {
            if (val->type == Type::String)
            {
                auto strVal = cast_node<StringNode>(val);
                auto *constant = llvm::ConstantDataArray::getString(llvm_context, strVal->val, true);
                bool success = context.setLocal(context.trace + "_" + name->name, constant);
                success &= context.setLocalConst(context.trace + "_" + name->name, constant);
                if (!success) throw CodegenException("Duplicate identifier in const section of function " + context.trace + ": " + name->name);
                return new llvm::GlobalVariable(*context.getModule(), constant->getType(), true, llvm::GlobalVariable::ExternalLinkage, constant, context.trace + "_" + name->name);
            }
            else
            {
                auto *constant = llvm::cast<llvm::Constant>(val->codegen(context));
                bool success = context.setLocal(context.trace + "_" + name->name, constant);
                success &= context.setLocalConst(context.trace + "_" + name->name, constant);
                if (!success) throw CodegenException("Duplicate identifier in const section of function " + context.trace + ": " + name->name);
                return new llvm::GlobalVariable(*context.getModule(), val->getLLVMType(context), true, llvm::GlobalVariable::ExternalLinkage, constant, context.trace + "_" + name->name);
            } 
        }
        else
        {
            if (val->type == Type::String)
            {
                auto strVal = cast_node<StringNode>(val);
                auto *constant = llvm::ConstantDataArray::getString(llvm_context, strVal->val, true);
                return new llvm::GlobalVariable(*context.getModule(), constant->getType(), true, llvm::GlobalVariable::ExternalLinkage, constant, name->name);
            }
            else
            {
                auto *constant = llvm::cast<llvm::Constant>(val->codegen(context));
                return new llvm::GlobalVariable(*context.getModule(), val->getLLVMType(context), true, llvm::GlobalVariable::ExternalLinkage, constant, name->name);
            } 
        }  
    }

    llvm::Value *TypeDeclNode::codegen(CodegenContext &context)
    {
        bool success = context.setAlias(context.trace + "_" + name->name, type->getLLVMType(context));
        if (!success) throw CodegenException("Duplicate type alias in function " + context.trace + ": " + name->name);
        return nullptr;
    }


} // namespace spc
