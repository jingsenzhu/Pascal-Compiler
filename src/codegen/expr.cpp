#include "utils/ast.hpp"
#include "codegen_context.hpp"
#include <cassert>

namespace spc
{
    llvm::Value *BinaryExprNode::codegen(CodegenContext &context)
    {
        auto *lexp = lhs->codegen(context);
        auto *rexp = rhs->codegen(context);
        std::map<BinaryOp, llvm::CmpInst::Predicate> iCmpMap = {
                {BinaryOp::Gt, llvm::CmpInst::ICMP_SGT},
                {BinaryOp::Geq, llvm::CmpInst::ICMP_SGE},
                {BinaryOp::Lt, llvm::CmpInst::ICMP_SLT},
                {BinaryOp::Leq, llvm::CmpInst::ICMP_SLE},
                {BinaryOp::Eq, llvm::CmpInst::ICMP_EQ},
                {BinaryOp::Neq, llvm::CmpInst::ICMP_NE}
        };
        std::map<BinaryOp, llvm::CmpInst::Predicate> fCmpMap = {
                {BinaryOp::Gt, llvm::CmpInst::FCMP_OGT},
                {BinaryOp::Geq, llvm::CmpInst::FCMP_OGE},
                {BinaryOp::Lt, llvm::CmpInst::FCMP_OLT},
                {BinaryOp::Leq, llvm::CmpInst::FCMP_OLE},
                {BinaryOp::Eq, llvm::CmpInst::FCMP_OEQ},
                {BinaryOp::Neq, llvm::CmpInst::FCMP_ONE}
        };

        if (lexp->getType()->isDoubleTy() || rexp->getType()->isDoubleTy()) 
        {
            if (!lexp->getType()->isDoubleTy()) 
            {
                lexp = context.getBuilder().CreateSIToFP(lexp, context.getBuilder().getDoubleTy());
            }
            else if (!rexp->getType()->isDoubleTy()) 
            {
                rexp = context.getBuilder().CreateSIToFP(rexp, context.getBuilder().getDoubleTy());
            }
            auto it = fCmpMap.find(op);
            if (it != fCmpMap.end())
                return context.getBuilder().CreateFCmp(it->second, lexp, rexp);
            llvm::Instruction::BinaryOps binop;
            switch(op) 
            {
                case BinaryOp::Plus: binop = llvm::Instruction::FAdd; break;
                case BinaryOp::Minus: binop = llvm::Instruction::FSub; break;
                case BinaryOp::Truediv: binop = llvm::Instruction::FDiv; break;
                case BinaryOp::Mul: binop = llvm::Instruction::FMul; break;
                default: throw CodegenException("Invaild operator for REAL type");
            }
            return context.getBuilder().CreateBinOp(binop, lexp, rexp);
        }
        else if (lexp->getType()->isIntegerTy(32) && rexp->getType()->isIntegerTy(32)) 
        {
            auto it = iCmpMap.find(op);
            if (it != iCmpMap.end())
                return context.getBuilder().CreateICmp(it->second, lexp, rexp);
            llvm::Instruction::BinaryOps binop;
            switch(op) 
            {
                case BinaryOp::Plus: binop = llvm::Instruction::Add; break;
                case BinaryOp::Minus: binop = llvm::Instruction::Sub; break;
                case BinaryOp::Div: binop = llvm::Instruction::SDiv; break;
                case BinaryOp::Mod: binop = llvm::Instruction::SRem; break;
                case BinaryOp::Mul: binop = llvm::Instruction::Mul; break;
                case BinaryOp::And: binop = llvm::Instruction::And; break;
                case BinaryOp::Or: binop = llvm::Instruction::Or; break;
                case BinaryOp::Xor: binop = llvm::Instruction::Xor; break;
                case BinaryOp::Truediv:
                    lexp = context.getBuilder().CreateSIToFP(lexp, context.getBuilder().getDoubleTy());
                    rexp = context.getBuilder().CreateSIToFP(rexp, context.getBuilder().getDoubleTy());
                    binop = llvm::Instruction::FDiv; 
                    break;
                default: throw CodegenException("Invaild operator for INTEGER type");
            }
            return context.getBuilder().CreateBinOp(binop, lexp, rexp);
        }
        else if (lexp->getType()->isIntegerTy(1) && rexp->getType()->isIntegerTy(1)) 
        {
            auto it = iCmpMap.find(op);
            if (it != iCmpMap.end())
                return context.getBuilder().CreateICmp(it->second, lexp, rexp);
            llvm::Instruction::BinaryOps binop;
            switch(op) 
            {
                case BinaryOp::And: binop = llvm::Instruction::And; break;
                case BinaryOp::Or: binop = llvm::Instruction::Or; break;
                case BinaryOp::Xor: binop = llvm::Instruction::Xor; break;
                default: throw CodegenException("Invaild operator for BOOLEAN type");
            }
            return context.getBuilder().CreateBinOp(binop, lexp, rexp);
        }
        else if (lexp->getType()->isIntegerTy(8) && rexp->getType()->isIntegerTy(8)) 
        {
            auto it = iCmpMap.find(op);
            if (it != iCmpMap.end())
                return context.getBuilder().CreateICmp(it->second, lexp, rexp);
            else
                throw CodegenException("Invaild operator for CHAR type");
        }
        else
            throw CodegenException("Invaild operation between different types");
    }

