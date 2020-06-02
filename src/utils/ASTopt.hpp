#ifndef __ASTOPT__H__
#define __ASTOPT__H__

#include "utils/ast.hpp"
#include <variant>

namespace spc
{
    
    class ASTopt
    {
    public:
        using ExprVal = std::variant<bool, char, int, double>;
        ASTopt() = default;
        ~ASTopt() = default;
        void operator()(std::shared_ptr<BaseRoutineNode> prog);
    private:
        int computeBoolExpr(const std::shared_ptr<ExprNode>& expr);
        std::pair<bool, ExprVal> computeExpr(const std::shared_ptr<ExprNode>& expr);
        template<typename T> bool cmp(T lhs, T rhs, BinaryOp op);
        void opt(std::shared_ptr<CompoundStmtNode> &stmt);
    };


} // namespace spc


#endif