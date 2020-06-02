#include "ASTopt.hpp"
#include <cmath>

using namespace spc;

static std::set<BinaryOp> cmpOp = { BinaryOp::Eq, BinaryOp::Neq, BinaryOp::Leq, BinaryOp::Geq, BinaryOp::Lt, BinaryOp::Gt };

template<typename T>
bool ASTopt::cmp(const T lhs, const T rhs, Type op)
{
    switch (op)
    {
    case BinaryOp::Eq:
        return lhs == rhs;
    case BinaryOp::Neq:
        return lhs != rhs;
    case BinaryOp::Leq:
        return lhs <= rhs;
    case BinaryOp::Geq:
        return lhs >= rhs;
    case BinaryOp::Lt:
        return lhs < rhs;
    case BinaryOp::Gt:
        return lhs > rhs;
    default:
        throw std::invalid_argument("Invalid argument!");
    }
}

int ASTopt::computeBoolExpr(const std::shared_ptr<ExprNode>& expr)
/*
Return val:
    0: false
    1: true
    2: unknown
*/
{
    if (is_ptr_of<ConstValueNode>(expr))
    {
        auto c = cast_node<ConstValueNode>(expr);
        if (c->type == Type::Bool)
            return (int)cast_node<BooleanNode>(c)->val;
        return 2;
    }
    else if (is_ptr_of<BinaryExprNode>(expr))
    {
        auto b = cast_node<BinaryExprNode>(expr);
        bool lb, rb;
        auto lhs = computeExpr(b->lhs), rhs = computeExpr(b->rhs);
        try
        {
            lb = std::get<bool>(lhs), rb = std::get<bool>(rhs);
        }
        catch(const std::bad_variant_access&)
        {
            return 2;
        }
        switch (b->op)
        {
        case BinaryOp::And:
            return lb and rb;
            break;
        case BinaryOp::Or:
            return lb or rb;
        case BinaryOp::Xor:
            return lb xor rb;
        default:
            if (cmpOp.find(b->op) != cmpOp.end()) return cmp(lb, rb, b->op);
            return 2;
        }
    }
    return 2;
}


