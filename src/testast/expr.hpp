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

        // llvm::Value *codegen(CodegenContext &) override;
        // void print() override;
        friend class ASTvis;
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

        // llvm::Value *codegen(CodegenContext &) override;
        // void print() override;
        friend class ASTvis;
    };
    
    class ArrayRefNode: public ExprNode
    {
    private:
        std::shared_ptr<IdentifierNode> arr;
        std::shared_ptr<ExprNode> index;
    public:
        ArrayRefNode(const std::shared_ptr<IdentifierNode> &arr, const std::shared_ptr<ExprNode> &index)
            : arr(arr), index(index) {}
        ~ArrayRefNode() = default;

        // llvm::Value *codegen(CodegenContext &) override;
        // llvm::Value *getPtr();
        // void print() override;
        friend class ASTvis;
    };

    class RecordRefNode: public ExprNode
    {
    private:
        std::shared_ptr<IdentifierNode> name;
        std::shared_ptr<IdentifierNode> field;
    public:
        RecordRefNode(const std::shared_ptr<IdentifierNode> &name, const std::shared_ptr<IdentifierNode> &field)
            : name(name), field(field) {}
        ~RecordRefNode() = default;

        // llvm::Value *codegen(CodegenContext &) override { return nullptr; }
        // void print() override;
        friend class ASTvis;
    };

    class ProcNode: public ExprNode
    {
    public:
        ProcNode() = default;
        ~ProcNode() = default;
        // llvm::Value *codegen(CodegenContext &context) = 0;
        // void print() = 0;
    };

    class CustomProcNode: public ProcNode
    {
    private:
        std::shared_ptr<IdentifierNode> name;
        std::shared_ptr<ArgList> args;
    public:
        CustomProcNode(const std::string &name, const std::shared_ptr<ArgList> &args = nullptr) 
            : name(make_node<IdentifierNode>(name)), args(args) {}
        // CustomProcStmtNode(const std::string &name) 
        //     : name(make_node<IdentifierNode>(name)), args(nullptr) {}
        CustomProcNode(const std::shared_ptr<IdentifierNode> &name, const std::shared_ptr<ArgList> &args = nullptr) 
            : name(name), args(args) {}
        // CustomProcStmtNode(const std::shared_ptr<IdentifierNode> &name) 
        //     : name(name), args(nullptr) {}
        ~CustomProcNode() = default;

        // llvm::Value *codegen(CodegenContext &context) override;
        // void print() override;
        friend class ASTvis;
    };

    enum SysFunc { Read, Write, Writeln, Abs, Chr, Odd, Ord, Pred, Sqr, Sqrt, Succ };
    
    class SysProcNode: public ProcNode
    {
    private:
        SysFunc name;
        std::shared_ptr<ArgList> args;
    public:
        SysProcNode(const SysFunc name, const std::shared_ptr<ArgList> &args = nullptr) 
            : name(name), args(args) {}
        // SysProcStmtNode(const SysFunc &name) 
        //     : name(name), args(nullptr) {}
        ~SysProcNode() = default;

        // llvm::Value *codegen(CodegenContext &context) override;
        // void print() override;
        friend class ASTvis;
    };
    

} // namespace spc



#endif