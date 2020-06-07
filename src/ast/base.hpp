#ifndef BASE_AST
#define BASE_AST
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Utils.h>
#include <list>
#include <memory>
#include <iostream>
#include "utils/utils.hpp"

namespace spc
{
    class CodegenContext;
    class BaseNode
    {
    public:
        BaseNode() {}
        ~BaseNode() {}
        virtual llvm::Value *codegen(CodegenContext &) = 0;
        // virtual void print() = 0;
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
            // children.merge(std::move(rhs->children));
        }
        void merge(std::shared_ptr<ListNode<T>> &&rhs)
        {
            children.merge(std::move(rhs->children));
        }
        void mergeList(const std::list<std::shared_ptr<T>> &lst)
        {
            for (auto &c: lst)
            {
                children.push_back(c);
            }
            // children.merge(std::move(lst));
        }
        void mergeList(std::list<std::shared_ptr<T>> &&lst)
        {
            children.merge(std::move(lst));
        }
        virtual llvm::Value *codegen(CodegenContext &context)
        {
            for (auto &c : children)
            {
                c->codegen(context);
            }
        }
    };

    class ExprNode: public BaseNode
    {
    public:
        ExprNode() {}
        ~ExprNode() {}
        virtual llvm::Value *codegen(CodegenContext &context) = 0;
        // virtual void print() = 0;
    };

    class LeftExprNode: public ExprNode
    {
    public:
        LeftExprNode() {}
        ~LeftExprNode() = default;
        virtual llvm::Value *codegen(CodegenContext &context) = 0;
        virtual llvm::Value *getPtr(CodegenContext &context) = 0;
        virtual llvm::Value *getAssignPtr(CodegenContext &context) = 0;
        virtual const std::string getSymbolName() = 0;
        // virtual void print() = 0;
    };
    
    class StmtNode: public BaseNode
    {
    public:
        StmtNode() {}
        ~StmtNode() {}
        virtual llvm::Value *codegen(CodegenContext &context) = 0;
        // virtual void print() = 0;
    };
    
    template<typename T, typename... Args>
    std::shared_ptr<T> make_node(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

} // namespace spc


#endif