    llvm::Value *CustomProcNode::codegen(CodegenContext &context)
    {
        auto *func = context.getModule()->getFunction(name->name);
        if (!func)
            throw CodegenException("Function not found: " + name->name + "()");
        if (func->arg_size() != args->getChildren().size())
            throw CodegenException("Wrong number of arguments: " + name->name + "()");
        std::vector<llvm::Value*> values;
        for (auto &arg : args->getChildren()) 
            values.push_back(arg->codegen(context));
        return context.getBuilder().CreateCall(func, values);
    }

    llvm::Value *SysProcNode::codegen(CodegenContext &context)
    {
        if (name == SysFunc::Write || name == SysFunc::Writeln) {
            for (auto &arg : this->args->getChildren()) {
                auto *value = arg->codegen(context);
                auto x = value->getType();
                std::vector<llvm::Value*> func_args;
                if (value->getType()->isIntegerTy(32)) 
                {
                    func_args.push_back(context.getBuilder().CreateGlobalStringPtr("%d"));
                    func_args.push_back(value);
                }
                else if (value->getType()->isIntegerTy(8)) 
                {
                    func_args.push_back(context.getBuilder().CreateGlobalStringPtr("%c"));
                    func_args.push_back(value);
                }
                else if (value->getType()->isDoubleTy()) 
                {
                    func_args.push_back(context.getBuilder().CreateGlobalStringPtr("%f"));
                    func_args.push_back(value);
                }
                else if (value->getType()->isArrayTy()) // String
                {
                    func_args.push_back(context.getBuilder().CreateGlobalStringPtr("%s"));
                    auto argId = cast_node<IdentifierNode>(arg);
                    auto *valuePtr = argId->getPtr(context);
                    func_args.push_back(valuePtr);
                }
                else if (value->getType()->isPointerTy()) // String
                {
                    func_args.push_back(context.getBuilder().CreateGlobalStringPtr("%s"));
                    func_args.push_back(value);
                }
                else 
                    throw CodegenException("Incompatible type in read(): expected char, integer, real, array, string");
                context.getBuilder().CreateCall(context.printfFunc, func_args);
            }
            if (name == SysFunc::Writeln) {
                context.getBuilder().CreateCall(context.printfFunc, context.getBuilder().CreateGlobalStringPtr("\n"));
            }
            return nullptr;
        }
        else if (name == SysFunc::Read)
        {
            for (auto &arg : args->getChildren())
            {
                auto ptr = cast_node<IdentifierNode>(arg)->getPtr(context);
                std::vector<llvm::Value*> args;
                if (ptr->getType()->getPointerElementType()->isIntegerTy(8))
                { 
                    args.push_back(context.getBuilder().CreateGlobalStringPtr("%c")); 
                    args.push_back(ptr); 
                }
                else if (ptr->getType()->getPointerElementType()->isIntegerTy(32))
                { 
                    args.push_back(context.getBuilder().CreateGlobalStringPtr("%d")); 
                    args.push_back(ptr); 
                }
                else if (ptr->getType()->getPointerElementType()->isDoubleTy())
                { 
                    args.push_back(context.getBuilder().CreateGlobalStringPtr("%lf")); 
                    args.push_back(ptr); 
                }
                // TODO: String support
                else
                    throw CodegenException("Incompatible type in read(): expected char, integer, real");
                context.getBuilder().CreateCall(context.scanfFunc, args);
            }
            return nullptr;
        }
        else if (name == SysFunc::Abs)
        {
            if (args->getChildren().size() != 1)
                throw CodegenException("Wrong number of arguments in abs(): expected 1");
            auto *value = args->getChildren().front()->codegen(context);
            if (value->getType()->isIntegerTy(32))
                return context.getBuilder().CreateCall(context.absFunc, value);
            else if (value->getType()->isDoubleTy())
                return context.getBuilder().CreateCall(context.fabsFunc, value);
            else
                throw CodegenException("Incompatible type in abs(): expected integer, real");
        }
        else if (name == SysFunc::Sqrt)
        {
            if (args->getChildren().size() != 1)
                throw CodegenException("Wrong number of arguments in sqrt(): expected 1");
            auto *value = args->getChildren().front()->codegen(context);
            auto *double_ty = context.getBuilder().getDoubleTy();
            if (value->getType()->isIntegerTy(32))
                value = context.getBuilder().CreateSIToFP(value, double_ty);
            else if (!value->getType()->isDoubleTy())
                throw CodegenException("Incompatible type in sqrt(): expected integer, real");
            return context.getBuilder().CreateCall(context.sqrtFunc, value);
        }
        else if (name == SysFunc::Sqr)
        {
            if (args->getChildren().size() != 1)
                throw CodegenException("Wrong number of arguments: sqr()");
            auto *value = args->getChildren().front()->codegen(context);
            if (value->getType()->isIntegerTy(32))      
                return context.getBuilder().CreateBinOp(llvm::Instruction::Mul, value, value);
            else if (value->getType()->isDoubleTy())    
                return context.getBuilder().CreateBinOp(llvm::Instruction::FMul, value, value);
            else
                throw CodegenException("Incompatible type in sqr(): expected char");
        }
        else if (name == SysFunc::Chr)
        {
            if (args->getChildren().size() != 1)
                throw CodegenException("Wrong number of arguments in chr(): expected 1");
            auto *value = args->getChildren().front()->codegen(context);
            if (!value->getType()->isIntegerTy(32))
                throw CodegenException("Incompatible type in chr(): expected integer");
            return context.getBuilder().CreateTrunc(value, context.getBuilder().getInt8Ty());
        }
        else if (name == SysFunc::Ord)
        {
            if (args->getChildren().size() != 1)
                throw CodegenException("Wrong number of arguments in ord(): expected 1");
            auto *value = args->getChildren().front()->codegen(context);
            if (!value->getType()->isIntegerTy(8))
                throw CodegenException("Incompatible type in ord(): expected char");
            return context.getBuilder().CreateZExt(value, context.getBuilder().getInt32Ty());
        }
        else if (name == SysFunc::Pred)
        {
            if (args->getChildren().size() != 1)
                throw CodegenException("Wrong number of arguments: pred()");
            auto *value = args->getChildren().front()->codegen(context);
            if (!value->getType()->isIntegerTy(8))
                throw CodegenException("Incompatible type in pred(): expected char");
            return context.getBuilder().CreateBinOp(llvm::Instruction::Sub, value, context.getBuilder().getInt8(1));
        }
        else if (name == SysFunc::Succ)
        {
            if (args->getChildren().size() != 1)
                throw CodegenException("Wrong number of arguments: succ()");
            auto *value = args->getChildren().front()->codegen(context);
            if (!value->getType()->isIntegerTy(8))
                throw CodegenException("Incompatible type in succ(): expected char");
            return context.getBuilder().CreateBinOp(llvm::Instruction::Add, value, context.getBuilder().getInt8(1));
        }
    }

