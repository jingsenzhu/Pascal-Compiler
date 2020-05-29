#include "utils/ast.hpp"
#include "codegen_context.hpp"

namespace spc
{
    
    llvm::Value *IfStmtNode::codegen(CodegenContext &context)
    {
        auto *cond = expr->codegen(context);
        if (!cond->getType()->isIntegerTy(1))       
            throw CodegenException("Incompatible type in if condition: expected boolean");

        auto *func = context.getBuilder().GetInsertBlock()->getParent();
        auto *then_block = llvm::BasicBlock::Create(context.getModule()->getContext(), "then", func);
        auto *else_block = llvm::BasicBlock::Create(context.getModule()->getContext(), "else");
        auto *cont_block = llvm::BasicBlock::Create(context.getModule()->getContext(), "cont");
        context.getBuilder().CreateCondBr(cond, then_block, else_block);

        context.getBuilder().SetInsertPoint(then_block);
        if_stmt->codegen(context);
        context.getBuilder().CreateBr(cont_block);

        func->getBasicBlockList().push_back(else_block);
        context.getBuilder().SetInsertPoint(else_block);
        if (else_stmt != nullptr)
            else_stmt->codegen(context);
        context.getBuilder().CreateBr(cont_block);

        func->getBasicBlockList().push_back(cont_block);
        context.getBuilder().SetInsertPoint(cont_block);
        return nullptr;
    }

    llvm::Value *WhileStmtNode::codegen(CodegenContext &context)
    {
        auto *func = context.getBuilder().GetInsertBlock()->getParent();
        auto *while_block = llvm::BasicBlock::Create(context.getModule()->getContext(), "while", func);
        auto *loop_block = llvm::BasicBlock::Create(context.getModule()->getContext(), "loop", func);
        auto *cont_block = llvm::BasicBlock::Create(context.getModule()->getContext(), "cont");
        context.getBuilder().CreateBr(while_block);

        context.getBuilder().SetInsertPoint(while_block);
        auto *cond = expr->codegen(context);
        if (!cond->getType()->isIntegerTy(1))
        { throw CodegenException("Incompatible type in while condition: expected boolean"); }
        context.getBuilder().CreateCondBr(cond, loop_block, cont_block);

        context.getBuilder().SetInsertPoint(loop_block);
        stmt->codegen(context);
        context.getBuilder().CreateBr(while_block);

        func->getBasicBlockList().push_back(cont_block);
        context.getBuilder().SetInsertPoint(cont_block);
        return nullptr;
    }

    llvm::Value *ForStmtNode::codegen(CodegenContext &context)
    {
        if (!id->codegen(context)->getType()->isIntegerTy(32))
            throw CodegenException("Incompatible type in for iterator: expected int");

        auto init = make_node<AssignStmtNode>(id, init_val);
        auto upto = direction == ForDirection::To;
        auto cond = make_node<BinaryExprNode>(upto ? BinaryOp::Leq : BinaryOp::Geq, id, end_val);
        auto iter = make_node<AssignStmtNode>(id,
                make_node<BinaryExprNode>(upto ? BinaryOp::Plus :BinaryOp::Minus, id, make_node<IntegerNode>(1)));
        auto compound = make_node<CompoundStmtNode>();
        compound->merge(stmt); 
        compound->append(iter);
        auto while_stmt = make_node<WhileStmtNode>(cond, compound);

        init->codegen(context);
        while_stmt->codegen(context);
        return nullptr;
    }

    llvm::Value *RepeatStmtNode::codegen(CodegenContext &context)
    {
        auto *func = context.getBuilder().GetInsertBlock()->getParent();
        auto *block = llvm::BasicBlock::Create(context.getModule()->getContext(), "repeat", func);
        context.getBuilder().CreateBr(block);
        context.getBuilder().SetInsertPoint(block);
        
        stmt->codegen(context);
        auto *cond = expr->codegen(context);
        if (!cond->getType()->isIntegerTy(1))
            throw CodegenException("Incompatible type in repeat condition: expected boolean");
        auto *cont = llvm::BasicBlock::Create(context.getModule()->getContext(), "cont", func);
        context.getBuilder().CreateCondBr(cond, cont, block);

        context.getBuilder().SetInsertPoint(cont);
        return nullptr;
    }

