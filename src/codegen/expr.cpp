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
        size_t argCnt = 0;
        int index = 0;
        if (args != nullptr)
            argCnt = args->getChildren().size();
        if (func->arg_size() != argCnt)
            throw CodegenException("Wrong number of arguments: " + name->name + "()");
        auto *funcTy = func->getFunctionType();
        std::vector<llvm::Value*> values;
        if (args != nullptr)
            for (auto &arg : args->getChildren())
            {
                llvm::Value *argVal = arg->codegen(context);
                auto *paramTy = funcTy->getParamType(index), *argTy = argVal->getType();
                if (paramTy->isDoubleTy() && argTy->isIntegerTy(32))
                    argVal = context.getBuilder().CreateSIToFP(argVal, paramTy);
                else if (argTy->isDoubleTy() && paramTy->isIntegerTy(32))
                {
                    std::cerr << "Warning: casting REAL type to INTEGER type when calling function " << name->name << "()" << std::endl;
                    argVal = context.getBuilder().CreateFPToSI(argVal, paramTy);
                }
                else if (funcTy->getParamType(index) != argVal->getType())
                    throw CodegenException("Incompatible type in the " + std::to_string(index) + "th arg when calling " + name->name + "()");
                values.push_back(argVal);
                index++;
            }
        // if (func->getReturnType()->isArrayTy())
        // {

        // }
        return context.getBuilder().CreateCall(func, values);
    }

    llvm::Value *SysProcNode::codegen(CodegenContext &context)
    {
        if (name == SysFunc::Write || name == SysFunc::Writeln) {
            context.log() << "\tSysfunc WRITE" << std::endl;
            if (this->args != nullptr)
                for (auto &arg : this->args->getChildren()) {
                    auto *value = arg->codegen(context);
                    assert(value != nullptr);
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
                        auto *a = llvm::cast<llvm::ArrayType>(x);
                        if (!a->getElementType()->isIntegerTy(8))
                            throw CodegenException("Cannot print a non-char array");
                        func_args.push_back(context.getBuilder().CreateGlobalStringPtr("%s"));
                        llvm::Value *valuePtr;
                        if (is_ptr_of<LeftExprNode>(arg))
                        {
                            auto argId = cast_node<LeftExprNode>(arg);
                            valuePtr = argId->getPtr(context);
                        }
                        // else if (is_ptr_of<RecordRefNode>(arg))
                        // {
                        //     auto argId = cast_node<RecordRefNode>(arg);
                        //     valuePtr = argId->getPtr(context);
                        // }
                        // else if (is_ptr_of<ArrayRefNode>(arg))
                        // {
                        //     auto argId = cast_node<ArrayRefNode>(arg);
                        //     valuePtr = argId->getPtr(context);
                        // }
                        else if (is_ptr_of<CustomProcNode>(arg))
                            valuePtr = value;
                        else
                            assert(0);
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
        else if (name == SysFunc::Read || name == SysFunc::Readln)
        {
            context.log() << "\tSysfunc READ" << std::endl;
            if (this->args != nullptr)
                for (auto &arg : args->getChildren())
                {
                    llvm::Value *ptr;
                    if (is_ptr_of<LeftExprNode>(arg))
                        ptr = cast_node<LeftExprNode>(arg)->getPtr(context);
                    // else if (is_ptr_of<ArrayRefNode>(arg))
                    //     ptr = cast_node<ArrayRefNode>(arg)->getPtr(context);
                    // else if (is_ptr_of<RecordRefNode>(arg))
                    //     ptr = cast_node<RecordRefNode>(arg)->getPtr(context);
                    // else if (is_ptr_of<ArrayRefNode>(arg))
                    //     ptr = cast_node<ArrayRefNode>(arg)->getPtr(context);
                    else
                        throw CodegenException("Argument in read() must be identifier or array/record reference");
                    std::vector<llvm::Value*> func_args;
                    if (ptr->getType()->getPointerElementType()->isIntegerTy(8))
                    { 
                        func_args.push_back(context.getBuilder().CreateGlobalStringPtr("%c")); 
                        func_args.push_back(ptr); 
                    }
                    else if (ptr->getType()->getPointerElementType()->isIntegerTy(32))
                    { 
                        func_args.push_back(context.getBuilder().CreateGlobalStringPtr("%d")); 
                        func_args.push_back(ptr); 
                    }
                    else if (ptr->getType()->getPointerElementType()->isDoubleTy())
                    { 
                        func_args.push_back(context.getBuilder().CreateGlobalStringPtr("%lf")); 
                        func_args.push_back(ptr); 
                    }
                    // String support
                    else if (ptr->getType()->getPointerElementType()->isArrayTy() && llvm::cast<llvm::ArrayType>(ptr->getType()->getPointerElementType())->getElementType()->isIntegerTy(8))
                    {
                        if (name == SysFunc::Read)
                            func_args.push_back(context.getBuilder().CreateGlobalStringPtr("%s"));
                        else // Readln
                        {
                            func_args.push_back(context.getBuilder().CreateGlobalStringPtr("%[^\n]"));
                            if (arg != this->args->getChildren().back())
                                std::cerr << "Warning in readln(): string type should be the last argument in readln(), otherwise the subsequent arguments cannot be read!" << std::endl;
                        }
                        func_args.push_back(ptr);
                    }
                    else
                        throw CodegenException("Incompatible type in read(): expected char, integer, real, string");
                    context.getBuilder().CreateCall(context.scanfFunc, func_args);
                }
            if (name == SysFunc::Readln)
            {
                // Flush all other inputs in this line
                context.getBuilder().CreateCall(context.scanfFunc, context.getBuilder().CreateGlobalStringPtr("%*[^\n]"));
                // Flush the final '\n'
                context.getBuilder().CreateCall(context.getcharFunc);
            }
            return nullptr;
        }
        else if (name == SysFunc::Concat)
        {
            context.log() << "\tSysfunc CONCAT" << std::endl;
            std::string format;
            std::list<llvm::Value*> func_args;
            for (auto &arg : this->args->getChildren()) {
                auto *value = arg->codegen(context);
                auto x = value->getType();
                if (value->getType()->isIntegerTy(32)) 
                {
                    format += "%d";
                    func_args.push_back(value);
                }
                else if (value->getType()->isIntegerTy(8)) 
                {
                    format += "%c";
                    func_args.push_back(value);
                }
                else if (value->getType()->isDoubleTy()) 
                {
                    format += "%f";
                    func_args.push_back(value);
                }
                else if (value->getType()->isArrayTy()) // String
                {
                    auto *a = llvm::cast<llvm::ArrayType>(x);
                    if (!a->getElementType()->isIntegerTy(8))
                        throw CodegenException("Cannot concat a non-char array");
                    format += "%s";
                    llvm::Value *valuePtr;
                    if (is_ptr_of<LeftExprNode>(arg))
                    {
                        auto argId = cast_node<LeftExprNode>(arg);
                        valuePtr = argId->getPtr(context);
                    }
                    // else if (is_ptr_of<RecordRefNode>(arg))
                    // {
                    //     auto argId = cast_node<RecordRefNode>(arg);
                    //     valuePtr = argId->getPtr(context);
                    // }
                    // else if (is_ptr_of<ArrayRefNode>(arg))
                    // {
                    //     auto argId = cast_node<ArrayRefNode>(arg);
                    //     valuePtr = argId->getPtr(context);
                    // }
                    else
                        assert(0);
                    func_args.push_back(valuePtr);
                }
                else if (value->getType()->isPointerTy()) // String
                {
                    format += "%s";
                    func_args.push_back(value);
                }
                else 
                    throw CodegenException("Incompatible type in concat(): expected char, integer, real, array, string");        
            }
            func_args.push_front(context.getBuilder().CreateGlobalStringPtr(format.c_str()));
            func_args.push_front(context.getTempStrPtr());
            // sprintf(__tmp_str, "...formats", ...args);
            std::vector<llvm::Value*> func_args_vec(func_args.begin(), func_args.end());
            context.getBuilder().CreateCall(context.sprintfFunc, func_args_vec);
            return context.getTempStrPtr();
        }
        else if (name == SysFunc::Length)
        {
            context.log() << "\tSysfunc LENGTH" << std::endl;
            if (args->getChildren().size() != 1)
                throw CodegenException("Wrong number of arguments in length(): expected 1");
            auto arg = args->getChildren().front();
            auto *value = arg->codegen(context);
            auto *ty = value->getType();
            llvm::Value *zero = llvm::ConstantInt::getSigned(context.getBuilder().getInt32Ty(), 0);
            // context.log() << ty->getTypeID() << std::endl;
            if (ty->isArrayTy() && ty->getArrayElementType()->isIntegerTy(8))
            {
                // context.log() << "\tqqqqqq" << std::endl;
                llvm::Value *valPtr;
                if (is_ptr_of<LeftExprNode>(arg))
                    valPtr = context.getBuilder().CreateInBoundsGEP(cast_node<LeftExprNode>(arg)->getPtr(context), {zero, zero});
                // else if (is_ptr_of<RecordRefNode>(arg))
                //     valPtr = context.getBuilder().CreateInBoundsGEP(cast_node<RecordRefNode>(arg)->getPtr(context), {zero, zero});
                // else if (is_ptr_of<ArrayRefNode>(arg))
                //     valPtr = context.getBuilder().CreateInBoundsGEP(cast_node<ArrayRefNode>(arg)->getPtr(context), {zero, zero});
                else if (is_ptr_of<CustomProcNode>(arg))
                    valPtr = context.getBuilder().CreateInBoundsGEP(value, {zero, zero});
                else
                    assert(0);
                return context.getBuilder().CreateCall(context.strlenFunc, valPtr);
            }
            else if (ty->isPointerTy())
            {
                // context.log() << "\tpppppp" << std::endl;
                if(ty == context.getBuilder().getInt8PtrTy())
                    return context.getBuilder().CreateCall(context.strlenFunc, value);
                else if (ty->getPointerElementType()->isIntegerTy(8))
                {
                    llvm::Value *valPtr = context.getBuilder().CreateGEP(value, zero);
                    return context.getBuilder().CreateCall(context.atoiFunc, valPtr);
                }
                else if (ty->getPointerElementType()->isArrayTy())
                {
                    // context.log() << "\tpppppp" << std::endl;
                    llvm::Value *valPtr = context.getBuilder().CreateInBoundsGEP(value, {zero, zero});
                    return context.getBuilder().CreateCall(context.strlenFunc, valPtr);
                }
                else
                {
                    // context.log() << ty->getPointerElementType()->getTypeID() << std::endl;
                    throw CodegenException("Incompatible type in length(): expected string");
                }
            }
            // else if (ty->isIntegerTy(8))
            // {
            //     if (!is_ptr_of<IdentifierNode>(arg))
            //         throw CodegenException("Incompatible type in length(): expected string2");
            //     std::shared_ptr<std::pair<int,int>> arrEntry;
            //     auto argId = cast_node<IdentifierNode>(arg);
            //     arrEntry = context.getArrayEntry(context.getTrace() + "." + argId->name);
            //     if (arrEntry == nullptr)
            //         throw CodegenException("Incompatible type in length(): expected string3");
            //     return context.getBuilder().CreateCall(context.strlenFunc, argId->getPtr(context));
            // }
            else
            {
                // context.log() << ty->getTypeID() << std::endl;
                throw CodegenException("Incompatible type in length(): expected string");
            }
        }
        else if (name == SysFunc::Abs)
        {
            context.log() << "\tSysfunc ABS" << std::endl;
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
        else if (name == SysFunc::Val)
        {
            context.log() << "\tSysfunc VAL" << std::endl;
            if (args->getChildren().size() != 1)
                throw CodegenException("Wrong number of arguments in val(): expected 1");
            auto arg = args->getChildren().front();
            auto *value = arg->codegen(context);
            auto *ty = value->getType();
            llvm::Value *zero = llvm::ConstantInt::getSigned(context.getBuilder().getInt32Ty(), 0);
            if (ty->isArrayTy() && ty->getArrayElementType()->isIntegerTy(8))
            {
                llvm::Value *valPtr;
                if (is_ptr_of<LeftExprNode>(arg))
                    valPtr = context.getBuilder().CreateInBoundsGEP(cast_node<LeftExprNode>(arg)->getPtr(context), {zero, zero});
                // else if (is_ptr_of<RecordRefNode>(arg))
                //     valPtr = context.getBuilder().CreateInBoundsGEP(cast_node<RecordRefNode>(arg)->getPtr(context), {zero, zero});
                // else if (is_ptr_of<ArrayRefNode>(arg))
                //     valPtr = context.getBuilder().CreateInBoundsGEP(cast_node<ArrayRefNode>(arg)->getPtr(context), {zero, zero});
                else if (is_ptr_of<CustomProcNode>(arg))
                    valPtr = context.getBuilder().CreateInBoundsGEP(value, {zero, zero});
                else
                    assert(0);
                return context.getBuilder().CreateCall(context.atoiFunc, valPtr);
            }
            else if (ty->isPointerTy())
            {
                if(ty == context.getBuilder().getInt8PtrTy())
                    return context.getBuilder().CreateCall(context.atoiFunc, value);
                else if (ty->getPointerElementType()->isIntegerTy(8))
                {
                    llvm::Value *valPtr = context.getBuilder().CreateGEP(value, zero);
                    return context.getBuilder().CreateCall(context.atoiFunc, valPtr);
                }
                else if (ty->getPointerElementType()->isArrayTy())
                {
                    llvm::Value *valPtr = context.getBuilder().CreateInBoundsGEP(value, {zero, zero});
                    return context.getBuilder().CreateCall(context.atoiFunc, valPtr);
                }
                else
                    throw CodegenException("Incompatible type in val(): expected string");
            }
            // else if (ty->isIntegerTy(8))
            // {
            //     if (!is_ptr_of<IdentifierNode>(arg))
            //         throw CodegenException("Incompatible type in val(): expected string");
            //     std::shared_ptr<std::pair<int,int>> arrEntry;
            //     auto argId = cast_node<IdentifierNode>(arg);
            //     // for (auto rit = context.traces.rbegin(); rit != context.traces.rend(); rit++)
            //     // {
            //     //     if ((arrEntry = context.getArrayEntry(*rit + "." + argId->name)) != nullptr)
            //     //         break;
            //     // }
            //     // if (arrEntry == nullptr) arrEntry = context.getArrayEntry(argId->name);
            //     arrEntry = context.getArrayEntry(context.getTrace() + "." + argId->name);
            //     if (arrEntry == nullptr)
            //         throw CodegenException("Incompatible type in val(): expected string");
            //     return context.getBuilder().CreateCall(context.atoiFunc, argId->getPtr(context));
            // }
            else
                throw CodegenException("Incompatible type in val(): expected string");
        }
        else if (name == SysFunc::Str)
        {
            context.log() << "\tSysfunc STR" << std::endl;
            if (args->getChildren().size() != 1)
                throw CodegenException("Wrong number of arguments in str(): expected 1");
            auto arg = args->getChildren().front();
            auto *value = arg->codegen(context);
            auto *ty = value->getType();
            llvm::Value *zero = llvm::ConstantInt::getSigned(context.getBuilder().getInt32Ty(), 0);
            if (ty->isIntegerTy(8))
            {
                context.getBuilder().CreateCall(context.sprintfFunc, {context.getTempStrPtr(), context.getBuilder().CreateGlobalStringPtr("%c"), value});
                return context.getTempStrPtr();
            }
            else if (ty->isIntegerTy(32))
            {
                context.getBuilder().CreateCall(context.sprintfFunc, {context.getTempStrPtr(), context.getBuilder().CreateGlobalStringPtr("%d"), value});
                return context.getTempStrPtr();
            }
            else if (ty->isDoubleTy())
            {
                context.getBuilder().CreateCall(context.sprintfFunc, {context.getTempStrPtr(), context.getBuilder().CreateGlobalStringPtr("%f"), value});
                return context.getTempStrPtr();
            }
            else
                throw CodegenException("Incompatible type in str(): expected integer, char, real");
        }
        else if (name == SysFunc::Abs)
        {
            context.log() << "\tSysfunc ABS" << std::endl;
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
            context.log() << "\tSysfunc SQRT" << std::endl;
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
            context.log() << "\tSysfunc SQR" << std::endl;
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
            context.log() << "\tSysfunc CHR" << std::endl;
            if (args->getChildren().size() != 1)
                throw CodegenException("Wrong number of arguments in chr(): expected 1");
            auto *value = args->getChildren().front()->codegen(context);
            if (!value->getType()->isIntegerTy(32))
                throw CodegenException("Incompatible type in chr(): expected integer");
            return context.getBuilder().CreateTrunc(value, context.getBuilder().getInt8Ty());
        }
        else if (name == SysFunc::Ord)
        {
            context.log() << "\tSysfunc ORD" << std::endl;
            if (args->getChildren().size() != 1)
                throw CodegenException("Wrong number of arguments in ord(): expected 1");
            auto *value = args->getChildren().front()->codegen(context);
            if (!value->getType()->isIntegerTy(8))
                throw CodegenException("Incompatible type in ord(): expected char");
            return context.getBuilder().CreateZExt(value, context.getBuilder().getInt32Ty());
        }
        else if (name == SysFunc::Pred)
        {
            context.log() << "\tSysfunc PRED" << std::endl;
            if (args->getChildren().size() != 1)
                throw CodegenException("Wrong number of arguments: pred()");
            auto *value = args->getChildren().front()->codegen(context);
            if (!value->getType()->isIntegerTy(8))
                throw CodegenException("Incompatible type in pred(): expected char");
            return context.getBuilder().CreateBinOp(llvm::Instruction::Sub, value, context.getBuilder().getInt8(1));
        }
        else if (name == SysFunc::Succ)
        {
            context.log() << "\tSysfunc SUCC" << std::endl;
            if (args->getChildren().size() != 1)
                throw CodegenException("Wrong number of arguments: succ()");
            auto *value = args->getChildren().front()->codegen(context);
            if (!value->getType()->isIntegerTy(8))
            {
                // context.log() << value->getType()->getTypeID() << std::endl;
                throw CodegenException("Incompatible type in succ(): expected char");
            }
            return context.getBuilder().CreateBinOp(llvm::Instruction::Add, value, context.getBuilder().getInt8(1));
        }
    }

    llvm::Value *RecordRefNode::codegen(CodegenContext &context)
    {
        auto *ptr = this->getPtr(context);
        // if (ptr->getType()->getPointerElementType()->isArrayTy())
        // {
        //     // llvm::Value *zero = llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), 0, false);
        //     context.log() << "\tSTRING in RECORD" << std::endl;
        //     // return context.getBuilder().CreateInBoundsGEP(ptr, {zero, zero});
        //     return ptr;
        // }
        return context.getBuilder().CreateLoad(ptr);
    }
    llvm::Value *RecordRefNode::getPtr(CodegenContext &context)
    {
        llvm::Value *value = name->getPtr(context);
        assert(value != nullptr);
        std::shared_ptr<RecordTypeNode> recTy = nullptr;
        for (auto rit = context.traces.rbegin(); rit != context.traces.rend(); rit++)
        {
            if ((recTy = context.getRecordAlias(*rit + "." + name->getSymbolName())) != nullptr)
                break;
        }
        if (recTy == nullptr) recTy = context.getRecordAlias(name->getSymbolName());
        if (recTy == nullptr) throw CodegenException(name->getSymbolName() + " is not a record");
        assert(value->getType()->getPointerElementType()->isStructTy());
	    llvm::Value *idx = recTy->getFieldIdx(field->name, context);
        if (idx == nullptr)
            throw CodegenException("'" + field->name + "' is not in record field of " + name->getSymbolName());
        llvm::Value *zero = llvm::ConstantInt::get(context.getBuilder().getInt32Ty(), 0, false);
        return context.getBuilder().CreateInBoundsGEP(value, {zero, idx});
    }
    llvm::Value *RecordRefNode::getAssignPtr(CodegenContext &context)
    {
        return this->getPtr(context);
    }
    const std::string RecordRefNode::getSymbolName()
    {
        return this->name->getSymbolName() + "." + this->field->name;
    }

    llvm::Value *ArrayRefNode::codegen(CodegenContext &context) 
    {
        return context.getBuilder().CreateLoad(this->getPtr(context));
    }

    llvm::Value *ArrayRefNode::getAssignPtr(CodegenContext &context) 
    {
        llvm::Value *value = arr->getAssignPtr(context);
        assert(value != nullptr);
        return this->getPtr(context);
    }
    const std::string ArrayRefNode::getSymbolName()
    {
        return this->arr->getSymbolName() + "[]";
    }

    llvm::Value *ArrayRefNode::getPtr(CodegenContext &context) 
    {
        llvm::Value *value = arr->getPtr(context);
        assert(value != nullptr);
        auto *idx_value = context.getBuilder().CreateIntCast(this->index->codegen(context), context.getBuilder().getInt32Ty(), true);
        auto *ptr_type = value->getType()->getPointerElementType();
        std::vector<llvm::Value*> idx;
        // context.log() << "\tPtr elem type: " << ptr_type->getTypeID() << std::endl;
        // context.log() << "\tType: " << value->getType()->getTypeID() << std::endl;

        idx.push_back(llvm::ConstantInt::getSigned(context.getBuilder().getInt32Ty(), 0));
        std::shared_ptr<std::pair<int,int>> range;
        // bool is_local = false;
        for (auto rit = context.traces.rbegin(); rit != context.traces.rend(); rit++)
        {
            if ((range = context.getArrayEntry(*rit + "." + arr->getSymbolName())) != nullptr)
            {
                // is_local = true;
                break;
            }
        }
        if (range == nullptr)
            range = context.getArrayEntry(arr->getSymbolName());
        if (ptr_type->isArrayTy())
        {
            if (range == nullptr) std::cout << arr->getSymbolName() << std::endl;
            assert(range != nullptr && "Fatal error: Array not found in array table!");
        }
        else if (range == nullptr)
            throw CodegenException(arr->getSymbolName() + " is not an array");
        
        llvm::ConstantInt *const_idx = llvm::dyn_cast<llvm::ConstantInt>(idx_value);
        if (const_idx != nullptr)
        {
            int int_idx = const_idx->getSExtValue();
            if (int_idx < range->first || int_idx > range->second)
                std::cerr << "Warning: index out of bound when visiting array '" + arr->getSymbolName() + "'" << std::endl;
        }
        if (range->first != 0)
        {
            llvm::Value *range_start = llvm::ConstantInt::getSigned(context.getBuilder().getInt32Ty(), range->first);
            llvm::Value *trueIdx = context.getBuilder().CreateBinOp(llvm::Instruction::Sub, idx_value, range_start);
            if (ptr_type->isArrayTy() /*&& !is_local*/)
                idx.push_back(trueIdx);
            else
                return context.getBuilder().CreateGEP(value, trueIdx);
        }
        else
        {
            if (ptr_type->isArrayTy() /*&& !is_local*/)
                idx.push_back(idx_value);
            else
                return context.getBuilder().CreateGEP(value, idx_value);
        }
        return context.getBuilder().CreateInBoundsGEP(value, idx);
    }

} // namespace spc