std::pair<bool, ASTopt::ExprVal> ASTopt::computeExpr(const std::shared_ptr<ExprNode>& expr)
{
    ASTopt::ExprVal ret;
    if (is_ptr_of<ConstValueNode>(expr))
    {
        auto c = cast_node<ConstValueNode>(expr);
        switch (c->type)
        {
        case Type::Int:
            ret = cast_node<IntegerNode>(c)->val;
            return std::make_pair(true, ret);
        case Type::Char:
            ret = cast_node<CharNode>(c)->val;
            return std::make_pair(true, ret);
        case Type::Real:
            ret = cast_node<RealNode>(c)->val;
            return std::make_pair(true, ret);
        default:
            return std::make_pair(false, ret);
        }
    }
    else if (is_ptr_of<BinaryExprNode>(expr))
    {
        auto b = cast_node<BinaryExprNode>(expr);
        auto lret = computeExpr(b->lhs), rret = computeExpr(b->rhs);
        if (lret.first && rret.first)
        {
            auto lhs = lret.second, rhs = rret.second;
            bool lb, rb;
            char lc, rc;
            int li, ri;
            double ld, rd;
            switch ((lhs.index() << 2) | rhs.index())
            {
            case 0b0000: // bool op bool
                try
                {
                    lb = std::get<bool>(lhs), rb = std::get<bool>(rhs);
                }
                catch(const std::bad_variant_access&)
                {
                    return std::make_pair(false, ret);
                }
                switch (b->op)
                {
                case BinaryOp::And:
                    ret = lb and rb;
                    return std::make_pair(true, ret);
                case BinaryOp::Or:
                    ret = lb or rb;
                    return std::make_pair(true, ret);
                case BinaryOp::Xor:
                    ret = lb xor rb;
                    return std::make_pair(true, ret);
                default:
                    if (cmpOp.find(b->op) != cmpOp.end()) return std::make_pair(true, cmp(lb, rb, b->op));
                    return std::make_pair(false, ret);
                }
            case 0b0101: // char op char
                try
                {
                    lc = std::get<char>(lhs), rc = std::get<char>(rhs);
                }
                catch(const std::bad_variant_access&)
                {
                    return std::make_pair(false, ret);
                }
                if (cmpOp.find(b->op) != cmpOp.end()) return std::make_pair(true, cmp(lc, rc, b->op));
                return std::make_pair(false, ret);
            case 0b1010: // int op int
                try
                {
                    li = std::get<int>(lhs), ri = std::get<int>(rhs);
                }
                catch(const std::bad_variant_access&)
                {
                    return std::make_pair(false, ret);
                }
                switch (b->op)
                {
                case BinaryOp::And:
                    ret = li & ri;
                    return std::make_pair(true, ret);
                case BinaryOp::Or:
                    ret = li | ri;
                    return std::make_pair(true, ret);
                case BinaryOp::Xor:
                    ret = li ^ ri;
                    return std::make_pair(true, ret);
                case BinaryOp::Plus:
                    ret = li + ri;
                    return std::make_pair(true, ret);
                case BinaryOp::Minus:
                    ret = li - ri;
                    return std::make_pair(true, ret);
                case BinaryOp::Mul:
                    ret = li * ri;
                    return std::make_pair(true, ret);
                case BinaryOp::Div:
                    ret = li / ri;
                    return std::make_pair(true, ret);
                case BinaryOp::Mod:
                    ret = li % ri;
                    return std::make_pair(true, ret);
                case BinaryOp::Truediv:
                    ret = (double)li / (double)ri;
                    return std::make_pair(true, ret);
                default:
                    if (cmpOp.find(b->op) != cmpOp.end()) return std::make_pair(true, cmp(li, ri, b->op));
                    return std::make_pair(false, ret);
                }
            case 0b1111: // real op real
                try
                {
                    ld = std::get<double>(lhs), rd = std::get<double>(rhs);
                }
                catch(const std::bad_variant_access&)
                {
                    return std::make_pair(false, ret);
                }
                switch (b->op)
                {
                case BinaryOp::Plus:
                    ret = ld + rd;
                    return std::make_pair(true, ret);
                case BinaryOp::Minus:
                    ret = ld - rd;
                    return std::make_pair(true, ret);
                case BinaryOp::Mul:
                    ret = ld * rd;
                    return std::make_pair(true, ret);
                case BinaryOp::Truediv:
                    ret = ld / rd;
                    return std::make_pair(true, ret);
                default:
                    if (cmpOp.find(b->op) != cmpOp.end()) return std::make_pair(true, cmp(ld, rd, b->op));
                    return std::make_pair(false, ret);
                }
            case 0b1011: // int op real
                try
                {
                    ld = (double)std::get<int>(lhs), rd = std::get<double>(rhs);
                }
                catch(const std::bad_variant_access&)
                {
                    return std::make_pair(false, ret);
                }
                switch (b->op)
                {
                case BinaryOp::Plus:
                    ret = ld + rd;
                    return std::make_pair(true, ret);
                case BinaryOp::Minus:
                    ret = ld - rd;
                    return std::make_pair(true, ret);
                case BinaryOp::Mul:
                    ret = ld * rd;
                    return std::make_pair(true, ret);
                case BinaryOp::Truediv:
                    ret = ld / rd;
                    return std::make_pair(true, ret);
                default:
                    if (cmpOp.find(b->op) != cmpOp.end()) return std::make_pair(true, cmp(ld, rd, b->op));
                    return std::make_pair(false, ret);
                }
            case 0b1110: // real op int
                try
                {
                    ld = std::get<double>(lhs), rd = (double)std::get<int>(rhs);
                }
                catch(const std::bad_variant_access&)
                {
                    return std::make_pair(false, ret);
                }
                switch (b->op)
                {
                case BinaryOp::Plus:
                    ret = ld + rd;
                    return std::make_pair(true, ret);
                case BinaryOp::Minus:
                    ret = ld - rd;
                    return std::make_pair(true, ret);
                case BinaryOp::Mul:
                    ret = ld * rd;
                    return std::make_pair(true, ret);
                case BinaryOp::Truediv:
                    ret = ld / rd;
                    return std::make_pair(true, ret);
                default:
                    if (cmpOp.find(b->op) != cmpOp.end()) return std::make_pair(true, cmp(ld, rd, b->op));
                    return std::make_pair(false, ret);
                }
            default:
                return std::make_pair(false, ret);
            }
        }
        else
            return std::make_pair(false, ret);   
    }
    else if (is_ptr_of<SysProcNode>(expr))
    {
        auto p = cast_node<SysProcNode>(expr);
        auto arg = p->args->getChildren().front();
        auto argVal = computeExpr(arg);
        if (!argVal.first) return std::make_pair(false, ret);
        int iVal;
        double dVal;
        char cVal;
        switch (p->name)
        {
        case SysFunc::Sqr:
            if (argVal.second.index() == 2)
            {
                iVal = std::get<int>(argVal.second);
                ret = iVal * iVal;
                return std::make_pair(true, ret);
            }
            else if (argVal.second.index() == 3)
            {
                dVal = std::get<double>(argVal.second);
                ret = dVal * dVal;
                return std::make_pair(true, ret);
            }
            return std::make_pair(false, ret);
        case SysFunc::Sqrt:
            if (argVal.second.index() == 2)
            {
                iVal = std::get<int>(argVal.second);
                ret = sqrt(iVal);
                return std::make_pair(true, ret);
            }
            else if (argVal.second.index() == 3)
            {
                dVal = std::get<double>(argVal.second);
                ret = sqrt(dVal);
                return std::make_pair(true, ret);
            }
            return std::make_pair(false, ret);
        case SysFunc::Ord:
            try
            {
                cVal = std::get<char>(argVal.second);
                ret = (int)cVal;
                return std::make_pair(true, ret);
            }
            catch(const std::bad_variant_access&)
            {
                return std::make_pair(false, ret);
            }
            return std::make_pair(false, ret);
        case SysFunc::Chr:
            try
            {
                iVal = std::get<int>(argVal.second);
                ret = (char)iVal;
                return std::make_pair(true, ret);
            }
            catch(const std::bad_variant_access&)
            {
                return std::make_pair(false, ret);
            }
            return std::make_pair(false, ret);
        case SysFunc::Pred:
            try
            {
                cVal = std::get<char>(argVal.second);
                ret = (char)(cVal - 1);
                return std::make_pair(true, ret);
            }
            catch(const std::bad_variant_access&)
            {
                return std::make_pair(false, ret);
            }
            return std::make_pair(false, ret);
        case SysFunc::Succ:
            try
            {
                cVal = std::get<char>(argVal.second);
                ret = (char)(cVal + 1);
                return std::make_pair(true, ret);
            }
            catch(const std::bad_variant_access&)
            {
                return std::make_pair(false, ret);
            }
            return std::make_pair(false, ret);
        default:
            break;
        }
    }
    return std::make_pair(false, ret);
}

