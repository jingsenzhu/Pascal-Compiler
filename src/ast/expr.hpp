#ifndef EXPR_AST
#define EXPR_AST

#include "decl.hpp"

namespace spc
{
    
    enum UnaryOp { Pos, Neg, Not };
    enum BinaryOp
    {
        Plus, Minus, Mul, Div, Mod, Truediv, And, Or, Xor,
        Eq, Neq, Gt, Lt, Geq, Leq
    };

    class BinaryExprNode: public ExprNode
    {
    private:
        BinaryOp op;
        std::shared_ptr<ExprNode> lhs, rhs;
    public:
        BinaryExprNode(
            const BinaryOp op, 
            const std::shared_ptr<ExprNode>& lval, 
            const std::shared_ptr<ExprNode>& rval
            ) 
            : op(op), lhs(lval), rhs(rval) {}
        ~BinaryExprNode() = default;

        llvm::Value *codegen(CodegenContext &) override;
        void print() override;
    };

    class UnaryExprNode: public ExprNode
    {
    private:
        UnaryOp op;
        std::shared_ptr<ExprNode> rhs;
    public:
        UnaryExprNode(
            const UnaryOp op, 
            const std::shared_ptr<ExprNode>& rval
            ) 
            : op(op), rhs(rval) {}
        ~UnaryExprNode() = default;

        llvm::Value *codegen(CodegenContext &) override;
        void print() override;
    };
    

} // namespace spc



#endif