#ifndef TYPE_AST
#define TYPE_AST

#include "base.hpp"
#include "identifier.hpp"
#include <string>

namespace spc
{
    
    enum Type { Unknown, Void, Int, Real, String, Array, Record, Bool, Long, Char };

    class TypeNode: public BaseNode
    {
    public:
        Type type;
        TypeNode() {}
        ~TypeNode() {}
        llvm::Value *codegen(CodegenContext &) override { return nullptr; };
        llvm::Type *getLLVMType(CodegenContext &);
        void print() override;
    };

    class VoidTypeNode: public TypeNode
    {
    public:
        VoidTypeNode() : type(Type::Void) {}
        ~VoidTypeNode() = default;
        void print() override;
    };
    
    class SimpleTypeNode: public TypeNode
    {
    public:
        SimpleTypeNode(const Type type) : type(type) {}
        ~SimpleTypeNode() = default;
        void print() override;
    };
    
    class StringTypeNode: public TypeNode
    {
    public:
        StringTypeNode() : type(Type::String) {}
        ~StringTypeNode() = default;
        void print() override;
    };

    class ArrayTypeNode: public TypeNode
    {
    public:
        std::shared_ptr<ExprNode> range_start;
        std::shared_ptr<ExprNode> range_end;
        std::shared_ptr<TypeNode> itemType;

        ArrayTypeNode() : type(Type::String) {}
        ~ArrayTypeNode() = default;
        void print() override;
    };

    class AliasTypeNode: public TypeNode
    {
    private:
        std::shared_ptr<IdentifierNode> name;
    public:
        AliasTypeNode(const std::shared_ptr<IdentifierNode> &name)
            : name(name) {}
        ~AliasTypeNode() = default;
        void print() override;
    };

    class VarDeclNode;
    
    // TODO: RecordTypeNode
    class RecordTypeNode: public TypeNode
    {
    private:
        std::list<std::shared_ptr<VarDeclNode>> field;
    public:
        RecordTypeNode(const std::shared_ptr<IdentifierList> &names, const std::shared_ptr<SimpleTypeNode> &type)
        {
            for (auto &id : names->getChildren())
            {
                field.push_back(make_node<VarDeclNode>(id, type));
            }
        }
        ~RecordTypeNode() = default;
        
        void append(const std::shared_ptr<VarDeclNode> &var)
        {
            field.push_back(var);
        }
        void merge(const std::shared_ptr<RecordTypeNode> &rhs)
        {
            field.merge(std::move(rhs->field));
        }
        void print() override;
    };
    

    class ConstValueNode: ExprNode
    {
    public:
        Type type = Type::Unknown;
        ConstValueNode() {}
        ~ConstValueNode() = default;

        llvm::Type *getLLVMType(CodegenContext &context)
        {
            switch (type) 
            {
                case Type::Bool: return context.GetBuilder().getInt1Ty();
                case Type::Int: return context.GetBuilder().getInt32Ty();
                case Type::Long: return context.GetBuilder().getInt32Ty();
                case Type::Real: return context.GetBuilder().getDoubleTy();
                case Type::String: throw CodegenException("String currently not supported.\n");
                default: return nullptr;
            }
        }
        void print() override;
    };
    
    class BooleanNode: public ConstValueNode
    {
    public:
        bool val;
        BooleanNode(const bool val = false): type(Type::Bool), val(val) {}
        ~BooleanNode() = default;

        llvm::Type *codegen(CodegenContext &) override;
        void print() override;
    };

    class IntegerNode: public ConstValueNode
    {
    public:
        int val;
        IntegerNode(const int val = 0): type(Type::Int), val(val) {}
        ~IntegerNode() = default;

        llvm::Type *codegen(CodegenContext &) override;
        void print() override;
    };

    class RealNode: public ConstValueNode
    {
    public:
        double val;
        RealNode(const double val = 0.0): type(Type::Real), val(val) {}
        ~RealNode() = default;

        llvm::Type *codegen(CodegenContext &) override;
        void print() override;
    };

    class CharNode: public ConstValueNode
    {
    public:
        char val;
        CharNode(const char val = '\0'): type(Type::Char), val(val) {}
        ~CharNode() = default;

        llvm::Type *codegen(CodegenContext &) override;
        void print() override;
    };

    class StringNode: public ConstValueNode
    {
    public:
        std::string val;
        StringNode(const char *val = ""): type(Type::String), val(val) {}
        StringNode(const std::string &val): type(Type::String), val(val) {}
        ~StringNode() = default;

        llvm::Type *codegen(CodegenContext &) override;
        void print() override;
    };
    
    
    

} // namespace spc


#endif