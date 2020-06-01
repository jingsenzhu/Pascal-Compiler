#ifndef IDENTIFIER_AST
#define IDENTIFIER_AST

#include "base.hpp"
#include <algorithm>
#include <string>

namespace spc
{
    class IdentifierNode: public LeftExprNode
    {      
    public:
        std::string name;
        IdentifierNode(const std::string &str)
            : name(str) 
        {
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        }
        IdentifierNode(const char *str)
            : name(str) 
        {
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        }
        ~IdentifierNode() = default;

        llvm::Value *codegen(CodegenContext &context) override;
        llvm::Constant *getConstVal(CodegenContext &context);
        llvm::Value *getPtr(CodegenContext &context) override;
        llvm::Value *getAssignPtr(CodegenContext &context) override;
        const std::string getSymbolName() override { return this->name; }
        // void print() override;
    };

    using IdentifierList = ListNode<IdentifierNode>;

} // namespace spc


#endif