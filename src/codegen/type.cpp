#include "utils/ast.hpp"
#include "codegen_context.hpp"

namespace spc
{
    
    llvm::Type *SimpleTypeNode::getLLVMType(CodegenContext &context)
    {
        switch (type)
        {
            case Type::Bool: return context.getBuilder().getInt1Ty();
            case Type::Char: return context.getBuilder().getInt8Ty();
            case Type::Int: return context.getBuilder().getInt32Ty();
            case Type::Long: return context.getBuilder().getInt32Ty();
            case Type::Real: return context.getBuilder().getDoubleTy();
            default: throw CodegenException("Unknown type");
        }
    }

    llvm::Type *ConstValueNode::getLLVMType(CodegenContext &context)
    {
        switch (type) 
        {
            case Type::Bool: return context.getBuilder().getInt1Ty();
            case Type::Int: return context.getBuilder().getInt32Ty();
            case Type::Long: return context.getBuilder().getInt32Ty();
            case Type::Char: return context.getBuilder().getInt8Ty();
            case Type::Real: return context.getBuilder().getDoubleTy();
            case Type::String: throw CodegenException("String currently not supported.");
            default: throw CodegenException("Unknown type!"); return nullptr;
        }
        return nullptr;
    }

    llvm::Value *BooleanNode::codegen(CodegenContext &context)
    {
        return val ? context.getBuilder().getTrue() : context.getBuilder().getFalse();
    }

    llvm::Value *IntegerNode::codegen(CodegenContext &context)
    {
        auto *ty = context.getBuilder().getInt32Ty();
        return llvm::ConstantInt::getSigned(ty, val);
    }

    llvm::Value *CharNode::codegen(CodegenContext &context)
    {
        auto *ty = context.getBuilder().getInt8Ty();
        return llvm::ConstantInt::getSigned(ty, val);
    }

    llvm::Value *RealNode::codegen(CodegenContext &context)
    {
        auto *ty = context.getBuilder().getDoubleTy();
        return llvm::ConstantFP::get(ty, val);
    }

    llvm::Value *StringNode::codegen(CodegenContext &context) 
    {
        llvm::Module *M = context.getModule().get();
        llvm::LLVMContext& ctx = M->getContext();
        llvm::Constant *strConstant = llvm::ConstantDataArray::getString(ctx, val);
        llvm::Type *t = strConstant->getType();
        llvm::GlobalVariable *GVStr = new llvm::GlobalVariable(*M, t, true, llvm::GlobalValue::ExternalLinkage, strConstant, "");
        llvm::Constant* zero = llvm::Constant::getNullValue(llvm::IntegerType::getInt32Ty(ctx));

        llvm::Constant *strVal = llvm::ConstantExpr::getGetElementPtr(t, GVStr, zero, true);

        return strVal;
    }

} // namespace spc
