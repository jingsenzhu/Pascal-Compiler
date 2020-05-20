#ifndef STMT_AST
#define STMT_AST

#include "expr.hpp"

namespace spc
{
    
    using CompoundStmtNode = ListNode<StmtNode>;

    class IfStmtNode: public StmtNode
    {
    private:
        std::shared_ptr<ExprNode> expr;
        std::shared_ptr<CompoundStmtNode> if_stmt;
        std::shared_ptr<CompoundStmtNode> else_stmt;
    public:
        // IfStmtNode(
        //     const std::shared_ptr<ExprNode> &expr, 
        //     const std::shared_ptr<CompoundStmtNode> &if_stmt
        //     ) 
        //     : expr(expr), if_stmt(if_stmt), else_stmt(nullptr) {}
        IfStmtNode(
            const std::shared_ptr<ExprNode> &expr, 
            const std::shared_ptr<CompoundStmtNode> &if_stmt, 
            const std::shared_ptr<CompoundStmtNode> &else_stmt = nullptr
            ) 
            : expr(expr), if_stmt(if_stmt), else_stmt(else_stmt) {}
        ~IfStmtNode() = default;

        llvm::Value *codegen(CodegenContext &context) override;
        void print() override;
    };
    
    class WhileStmtNode: public StmtNode
    {
    private:
        std::shared_ptr<ExprNode> expr;
        std::shared_ptr<CompoundStmtNode> stmt;
    public:
        WhileStmtNode(
            const std::shared_ptr<ExprNode> &expr, 
            const std::shared_ptr<CompoundStmtNode> &stmt
            )
            : expr(expr), stmt(stmt) {}
        ~WhileStmtNode() = default;

        llvm::Value *codegen(CodegenContext &context) override;
        void print() override;
    };

    
    enum ForDirection { To, Downto };
    
    class ForStmtNode: public StmtNode
    {
    private:
        ForDirection direction;
        std::shared_ptr<IdentifierNode> id;
        std::shared_ptr<ExprNode> init_val;
        std::shared_ptr<ExprNode> end_val;
        std::shared_ptr<CompoundStmtNode> stmt;
    public:
        ForStmtNode(
            const std::shared_ptr<IdentifierNode> &id, 
            const std::shared_ptr<ExprNode> &init_val, 
            const std::shared_ptr<ExprNode> &end_val, 
            const std::shared_ptr<CompoundStmtNode> &stmt
            )
            : init_val(init_val), end_val(end_val), stmt(stmt) {}
        ~ForStmtNode() = default;

        llvm::Value *codegen(CodegenContext &context) override;
        void print() override;
    };
    
    class RepeatStmtNode: public StmtNode
    {
    private:
        std::shared_ptr<ExprNode> expr;
        std::shared_ptr<CompoundStmtNode> stmt;
    public:
        RepeatStmtNode(
            const std::shared_ptr<ExprNode> &expr, 
            const std::shared_ptr<CompoundStmtNode> &stmt
            )
            : expr(expr), stmt(stmt) {}
        ~RepeatStmtNode() = default;

        llvm::Value *codegen(CodegenContext &context) override;
        void print() override;
    };

    
    class ProcStmtNode: public StmtNode
    {
    public:
        ProcStmtNode() = default;
        ~ProcStmtNode() = default;
        llvm::Value *codegen(CodegenContext &context) = 0;
        void print() = 0;
    };

    class CustomProcStmtNode: public ProcStmtNode
    {
    private:
        std::shared_ptr<IdentifierNode> name;
        std::shared_ptr<ArgList> args;
    public:
        CustomProcStmtNode(const std::string &name, const std::shared_ptr<ArgList> &args = nullptr) 
            : name(make_node<IdentifierNode>(name)), args(args) {}
        // CustomProcStmtNode(const std::string &name) 
        //     : name(make_node<IdentifierNode>(name)), args(nullptr) {}
        CustomProcStmtNode(const std::shared_ptr<IdentifierNode> &name, const std::shared_ptr<ArgList> &args = nullptr) 
            : name(name), args(args) {}
        // CustomProcStmtNode(const std::shared_ptr<IdentifierNode> &name) 
        //     : name(name), args(nullptr) {}
        ~CustomProcStmtNode() = default;

        llvm::Value *codegen(CodegenContext &context) override;
        void print() override;
    };

    enum SysFunc { Read, Write, Readln, Writeln };
    
    class SysProcStmtNode: public ProcStmtNode
    {
    private:
        SysFunc name;
        std::shared_ptr<ArgList> args;
    public:
        SysProcStmtNode(const SysFunc name, const std::shared_ptr<ArgList> &args = nullptr) 
            : name(name), args(args) {}
        // SysProcStmtNode(const SysFunc &name) 
        //     : name(name), args(nullptr) {}
        ~SysProcStmtNode() = default;

        llvm::Value *codegen(CodegenContext &context) override;
        void print() override;
    };

    class AssignStmtNode: public StmtNode
    {
    private:
        std::shared_ptr<ExprNode> lhs;
        std::shared_ptr<ExprNode> rhs;
    public:
        AssignStmtNode(const std::shared_ptr<ExprNode> &lhs, const std::shared_ptr<ExprNode> &rhs)
            : lhs(lhs), rhs(rhs)
        {
            if (!(is_ptr_of<IdentifierNode>(lhs) || is_ptr_of<IdentifierNode>(lhs)))
            {
                throw CodegenException("Left side of assignment must be identifier or array reference!");
            }
        }
        ~AssignStmtNode() = default;

        llvm::Value *codegen(CodegenContext &context) override;
        void print() override;
    };
    

    // TODO: Case
    class CaseBranchNode: public StmtNode
    {
    private:
        std::shared_ptr<ConstValueNode> branch;
        std::shared_ptr<CompoundStmtNode> stmt;
    public:
        CaseBranchNode(const std::shared_ptr<ConstValueNode> &branch, const std::shared_ptr<CompoundStmtNode> &stmt)
            : branch(branch), stmt(stmt) {}
        ~CaseBranchNode() = default;

        llvm::Value *codegen(CodegenContext &context) override;
        void print() override;
    };

    using CaseBranchList = ListNode<CaseBranchNode>;

    class CaseStmtNode: public StmtNode
    {
    private:
        std::shared_ptr<ExprNode> expr;
        std::list<std::shared_ptr<CaseBranchNode>> branches;
    public:
        CaseStmtNode(const std::shared_ptr<ExprNode> &expr, const CaseBranchList &list)
            : expr(expr), branches(std::move(list->getChildren())) {}
        ~CaseStmtNode();

        llvm::Value *codegen(CodegenContext &context) override;
        void print() override;
    };
    
    CaseStmtNode::CaseStmtNode(/* args */)
    {
    }
    
    CaseStmtNode::~CaseStmtNode()
    {
    }
    
    

} // namespace spc


#endif