    llvm::Value *ProcStmtNode::codegen(CodegenContext &context)
    {
        return call->codegen(context);
    }

    llvm::Value *AssignStmtNode::codegen(CodegenContext &context)
    {
        llvm::Value *lhs;
        if (is_ptr_of<IdentifierNode>(this->lhs))
        {
            auto id_cast = cast_node<IdentifierNode>(this->lhs);
            lhs = id_cast->getAssignPtr(context);
        }
        else if (is_ptr_of<ArrayRefNode>(this->lhs))
        {
            auto arr_cast = cast_node<ArrayRefNode>(this->lhs);
            lhs = arr_cast->getAssignPtr(context);
        }
        else if (is_ptr_of<RecordRefNode>(this->lhs))
        {
            auto rec_cast = cast_node<RecordRefNode>(this->lhs);
            lhs = rec_cast->getPtr(context);
        }
        else
            throw CodegenException("Assignment left argument not a identifier.");
        auto *rhs = this->rhs->codegen(context);
        auto *lhs_type = lhs->getType()->getPointerElementType();
        auto *rhs_type = rhs->getType();
        if (lhs_type->isDoubleTy() && rhs_type->isIntegerTy(32))
        {
            rhs = context.getBuilder().CreateSIToFP(rhs, context.getBuilder().getDoubleTy());
        }
        else if (lhs_type->isArrayTy())
        {
            if (!lhs_type->getArrayElementType()->isIntegerTy(8))
                throw CodegenException("Cannot assign to a non-char array");
            llvm::Value *zero = llvm::ConstantInt::getSigned(context.getBuilder().getInt32Ty(), 0);
            llvm::Value *lhsPtr = context.getBuilder().CreateInBoundsGEP(lhs, {zero, zero});;
            llvm::Value *rhsPtr;
            if (rhs_type->isPointerTy())
            {
                std::cout << "Sysfunc STRCPY" << std::endl;
                if (rhs_type->getPointerElementType()->isArrayTy())
                    rhsPtr = context.getBuilder().CreateInBoundsGEP(rhs, {zero, zero});
                else
                    rhsPtr = rhs;
                context.getBuilder().CreateCall(context.strcpyFunc, {lhsPtr, rhsPtr});
                return nullptr;
            }
            else if (rhs_type->isArrayTy())
            {
                std::cout << "Sysfunc SPRINTF" << std::endl;
                if (is_ptr_of<IdentifierNode>(this->rhs))
                    rhsPtr = context.getBuilder().CreateInBoundsGEP(cast_node<IdentifierNode>(this->rhs)->getPtr(context), {zero, zero});
                else if (is_ptr_of<RecordRefNode>(this->rhs))
                    rhsPtr = cast_node<RecordRefNode>(this->rhs)->getPtr(context);
                else if (is_ptr_of<CustomProcNode>(this->rhs))
                    rhsPtr = context.getBuilder().CreateInBoundsGEP(rhs, {zero, zero});
                if (!rhs_type->getArrayElementType()->isIntegerTy(8))
                    throw CodegenException("Cannot assign to a non-char array");
                context.getBuilder().CreateCall(context.sprintfFunc, {lhsPtr, context.getBuilder().CreateGlobalStringPtr("%s"), rhsPtr});
                return nullptr;
            }
            else if (rhs_type->isIntegerTy(8))
            {
                std::cout << "Sysfunc STRCPY" << std::endl;
                if (!is_ptr_of<IdentifierNode>(this->rhs))
                    throw CodegenException("Incompatible type in assignment");
                auto rhsId = cast_node<IdentifierNode>(this->rhs);
                auto arrEntry = context.getArrayEntry(context.getTrace() + "_" + rhsId->name);
                if (arrEntry == nullptr)
                    throw CodegenException("Incompatible type in assignment");
                rhsPtr = rhsId->getPtr(context);
                context.getBuilder().CreateCall(context.strcpyFunc, {lhsPtr, rhsPtr});
                return nullptr;
            }
        }
        else if (lhs_type->isIntegerTy(8))
        {
            if (!is_ptr_of<IdentifierNode>(this->lhs))
                throw CodegenException("Incompatible type in assignment");
            auto lhsId = cast_node<IdentifierNode>(this->lhs);
            auto arrEntry = context.getArrayEntry(context.getTrace() + "_" + lhsId->name);
            if (arrEntry != nullptr)
            {
                llvm::Value *zero = llvm::ConstantInt::getSigned(context.getBuilder().getInt32Ty(), 0);
                llvm::Value *lhsPtr = lhsId->getAssignPtr(context);
                llvm::Value *rhsPtr;
                if (rhs_type->isPointerTy())
                {
                    std::cout << "Sysfunc STRCPY" << std::endl;
                    if (rhs_type->getPointerElementType()->isArrayTy())
                        rhsPtr = context.getBuilder().CreateInBoundsGEP(rhs, {zero, zero});
                    else
                        rhsPtr = rhs;
                    context.getBuilder().CreateCall(context.strcpyFunc, {lhsPtr, rhsPtr});
                    return nullptr;
                }
                else if (rhs_type->isArrayTy())
                {
                    std::cout << "Sysfunc SPRINTF" << std::endl;
                    if (is_ptr_of<IdentifierNode>(this->rhs))
                        rhsPtr = context.getBuilder().CreateInBoundsGEP(cast_node<IdentifierNode>(this->rhs)->getPtr(context), {zero, zero});
                    else if (is_ptr_of<RecordRefNode>(this->rhs))
                        rhsPtr = cast_node<RecordRefNode>(this->rhs)->getPtr(context);
                    else if (is_ptr_of<CustomProcNode>(this->rhs))
                        rhsPtr = context.getBuilder().CreateInBoundsGEP(rhs, {zero, zero});
                    if (!rhs_type->getArrayElementType()->isIntegerTy(8))
                        throw CodegenException("Cannot assign to a non-char array");
                    context.getBuilder().CreateCall(context.sprintfFunc, {lhsPtr, context.getBuilder().CreateGlobalStringPtr("%s"), rhsPtr});
                    return nullptr;
                }
                else if (rhs_type->isIntegerTy(8))
                {
                    std::cout << "Sysfunc STRCPY" << std::endl;
                    if (!is_ptr_of<IdentifierNode>(this->rhs))
                        throw CodegenException("Incompatible type in assignment");
                    auto rhsId = cast_node<IdentifierNode>(this->rhs);
                    auto arrEntry = context.getArrayEntry(context.getTrace() + "_" + rhsId->name);
                    if (arrEntry == nullptr)
                        throw CodegenException("Incompatible type in assignment");
                    rhsPtr = rhsId->getPtr(context);
                    context.getBuilder().CreateCall(context.strcpyFunc, {lhsPtr, rhsPtr});
                    return nullptr;
                }
            }
        }
        // else if (lhs_type->isArrayTy() && rhs_type->isPointerTy()) // const string
        // {
        //     if (!lhs_type->getArrayElementType()->isIntegerTy(8))
        //         throw CodegenException("Cannot assign to a non-char array");
        //     std::cout << "Sysfunc STRCPY" << std::endl;
        //     llvm::Value *zero = llvm::ConstantInt::getSigned(context.getBuilder().getInt32Ty(), 0);
        //     auto *lhsPtr = context.getBuilder().CreateInBoundsGEP(lhs, {zero, zero});
        //     llvm::Value *rhsPtr;
        //     if (rhs_type->getPointerElementType()->isArrayTy())
        //         rhsPtr = context.getBuilder().CreateInBoundsGEP(rhs, {zero, zero});
        //     else
        //         rhsPtr = rhs;
        //     context.getBuilder().CreateCall(context.strcpyFunc, {lhsPtr, rhsPtr});
        //     return nullptr;
        // }
        // else if (lhs_type->isArrayTy() && rhs_type->isArrayTy())
        // {
        //     llvm::Value *zero = llvm::ConstantInt::getSigned(context.getBuilder().getInt32Ty(), 0);
        //     llvm::Value *rhsPtr;
        //     if (is_ptr_of<IdentifierNode>(this->rhs))
        //         rhsPtr = context.getBuilder().CreateInBoundsGEP(cast_node<IdentifierNode>(this->rhs)->getPtr(context), {zero, zero});
        //     else if (is_ptr_of<RecordRefNode>(this->rhs))
        //         rhsPtr = cast_node<RecordRefNode>(this->rhs)->getPtr(context);
        //     else if (is_ptr_of<CustomProcNode>(this->rhs))
        //         rhsPtr = context.getBuilder().CreateInBoundsGEP(rhs, {zero, zero});
        //     if (!lhs_type->getArrayElementType()->isIntegerTy(8) || !llvm::cast<llvm::ArrayType>(rhs_type)->getElementType()->isIntegerTy(8))
        //         throw CodegenException("Cannot assign to a non-char array");
        //     auto *lhsPtr = context.getBuilder().CreateInBoundsGEP(lhs, {zero, zero});
        //     context.getBuilder().CreateCall(context.sprintfFunc, {lhsPtr, context.getBuilder().CreateGlobalStringPtr("%s"), rhsPtr});
        //     return nullptr;
        // }
        if (!((lhs_type->isIntegerTy(1) && rhs_type->isIntegerTy(1))  // bool
                   || (lhs_type->isIntegerTy(32) && rhs_type->isIntegerTy(32))  // int
                   || (lhs_type->isIntegerTy(8) && rhs_type->isIntegerTy(8))  // char
                   || (lhs_type->isDoubleTy() && rhs_type->isDoubleTy()) // float
                   || (lhs_type->isArrayTy() && rhs_type->isIntegerTy(32)))) // string
            throw CodegenException("Incompatible type in assignment");
        context.getBuilder().CreateStore(rhs, lhs);
        return nullptr;
    }

