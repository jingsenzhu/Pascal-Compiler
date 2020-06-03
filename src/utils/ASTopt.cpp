#include "ASTopt.hpp"
#include <cmath>

using namespace spc;

static std::set<BinaryOp> cmpOp = { BinaryOp::Eq, BinaryOp::Neq, BinaryOp::Leq, BinaryOp::Geq, BinaryOp::Lt, BinaryOp::Gt };

template<typename T>
bool ASTopt::cmp(T lhs, T rhs, BinaryOp op)
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
    auto res = computeExpr(expr);
    if (res.first != Type::Bool) return 2;
    bool b = res.second.bval;
    return (int)b;
}


std::pair<Type, ASTopt::ExprVal> ASTopt::computeExpr(const std::shared_ptr<ExprNode>& expr)
{
    ASTopt::ExprVal ret;
    if (is_ptr_of<ConstValueNode>(expr))
    {
        auto c = cast_node<ConstValueNode>(expr);
        switch (c->type)
        {
        case Type::Bool:
            ret.bval = cast_node<BooleanNode>(c)->val;
            return std::make_pair(Type::Bool, ret);
        case Type::Int:
            ret.ival = cast_node<IntegerNode>(c)->val;
            return std::make_pair(Type::Int, ret);
        case Type::Char:
            ret.cval = cast_node<CharNode>(c)->val;
            return std::make_pair(Type::Char, ret);
        case Type::Real:
            ret.dval = cast_node<RealNode>(c)->val;
            return std::make_pair(Type::Real, ret);
        default:
            return std::make_pair(Type::Unknown, ret);
        }
    }
    else if (is_ptr_of<BinaryExprNode>(expr))
    {
        auto b = cast_node<BinaryExprNode>(expr);
        auto lret = computeExpr(b->lhs), rret = computeExpr(b->rhs);
        if (lret.first != Type::Unknown && rret.first != Type::Unknown)
        {
            auto lhs = lret.second, rhs = rret.second;
            bool lb, rb;
            char lc, rc;
            int li, ri;
            double ld, rd;
            if (lret.first == Type::Bool && rret.first == Type::Bool)
            {
                lb = lhs.bval, rb = rhs.bval; 
                switch (b->op)
                {
                case BinaryOp::And:
                    ret.bval = lb and rb;
                    return std::make_pair(Type::Bool, ret);
                case BinaryOp::Or:
                    ret.bval = lb or rb;
                    return std::make_pair(Type::Bool, ret);
                case BinaryOp::Xor:
                    ret.bval = lb xor rb;
                    return std::make_pair(Type::Bool, ret);
                default:
                    if (cmpOp.find(b->op) != cmpOp.end())
                    {
                        ret.bval = cmp(lb, rb, b->op);
                        return std::make_pair(Type::Bool, ret);
                    }
                    return std::make_pair(Type::Unknown, ret);
                }
            }
            else if (lret.first == Type::Char && rret.first == Type::Char)
            {
                lc = lhs.cval, rc = rhs.cval;
                if (cmpOp.find(b->op) != cmpOp.end())
                {
                    ret.bval = cmp(lc, rc, b->op);
                    return std::make_pair(Type::Bool, ret);
                }
                return std::make_pair(Type::Unknown, ret);
            }
            else if (lret.first == Type::Int && rret.first == Type::Int)
            {
                li = lhs.ival, ri = rhs.ival;
                switch (b->op)
                {
                case BinaryOp::And:
                    ret.ival = li & ri;
                    return std::make_pair(Type::Int, ret);
                case BinaryOp::Or:
                    ret.ival = li | ri;
                    return std::make_pair(Type::Int, ret);
                case BinaryOp::Xor:
                    ret.ival = li ^ ri;
                    return std::make_pair(Type::Int, ret);
                case BinaryOp::Plus:
                    ret.ival = li + ri;
                    return std::make_pair(Type::Int, ret);
                case BinaryOp::Minus:
                    ret.ival = li - ri;
                    return std::make_pair(Type::Int, ret);
                case BinaryOp::Mul:
                    ret.ival = li * ri;
                    return std::make_pair(Type::Int, ret);
                case BinaryOp::Div:
                    ret.ival = li / ri;
                    return std::make_pair(Type::Int, ret);
                case BinaryOp::Mod:
                    ret.ival = li % ri;
                    return std::make_pair(Type::Int, ret);
                case BinaryOp::Truediv:
                    ret.dval = (double)li / (double)ri;
                    return std::make_pair(Type::Real, ret);
                default:
                    if (cmpOp.find(b->op) != cmpOp.end())
                    {
                        ret.bval = cmp(li, ri, b->op);
                        return std::make_pair(Type::Bool, ret);
                    }
                    return std::make_pair(Type::Unknown, ret);
                }
            }
            else if (lret.first == Type::Real && rret.first == Type::Real)
            {
                ld = lhs.dval, rd = rhs.dval;
                switch (b->op)
                {
                case BinaryOp::Plus:
                    ret.dval = ld + rd;
                    return std::make_pair(Type::Real, ret);
                case BinaryOp::Minus:
                    ret.dval = ld - rd;
                    return std::make_pair(Type::Real, ret);
                case BinaryOp::Mul:
                    ret.dval = ld * rd;
                    return std::make_pair(Type::Real, ret);
                case BinaryOp::Truediv:
                    ret.dval = ld / rd;
                    return std::make_pair(Type::Real, ret);
                default:
                    if (cmpOp.find(b->op) != cmpOp.end())
                    {
                        ret.bval = cmp(ld, rd, b->op);
                        return std::make_pair(Type::Bool, ret);
                    } 
                    return std::make_pair(Type::Unknown, ret);
                }
            }
            else if (lret.first == Type::Int && rret.first == Type::Real)
            {
                ld = (double)lhs.ival, rd = rhs.dval;
                switch (b->op)
                {
                case BinaryOp::Plus:
                    ret.dval = ld + rd;
                    return std::make_pair(Type::Real, ret);
                case BinaryOp::Minus:
                    ret.dval = ld - rd;
                    return std::make_pair(Type::Real, ret);
                case BinaryOp::Mul:
                    ret.dval = ld * rd;
                    return std::make_pair(Type::Real, ret);
                case BinaryOp::Truediv:
                    ret.dval = ld / rd;
                    return std::make_pair(Type::Real, ret);
                default:
                    if (cmpOp.find(b->op) != cmpOp.end())
                    {
                        ret.bval = cmp(ld, rd, b->op);
                        return std::make_pair(Type::Bool, ret);
                    } 
                    return std::make_pair(Type::Unknown, ret);
                }
            }
            else if (lret.first == Type::Real && rret.first == Type::Int)
            {
                ld = lhs.dval, rd = (double)rhs.ival;
                switch (b->op)
                {
                case BinaryOp::Plus:
                    ret.dval = ld + rd;
                    return std::make_pair(Type::Real, ret);
                case BinaryOp::Minus:
                    ret.dval = ld - rd;
                    return std::make_pair(Type::Real, ret);
                case BinaryOp::Mul:
                    ret.dval = ld * rd;
                    return std::make_pair(Type::Real, ret);
                case BinaryOp::Truediv:
                    ret.dval = ld / rd;
                    return std::make_pair(Type::Real, ret);
                default:
                    if (cmpOp.find(b->op) != cmpOp.end())
                    {
                        ret.bval = cmp(ld, rd, b->op);
                        return std::make_pair(Type::Bool, ret);
                    } 
                    return std::make_pair(Type::Unknown, ret);
                }
            }
            return std::make_pair(Type::Unknown, ret);
        }
        else
            return std::make_pair(Type::Unknown, ret);   
    }
    else if (is_ptr_of<SysProcNode>(expr))
    {
        auto p = cast_node<SysProcNode>(expr);
        if (p->args->getChildren().size() > 1) return std::make_pair(Type::Unknown, ret);
        auto arg = p->args->getChildren().front();
        auto argVal = computeExpr(arg);
        if (argVal.first == Type::Unknown) return std::make_pair(Type::Unknown, ret);
        int iVal;
        double dVal;
        char cVal;
        switch (p->name)
        {
        case SysFunc::Sqr:
            if (argVal.first == Type::Int)
            {
                iVal = argVal.second.ival;
                ret.ival = iVal * iVal;
                return std::make_pair(Type::Int, ret);
            }
            else if (argVal.first == Type::Real)
            {
                dVal = argVal.second.dval;
                ret.dval = dVal * dVal;
                return std::make_pair(Type::Real, ret);
            }
            return std::make_pair(Type::Unknown, ret);
        case SysFunc::Sqrt:
            if (argVal.first == Type::Int)
            {
                iVal = argVal.second.ival;
                ret.dval = sqrt(iVal);
                return std::make_pair(Type::Real, ret);
            }
            else if (argVal.first == Type::Real)
            {
                dVal = argVal.second.dval;
                ret.dval = sqrt(dVal);
                return std::make_pair(Type::Real, ret);
            }
            return std::make_pair(Type::Unknown, ret);
        case SysFunc::Ord:
            if (argVal.first == Type::Char)
            {
                cVal = argVal.second.cval;
                ret.cval = (int)cVal;
                return std::make_pair(Type::Int, ret);
            }
            return std::make_pair(Type::Unknown, ret);
        case SysFunc::Chr:
            if (argVal.first == Type::Int)
            {
                iVal = argVal.second.ival;
                ret.cval = (char)iVal;
                return std::make_pair(Type::Char, ret);
            }
            return std::make_pair(Type::Unknown, ret);
        case SysFunc::Pred:
            if (argVal.first == Type::Char)
            {
                cVal = argVal.second.cval;
                ret.cval = (char)(cVal - 1);
                return std::make_pair(Type::Char, ret);
            }
            return std::make_pair(Type::Unknown, ret);
        case SysFunc::Succ:
            if (argVal.first == Type::Char)
            {
                cVal = argVal.second.cval;
                ret.cval = (char)(cVal + 1);
                return std::make_pair(Type::Char, ret);
            }
            return std::make_pair(Type::Unknown, ret);
        default:
            break;
        }
    }
    return std::make_pair(Type::Unknown, ret);
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
            auto rhs = ass->rhs;
            auto res = computeExpr(rhs);
            if (res.first != Type::Unknown)
            {
                std::shared_ptr<ConstValueNode> val;
                switch (res.first)
                {
                case Type::Bool:
                    val = make_node<BooleanNode>(res.second.bval);
                    ass->rhs = val;
                    break;
                case Type::Char:
                    val = make_node<CharNode>(res.second.cval);
                    ass->rhs = val;
                    break;
                case Type::Int:
                    val = make_node<IntegerNode>(res.second.ival);
                    ass->rhs = val;
                    break;
                case Type::Real:
                    val = make_node<RealNode>(res.second.dval);
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
            {
                auto &elseList = ifs->else_stmt->getChildren();
                for (auto &s : elseList)
                    stmt_list.insert(itr, s);
                // *itr = ifs->else_stmt; // always else
                // erase original if stmt
                auto last = --itr;
                stmt_list.erase(++itr);
                itr = last;
            }
            else if (cond == 1) // If condition always true
            {
                auto &thenList = ifs->if_stmt->getChildren();
                for (auto &t : thenList)
                    stmt_list.insert(itr, t);
                // *itr = ifs->if_stmt; // always then
                // erase original if stmt
                auto last = --itr;
                stmt_list.erase(++itr);
                itr = last;
            }
        }
        else if (is_ptr_of<WhileStmtNode>(stmt))
        {
            auto whs = cast_node<WhileStmtNode>(stmt);
            int cond = computeBoolExpr(whs->expr);
            if (cond == 0) // While condition always false
            {
                auto last = --itr;
                // std::cout << 666666 << std::endl;
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
            {
                auto &rl = rps->stmt->getChildren();
                for (auto &s : rl)
                    stmt_list.insert(itr, s);
                // erase original if stmt
                auto last = --itr;
                stmt_list.erase(++itr);
                itr = last;
            }
            else if (cond == 0) // Repeat condition always false
                std::cerr << "Warning: Dead loop detected" << std::endl; // Warn
        }
    }
}

void ASTopt::operator()(std::shared_ptr<BaseRoutineNode> prog)
{
    for (auto &routine : prog->header->subroutineList->getChildren())
        this->operator()(cast_node<BaseRoutineNode>(routine));
    opt(prog->body);
}