#ifndef PROGRAM_AST
#define PROGRAM_AST

#include "stmt.hpp"

namespace spc
{
    
    class RoutineNode;
    using RoutineList = ListNode<RoutineNode>;

    class RoutineHeadNode: public BaseNode
    {
    private:
        std::shared_ptr<ConstDeclList> constList;
        std::shared_ptr<VarDeclList> varList;
        std::shared_ptr<TypeDeclList> typeList;
        std::shared_ptr<RoutineList> subroutineList;
    public:
        RoutineHeadNode(
            const std::shared_ptr<ConstDeclList> &constList,
            const std::shared_ptr<VarDeclList> &varList,
            const std::shared_ptr<TypeDeclList> &typeList,
            const std::shared_ptr<RoutineList> &subroutineList
            )
            : constList(constList), varList(varList), typeList(typeList), subroutineList(subroutineList) {}
        ~RoutineHeadNode() = default;

        // llvm::Value *codegen(CodegenContext &) override;
        // void print() override;
        friend class ASTvis;
    };

    class BaseRoutineNode: public BaseNode
    {
    protected:
        std::shared_ptr<IdentifierNode> name;
        std::shared_ptr<RoutineHeadNode> header;
        std::shared_ptr<CompoundStmtNode> body;
    public:
        BaseRoutineNode(const std::shared_ptr<IdentifierNode> &name, const std::shared_ptr<RoutineHeadNode> &header, const std::shared_ptr<CompoundStmtNode> &body)
            : name(name), header(header), body(body) {}
        ~BaseRoutineNode() = default;

        std::string getName() const { return name->name; }
        // llvm::Value *codegen(CodegenContext &) = 0;
        // void print() = 0;
        friend class ASTvis;
    };

    class RoutineNode: public BaseRoutineNode
    {
    private:
        std::shared_ptr<ParamList> params;
        std::shared_ptr<TypeNode> retType;
    public:
        RoutineNode(
            const std::shared_ptr<IdentifierNode> &name, 
            const std::shared_ptr<RoutineHeadNode> &header, 
            const std::shared_ptr<CompoundStmtNode> &body, 
            const std::shared_ptr<ParamList> &params, 
            const std::shared_ptr<TypeNode> &retType
            )
            : BaseRoutineNode(name, header, body), params(params), retType(retType) {}
        ~RoutineNode() = default;

        // llvm::Value *codegen(CodegenContext &) override;
        // void print() override;
        friend class ASTvis;
    };

    class ProgramNode: public BaseRoutineNode
    {
    public:
        using BaseRoutineNode::BaseRoutineNode;
        ~ProgramNode() = default;

        // llvm::Value *codegen(CodegenContext &) override;
        // void print() override;
        friend class ASTvis;
    };   

} // namespace spc


#endif