    llvm::Value *CaseStmtNode::codegen(CodegenContext &context)
    {
        auto *value = expr->codegen(context);
        if (!value->getType()->isIntegerTy())
            throw CodegenException("Incompatible type in case statement: expected char, integer");
        auto *func = context.getBuilder().GetInsertBlock()->getParent();
        auto *cont = llvm::BasicBlock::Create(context.getModule()->getContext(), "cont");
        auto *switch_inst = context.getBuilder().CreateSwitch(value, cont, static_cast<unsigned int>(branches.size()));
        for (auto &branch : branches)
        {
            llvm::ConstantInt *constant;
            if (is_ptr_of<ConstValueNode>(branch->branch))
                constant = llvm::cast<llvm::ConstantInt>(branch->branch->codegen(context));
            else // ID node
                constant = context.getConstInt(cast_node<IdentifierNode>(branch->branch)->name);
            auto *block = llvm::BasicBlock::Create(context.getModule()->getContext(), "case", func);
            context.getBuilder().SetInsertPoint(block);
            branch->stmt->codegen(context);
            context.getBuilder().CreateBr(cont);
            switch_inst->addCase(constant, block);
        }
        func->getBasicBlockList().push_back(cont);
        context.getBuilder().SetInsertPoint(cont);
        return nullptr;
    }

} // namespace spc