void ASTopt::opt(std::shared_ptr<CompoundStmtNode> &stmt)
{
    auto &stmt_list = stmt->getChildren();
    for (auto itr = stmt_list.begin(); itr != stmt_list.end(); itr++)
    {
        auto stmt = *itr;
        if (is_ptr_of<AssignStmtNode>(stmt))
        {
            auto ass = cast_node<AssignStmtNode>(stmt);
            auto lhs = ass->rhs;
            auto res = computeExpr(rhs);
            if (res.first)
            {
                std::shared_ptr<ConstValueNode> val;
                switch (res.second.index())
                {
                case 0:
                    val = make_node<BooleanNode>(std::get<bool>(res.second));
                    ass->rhs = val;
                    break;
                case 1:
                    val = make_node<CharNode>(std::get<char>(res.second));
                    ass->rhs = val;
                    break;
                case 2:
                    val = make_node<IntegerNode>(std::get<int>(res.second));
                    ass->rhs = val;
                    break;
                case 3:
                    val = make_node<RealNode>(std::get<double>(res.second));
                    ass->rhs = val;
                    break;
                default:
                    break;
                }
            }
        }
        else if (is_ptr_of<IfStmtNode>(stmt))
        {
            auto ifs = cast_node<IfStmtNode>(stmt);
            int cond = computeBoolExpr(ifs->expr);
            if (cond == 0) // If condition always false
                *itr = ifs->else_stmt; // always else
            else if (cond == 1) // If condition always true
                *itr = ifs->if_stmt; // always then
        }
        else if (is_ptr_of<WhileStmtNode>(stmt))
        {
            auto whs = cast_node<WhileStmtNode>(stmt);
            int cond = computeBoolExpr(whs->expr);
            if (cond == 0) // While condition always false
            {
                auto last = --itr;
                stmt_list.erase(++itr); // Remove useless loop
                itr = last;
            }
            else if (cond == 1) // While condition always true
                std::cerr << "Warning: Dead loop detected" << std::endl; // Warn
        }
        else if (is_ptr_of<RepeatStmtNode>(stmt))
        {
            auto rps = cast_node<RepeatStmtNode>(stmt);
            int cond = computeBoolExpr(rps->expr);
            if (cond == 1) // Repeat condition always true
                *itr = rps->stmt; // Remove useless repeat statement
            else if (cond == 0) // Repeat condition always false
                std::cerr << "Warning: Dead loop detected" << std::endl; // Warn
        }
    }
}

void ASTopt::operator()(std::shared_ptr<BaseRoutineNode> &prog)
{
    for (auto &routine : prog->header->subroutineList->getChildren())
        this->operator()(routine);
    opt(prog->body);
}