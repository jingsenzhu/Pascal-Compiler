#ifndef DECL_AST
#define DECL_AST

#include "type.hpp"

namespace spc
{
    
    class DeclNode: public BaseNode
    {
    public:
        DeclNode() {}
        ~DeclNode() = default;
    };
    
    class VarDeclNode: public DeclNode
    {
    private:
        std::shared_ptr<IdentifierNode> name;
        std::shared_ptr<TypeNode> type;
    public:
        VarDeclNode(const std::shared_ptr<IdentifierNode>& name, const std::shared_ptr<TypeNode>& type) : name(name), type(type) {}
        ~VarDeclNode() = default;

        llvm::Value *codegen(CodegenContext &) override;
        // void print() override;
        friend class ASTvis;
        llvm::Value *createGlobalArray( CodegenContext &context, const std::shared_ptr<ArrayTypeNode> &);
        llvm::Value *createArray(CodegenContext &context, const std::shared_ptr<ArrayTypeNode> &);
    };

    class ConstDeclNode: public DeclNode
    {
    private:
        std::shared_ptr<IdentifierNode> name;
        std::shared_ptr<ConstValueNode> val;
    public:
        ConstDeclNode(const std::shared_ptr<IdentifierNode>& name, const std::shared_ptr<ConstValueNode>& val) : name(name), val(val) {}
        ~ConstDeclNode() = default;

        llvm::Value *codegen(CodegenContext &) override;
        // void print() override;
        friend class ASTvis;
    };
    
    class TypeDeclNode: public DeclNode
    {
    private:
        std::shared_ptr<IdentifierNode> name;
        std::shared_ptr<TypeNode> type;
    public:
        TypeDeclNode(const std::shared_ptr<IdentifierNode>& name, const std::shared_ptr<TypeNode>& type) : name(name), type(type) {}
        ~TypeDeclNode() = default;

        llvm::Value *codegen(CodegenContext &) override;
        // void print() override;
        friend class ASTvis;
    };

    class ParamNode: public DeclNode
    {
    private:
        std::shared_ptr<IdentifierNode> name;
        std::shared_ptr<TypeNode> type;
    public:
        ParamNode(const std::shared_ptr<IdentifierNode>& name, const std::shared_ptr<TypeNode>& type) : name(name), type(type) {}
        ~ParamNode() = default;

        llvm::Value *codegen(CodegenContext &) override { return nullptr; }
        // void print() override;
        friend class ASTvis;
        friend class RoutineNode;
    };
    
    using TypeDeclList = ListNode<TypeDeclNode>;
    using ConstDeclList = ListNode<ConstDeclNode>;
    using VarDeclList = ListNode<VarDeclNode>;
    using ArgList = ListNode<ExprNode>;
    using ParamList = ListNode<ParamNode>;

} // namespace spc


#endif