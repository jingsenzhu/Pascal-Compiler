#ifndef BASE_AST
#define BASE_AST

#include <list>
#include <memory>
#include <iostream>
#include <cassert>
#include "codegen/codegen_context.hpp"
#include "utils/utils.hpp"

namespace spc
{
    
    class BaseNode
    {
    public:
        BaseNode() {}
        ~BaseNode() {}
        virtual llvm::Value *codegen(CodegenContext &) = 0;
        virtual void print() = 0;
    };

    template <typename T>
    class ListNode: public BaseNode
    {
    private:
        std::list<std::shared_ptr<T>> children;
    public:
        ListNode() {}
        ListNode(const std::shared_ptr<T> &val) { children.push_back(val); }
        ListNode(std::shared_ptr<T> &&val) { children.push_back(val); }
        ~ListNode() {}

        std::list<std::shared_ptr<T>> &getChildren() { return children; }

        void append(const std::shared_ptr<T> &val) { children.push_back(val); }
        void append(std::shared_ptr<T> &&val) { children.push_back(val); }

        void merge(const std::shared_ptr<ListNode<T>> &rhs)
        {
            for (auto &c: rhs->children)
            {
                children.push_back(c);
            }           
        }
        void mergeList(const std::list<std::shared_ptr<T>> &lst)
        {
            for (auto &c: lst)
            {
                children.push_back(c);
            }
        }
        virtual llvm::Value *codegen(CodegenContext &context)
        {
            for (auto &c : children)
            {
                c->codegen(context);
            }
        }
        virtual void print()
        {
            std::cout << "===== Start List =====" << std::endl;
            for (auto &c : children)
            {
                c->print();
            }
        }
    };

    class ExprNode: public BaseNode
    {
    public:
        ExprNode() {}
        ~ExprNode() {}
        virtual llvm::Value *codegen(CodegenContext &context) = 0;
        virtual void print() = 0;
    };
    
    class StmtNode: public BaseNode
    {
    public:
        StmtNode() {}
        ~StmtNode() {}
        virtual llvm::Value *codegen(CodegenContext &context) = 0;
        virtual void print() = 0;
    };
    
    template<typename T, typename... Args>
    std::shared_ptr<T> make_node(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

} // namespace spc


#endif