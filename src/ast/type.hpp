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
        TypeNode(const Type type = Unknown) : type(type) {}
        ~TypeNode() {}
        llvm::Value *codegen(CodegenContext &) override { return nullptr; };
        llvm::Type *getLLVMType(CodegenContext &) = 0;
        // void print() override;
    };

    class VoidTypeNode: public TypeNode
    {
    public:
        VoidTypeNode() : TypeNode(Type::Void) {}
        ~VoidTypeNode() = default;
        llvm::Type *getLLVMType(CodegenContext &) { throw CodegenException("Unknown type!"); }
        // void print() override;
    };
    
    class SimpleTypeNode: public TypeNode
    {
    public:
        SimpleTypeNode(const Type type) : TypeNode(type) {}
        ~SimpleTypeNode() = default;
        llvm::Type *getLLVMType(CodegenContext &) override;
        // void print() override;
    };
    
    class StringTypeNode: public TypeNode
    {
    public:
        StringTypeNode() : TypeNode(Type::String) {}
        ~StringTypeNode() = default;
        llvm::Type *getLLVMType(CodegenContext &) override { return nullptr; }
        // void print() override;
    };

    class ArrayTypeNode: public TypeNode
    {
    public:
        std::shared_ptr<ExprNode> range_start;
        std::shared_ptr<ExprNode> range_end;
        std::shared_ptr<TypeNode> itemType;

        ArrayTypeNode(
            const std::shared_ptr<ExprNode> &start,
            const std::shared_ptr<ExprNode> &end,
            const std::shared_ptr<TypeNode> &itype
        ) : TypeNode(Type::Array), range_start(start), range_end(end), itemType(itype) {}
        ~ArrayTypeNode() = default;
        llvm::Type *getLLVMType(CodegenContext &) override { return nullptr; }
        // void print() override;
    };

    class AliasTypeNode: public TypeNode
    {
    private:
        std::shared_ptr<IdentifierNode> name;
    public:
        AliasTypeNode(const std::shared_ptr<IdentifierNode> &name)
            : name(name) {}
        ~AliasTypeNode() = default;
        llvm::Type *getLLVMType(CodegenContext &context) override 
        { 
            return context.getAlias(name->name); 
        }
        // void print() override;
    };

    class VarDeclNode;
    
    // TODO: RecordTypeNode
    class RecordTypeNode: public TypeNode
    {
    private:
        std::list<std::shared_ptr<VarDeclNode>> field;
    public:
        RecordTypeNode(const std::shared_ptr<IdentifierList> &names, const std::shared_ptr<TypeNode> &type)
            : TypeNode(Type::Record)
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
            for (auto &var : rhs->field)
            {
                field.push_back(var);
            }
        }
        void merge(std::shared_ptr<RecordTypeNode> &&rhs)
        {
            field.merge(std::move(rhs->field));
        }
        llvm::Type *getLLVMType(CodegenContext &) override { return nullptr; }
        // void print() override;
    };
    

    class ConstValueNode: public ExprNode
    {
    public:
        Type type;
        ConstValueNode(const Type type): type(Type::Unknown) {}
        ~ConstValueNode() = default;

        llvm::Type *getLLVMType(CodegenContext &context)
        {
            switch (type) 
            {
                case Type::Bool: return context.getBuilder().getInt1Ty();
                case Type::Int: return context.getBuilder().getInt32Ty();
                case Type::Long: return context.getBuilder().getInt32Ty();
                case Type::Char: return context.getBuilder().getInt8Ty();
                case Type::Real: return context.getBuilder().getDoubleTy();
                case Type::String: throw CodegenException("String currently not supported.\n");
                default: return nullptr;
            }
            return nullptr;
        }
        // void print() override;
    };
    
    class BooleanNode: public ConstValueNode
    {
    public:
        bool val;
        BooleanNode(const bool val = false): ConstValueNode(Type::Bool), val(val) {}
        ~BooleanNode() = default;

        llvm::Value *codegen(CodegenContext &) override;
        // void print() override;
    };

    class IntegerNode: public ConstValueNode
    {
    public:
        int val;
        IntegerNode(const int val = 0): ConstValueNode(Type::Int), val(val) {}
        ~IntegerNode() = default;

        llvm::Value *codegen(CodegenContext &) override;
        // void print() override;
    };

    class RealNode: public ConstValueNode
    {
    public:
        double val;
        RealNode(const double val = 0.0): ConstValueNode(Type::Real), val(val) {}
        ~RealNode() = default;

        llvm::Value *codegen(CodegenContext &) override;
        // void print() override;
    };

    class CharNode: public ConstValueNode
    {
    public:
        char val;
        CharNode(const char val = '\0'): ConstValueNode(Type::Char), val(val) {}
        ~CharNode() = default;

        llvm::Value *codegen(CodegenContext &) override;
        // void print() override;
    };

    class StringNode: public ConstValueNode
    {
    public:
        std::string val;
        StringNode(const char *val = ""): ConstValueNode(Type::String), val(val) {}
        StringNode(const std::string &val): ConstValueNode(Type::String), val(val) {}
        ~StringNode() = default;

        llvm::Value *codegen(CodegenContext &) override;
        // void print() override;
    };
    
    
    

} // namespace spc


#endif