    llvm::Value *ArrayRefNode::codegen(CodegenContext &context) 
    {
        return context.getBuilder().CreateLoad(this->getPtr(context));
    }

    llvm::Value *ArrayRefNode::getAssignPtr(CodegenContext &context) 
    {
        llvm::Value *value = arr->getAssignPtr(context);
        assert(value != nullptr);
        auto *idx_value = context.getBuilder().CreateIntCast(this->index->codegen(context), context.getBuilder().getInt32Ty(), true);
        auto *ptr_type = value->getType()->getPointerElementType();
        std::vector<llvm::Value*> idx;
        if (ptr_type->isArrayTy()) 
        {
            idx.push_back(llvm::ConstantInt::getSigned(context.getBuilder().getInt32Ty(), 0));
        }
        else
            throw CodegenException(arr->name + " is not an array");
        idx.push_back(idx_value);
        return context.getBuilder().CreateInBoundsGEP(value, idx);
    }

    llvm::Value *ArrayRefNode::getPtr(CodegenContext &context) 
    {
        llvm::Value *value = arr->getPtr(context);
        assert(value != nullptr);
        auto *idx_value = context.getBuilder().CreateIntCast(this->index->codegen(context), context.getBuilder().getInt32Ty(), true);
        auto *ptr_type = value->getType()->getPointerElementType();
        std::vector<llvm::Value*> idx;
        if (ptr_type->isArrayTy()) 
        {
            idx.push_back(llvm::ConstantInt::getSigned(context.getBuilder().getInt32Ty(), 0));
        }
        else
            throw CodegenException(arr->name + " is not an array");
        idx.push_back(idx_value);
        return context.getBuilder().CreateInBoundsGEP(value, idx);
    }

} // namespace spc
