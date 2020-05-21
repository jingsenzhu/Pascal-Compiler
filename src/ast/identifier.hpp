#ifndef IDENTIFIER_AST
#define IDENTIFIER_AST

#include "base.hpp"
#include <algorithm>
#include <string>

namespace spc
{
    class IdentifierNode: public ExprNode
    {      
    public:
        std::string name;
        IdentifierNode(const char *str)
            : name(str) 
        {
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        }
        ~IdentifierNode() = default;

        llvm::Value *codegen(CodegenContext &context) override;
        llvm::Value *getPtr(CodegenContext &context);
        void print() override;
    };

    using IdentifierList = ListNode<IdentifierNode>;

} // namespace spc


#endif