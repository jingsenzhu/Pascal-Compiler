#ifndef __ASTOPT__H__
#define __ASTOPT__H__

#include "utils/ast.hpp"
#include <variant>

namespace spc
{
    
    class ASTopt
    {
    private:
        using ExprVal = std::variant<int, char, double>;
        int computeBoolExpr(const std::shared_ptr<ExprNode>& expr);
        ExprVal computeExpr(const std::shared_ptr<ExprNode>& expr);
    public:
        ASTopt() = default;
        ~ASTopt() = default;
        void operator()(std::shared_ptr<ProgramNode> &prog);
    };


} // namespace spc


#endif