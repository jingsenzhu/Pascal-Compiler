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

    llvm::Type *AliasTypeNode::getLLVMType(CodegenContext &context) 
    {
        if (context.is_subroutine)
        {
            for (auto rit = context.traces.rbegin(); rit != context.traces.rend(); rit++)
            {
                llvm::Type *ret = nullptr;
                if ((ret = context.getAlias(*rit + "_" + name->name)) != nullptr)
                    return ret;
            }
        }
        llvm::Type *ret = context.getAlias(name->name);
        if (ret == nullptr)
            throw CodegenException("Undefined alias in function " + context.getTrace() + ": " + name->name);
        return ret;
    }

    llvm::Type *StringTypeNode::getLLVMType(CodegenContext &context)
    {
        return llvm::ArrayType::get(context.getBuilder().getInt8Ty(), 256);
        // return nullptr;
    }

    void RecordTypeNode::append(const std::shared_ptr<VarDeclNode> &var)
    {
        for (auto &f : field)
        {
            if (f->name->name == var->name->name)
                throw std::logic_error("Duplicate name \'" + f->name->name + "\' in record field declare!");
        }
        field.push_back(var);
    }
    void RecordTypeNode::merge(const std::shared_ptr<RecordTypeNode> &rhs)
    {
        for (auto &var : rhs->field)
        {
            field.push_back(var);
        }
        // Check duplicate
        std::set<std::string> nameSet;
        for (auto &f : field)
        {
            if (!nameSet.insert(f->name->name).second)
                throw std::logic_error("Duplicate name \'" + f->name->name + "\' in record field declare!");
        }
    }
    void RecordTypeNode::merge(std::shared_ptr<RecordTypeNode> &&rhs)
    {
        field.merge(std::move(rhs->field));
        // Check duplicate
        std::set<std::string> nameSet;
        for (auto &f : field)
        {
            if (!nameSet.insert(f->name->name).second)
                throw std::logic_error("Duplicate name \'" + f->name->name + "\' in record field declare!");
        }
    }

    llvm::Type *RecordTypeNode::getLLVMType(CodegenContext &context)
    { 
        std::vector<llvm::Type *> fieldTy;
        for (auto &decl: field)
        {
            auto *ty = decl->type->getLLVMType(context);
            if (ty == nullptr || ty->isStructTy())
                throw CodegenException("Unsupported type in record declaration");
            fieldTy.push_back(ty);
        }
        // return llvm::StructType::create(fieldTy);
        return llvm::StructType::get(context.getBuilder().getContext(), fieldTy);
    }

    llvm::Value *RecordTypeNode::getFieldIdx(const std::string &name, CodegenContext &context)
    {
        unsigned i = 0;
        for (auto &f : field)
        {
            if (f->name->name == name)
                return llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), i, false);
            ++i;
        }
        throw CodegenException("Unknown name in record field");
        return nullptr;
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
