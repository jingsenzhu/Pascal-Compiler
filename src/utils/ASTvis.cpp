#include "ASTvis.hpp"

#include <fstream>
#include <iostream>
#include <string>

using namespace spc;

void spc::ASTvis::travAST(const std::shared_ptr<ProgramNode>& prog)
{

    of << texHeader;
    travProgram(prog);
    of << texTail;

    // std::cout << "debug info:\n" << prog->getName() << std::endl;
    // std::cout << "\n\n>>>>>>>>>>>>>>>==========  AST over!==========<<<<<<<<<<<<<<<" << std::endl;

    return;
}

int spc::ASTvis::travProgram(const std::shared_ptr<spc::ProgramNode>& prog)
{
    of << "\\node {Program: " << prog->getName() << "}\n";
    return travRoutineBody(spc::cast_node<spc::BaseRoutineNode>(prog));
}

int spc::ASTvis::travRoutineBody(const std::shared_ptr<spc::BaseRoutineNode>& prog)
{
    int tmp = 0, lines = 6;
    of << "child { node {CONST}";
    tmp = travCONST(prog->header->constList);
    of << "}\n";
    for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp;
    // std::cout << "debug info: CONST part over" << std::endl;

    of << "child { node {TYPE}";
    tmp = travTYPE(prog->header->typeList);
    of << "}\n";
    for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp;
    // std::cout << "debug info: TYPE part over" << std::endl;

    of << "child { node {VAR}";
    tmp = travVAR(prog->header->varList);
    of << "}\n";
    for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp;
    // std::cout << "debug info: VAR part over" << std::endl;

    of << "child { node {PROC or FUNC}";
    tmp = travSubprocList(prog->header->subroutineList);
    of << "}\n";
    for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp;
    // std::cout << "debug info: PROC part over" << std::endl;

    of << "child { node {STMT}";
    tmp = travCompound(prog->body);
    of << "}\n";
    for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp;
    // std::cout << "debug info: BODY part over" << std::endl;

    return lines;
}

int spc::ASTvis::travCONST(const std::shared_ptr<spc::ConstDeclList>& const_declListAST)
{
    std::list<std::shared_ptr<ConstDeclNode>>& constList(const_declListAST->getChildren());
    int lines = constList.size();

    for (auto &p : constList) {
        of << "child { node {";
        of << p->name->name << " : ";
        switch (p->val->type) {
            case spc::Type::Void    : of << "VOID"   ; break;
            case spc::Type::Array   : of << "ARRAY"  ; break;
            case spc::Type::Record  : of << "RECORD" ; break;
            case spc::Type::Bool : of << "BOOLEAN"; break;
            case spc::Type::Int : of << "INTEGER"; break;
            case spc::Type::Long : of << "LONG"; break;
            case spc::Type::Real    : of << "REAL"   ; break;
            case spc::Type::String  : of << "STRING" ; break;
            default : of << "ERROR"; break;
        }
        of << "}}\n";
    }
    return lines;
}

int spc::ASTvis::travTYPE(const std::shared_ptr<spc::TypeDeclList>& type_declListAST)
{
    std::list<std::shared_ptr<TypeDeclNode>>& typeList(type_declListAST->getChildren());
    int lines = typeList.size();

    for (auto &p : typeList) {
        of << "child { node {";
        of << p->name->name << " : ";
        switch (p->type->type) {
            case spc::Type::Void    : of << "VOID"   ; break;
            case spc::Type::Array   : of << "ARRAY"  ; break;
            case spc::Type::Record  : of << "RECORD" ; break;
            case spc::Type::Bool : of << "BOOLEAN"; break;
            case spc::Type::Int : of << "INTEGER"; break;
            case spc::Type::Long : of << "LONG"; break;
            case spc::Type::Real    : of << "REAL"   ; break;
            case spc::Type::String  : of << "STRING" ; break;
            default : of << "ERROR"; break;
        }
        of << "}}\n";
    }
    return lines;
}
int spc::ASTvis::travVAR(const std::shared_ptr<spc::VarDeclList>& var_declListAST)
{
    std::list<std::shared_ptr<VarDeclNode>>& varList(var_declListAST->getChildren());
    int lines = varList.size();

    for (auto &p : varList) {
        of << "child { node {";
        of << p->name->name << " : ";
        switch (p->type->type) {
            case spc::Type::Void    : of << "VOID"   ; break;
            case spc::Type::Array   : of << "ARRAY"  ; break;
            case spc::Type::Record  : of << "RECORD" ; break;
            case spc::Type::Bool : of << "BOOLEAN"; break;
            case spc::Type::Int : of << "INTEGER"; break;
            case spc::Type::Long : of << "LONG"; break;
            case spc::Type::Real    : of << "REAL"   ; break;
            case spc::Type::String  : of << "STRING" ; break;
            default : of << "ERROR"; break;
        }
        of << "}}\n";
    }
    return lines;
}

int spc::ASTvis::travSubprocList(const std::shared_ptr<spc::RoutineList>& subProc_declListAST)
{
    std::list<std::shared_ptr<spc::RoutineNode>>& progList(subProc_declListAST->getChildren());
    int tmp = 0, lines = progList.size();

    for (auto &p : progList) {
        tmp = travSubproc(p);
        for (int i=0; i<tmp; ++i) of << texNone;
        lines += tmp;
    }
    return lines;
}

int spc::ASTvis::travSubproc(const std::shared_ptr<spc::RoutineNode>& subProc_AST)
{
    int lines = 0;
    of << "child { node {";
    if (subProc_AST->retType->type == spc::Type::Void )
        of << "PROCEDURE: " << subProc_AST->getName();
    else
    {
        of << "FUNCTION: " << subProc_AST->getName();
        of << "$ ---- $RET$-$TYPE: ";
        switch (subProc_AST->retType->type) {
            case spc::Type::Void    : of << "VOID"   ; break;
            case spc::Type::Array   : of << "ARRAY"  ; break;
            case spc::Type::Record  : of << "RECORD" ; break;
            case spc::Type::Bool : of << "BOOLEAN"; break;
            case spc::Type::Int : of << "INTEGER"; break;
            case spc::Type::Long : of << "LONG"; break;
            case spc::Type::Real    : of << "REAL"   ; break;
            case spc::Type::String  : of << "STRING" ; break;
            default : of << "ERROR"; break;
        }
    }

    std::list<std::shared_ptr<ParamNode>>& paramAsts
            = subProc_AST->params->getChildren();
    {
        of << "$ ---- $PARAMS: ";
        for (auto &p : paramAsts) {
            of << p->name->name << " $-$ ";
            switch (p->type->type) {
                case spc::Type::Void    : of << "VOID"   ; break;
                case spc::Type::Array   : of << "ARRAY"  ; break;
                case spc::Type::Record  : of << "RECORD" ; break;
                case spc::Type::Bool : of << "BOOLEAN"; break;
                case spc::Type::Int : of << "INTEGER"; break;
                case spc::Type::Long : of << "LONG"; break;
                case spc::Type::Real    : of << "REAL"   ; break;
                case spc::Type::String  : of << "STRING" ; break;
                default : of << "ERROR"; break;
            }
        }
    }
    of << "}";
    lines = travRoutineBody(spc::cast_node<spc::BaseRoutineNode>(subProc_AST));
    of << "}\n";
    return lines;
}

int spc::ASTvis::travCompound(const std::shared_ptr<spc::CompoundStmtNode>& compound_declListAST)
{
    if (compound_declListAST == nullptr) return 0;
    std::list<std::shared_ptr<spc::StmtNode>>& stmtList(compound_declListAST->getChildren());
    int tmp = 0, lines = stmtList.size();
    for (auto &p : stmtList) {
        tmp = 0;
        if (spc::is_ptr_of<spc::IfStmtNode>(p))
        {
            tmp += travStmt(spc::cast_node<spc::IfStmtNode>(p));
            // std::cout << "debug info: IF over" << std::endl;
        }
        else if (spc::is_ptr_of<spc::WhileStmtNode>(p))
        {
            tmp += travStmt(spc::cast_node<spc::WhileStmtNode>(p));
            // std::cout << "debug info: WHILE over" << std::endl;
        }
        else if (spc::is_ptr_of<spc::ForStmtNode>(p))
        {
            tmp += travStmt(spc::cast_node<spc::ForStmtNode>(p));
            // std::cout << "debug info: FOR over" << std::endl;
        }
        else if (spc::is_ptr_of<spc::RepeatStmtNode>(p))
        {
            tmp += travStmt(spc::cast_node<spc::RepeatStmtNode>(p));
            // std::cout << "debug info: REPEAT over" << std::endl;
        }
        else if (spc::is_ptr_of<spc::ProcStmtNode>(p))
        {
            tmp += travStmt(spc::cast_node<spc::ProcStmtNode>(p));
            // std::cout << "debug info: PROC over" << std::endl;
        }
        else if (spc::is_ptr_of<spc::AssignStmtNode>(p))
        {
            tmp += travStmt(spc::cast_node<spc::AssignStmtNode>(p));
            // std::cout << "debug info: ASSIGN over" << std::endl;
        }
        else if (spc::is_ptr_of<spc::CaseStmtNode>(p))
        {
            tmp += travStmt(spc::cast_node<spc::CaseStmtNode>(p));
            // std::cout << "debug info: CASE over" << std::endl;
        }
        lines += tmp;
    }
    return lines;
}

int spc::ASTvis::travStmt(const std::shared_ptr<spc::CaseStmtNode>&p_stmp)
{
    if (p_stmp == nullptr) return 0;
    std::list<std::shared_ptr<spc::CaseBranchNode>>& stmtList(p_stmp->branches);
    int tmp = 0, lines = stmtList.size();
    of << "child { node {CASE Statment case expr}\n";
    tmp = travExpr(p_stmp->expr);
    of << "}\n";
    for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp; tmp = 0;
    of << "child { node {CASE Statement case branches}\n";
    int sublines = stmtList.size();
    for (auto &p : stmtList)
    {
        std::string br;
        if (spc::is_ptr_of<spc::IntegerNode>(p->branch))
            br = std::to_string(spc::cast_node<IntegerNode>(p->branch)->val);
        else if (spc::is_ptr_of<spc::IdentifierNode>(p->branch))
            br = spc::cast_node<spc::IdentifierNode>(p->branch)->name;
        of << "child { node {Case " + br + "}\n";
        tmp = travCompound(p->stmt);
        of << "}\n";
        for (int i=0; i<tmp; ++i) of << texNone;
        lines += tmp; sublines += tmp; tmp = 0;
    }
    of << "}\n";
    for (int i=0; i<sublines; ++i) of << texNone;
    return lines;
}

int spc::ASTvis::travStmt(const std::shared_ptr<spc::StmtNode>&p_stmp)
{
    if (p_stmp == nullptr) return 0;
    of << "child { node {Base Statment}}\n";
    return 0;
}
// * done
int spc::ASTvis::travStmt(const std::shared_ptr<spc::IfStmtNode>&p_stmp)
{
    if (p_stmp == nullptr) return 0;
    int tmp = 0, lines = 3;
    of << "child { node {IF Statment if expr}\n";
    tmp = travExpr(p_stmp->expr);
    of << "}\n";
    for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp; tmp = 0;
    // std::cout << "debug info: IF expr over" << std::endl;
    of << "child { node {IF Statment if stmt}\n";
    tmp = travCompound(p_stmp->if_stmt);
    of << "}\n";
    for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp; tmp = 0;
    // std::cout << "debug info: IF part over" << std::endl;
    of << "child { node {IF Statment else stmt}\n";
    tmp = travCompound(p_stmp->else_stmt);
    of << "}\n";
    for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp; tmp = 0;
    // std::cout << "debug info: ELSE part over" << std::endl;

    return lines;
}
int spc::ASTvis::travStmt(const std::shared_ptr<spc::WhileStmtNode>&p_stmp)
{
    if (p_stmp == nullptr) return 0;
    int tmp = 0, lines = 0;
    of << "child { node {WHILE Expr}";
    tmp = travExpr(p_stmp->expr);
    of << "}\n";
    for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp; tmp = 0;
    of << "child { node {WHILE Statment}\n";
    tmp = travCompound(p_stmp->stmt);
    of << "}\n";
    for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp; tmp = 0;
    return lines;
}
int spc::ASTvis::travStmt(const std::shared_ptr<spc::ForStmtNode>&p_stmp)
{
    if (p_stmp == nullptr) return 0;
    int tmp = 0, lines = 0;
    of << "child { node {FOR Expr: ";
    of << p_stmp->id->name << " }\n ";

    tmp += travExpr(p_stmp->init_val);
    // of << texNone;
    tmp += travExpr(p_stmp->end_val);
    of << "}\n";
    for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp; tmp = 0;

    of << "child { node {FOR Statment}\n";
    tmp = travCompound(p_stmp->stmt);
    of << "}\n";
    for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp;

    return lines;
}
int spc::ASTvis::travStmt(const std::shared_ptr<spc::RepeatStmtNode>&p_stmp)
{
    if (p_stmp == nullptr) return 0;
    int tmp = 0, lines = 0;
    of << "child { node {REPEAT Expr}";
    tmp = travExpr(p_stmp->expr);
    of << "}\n";
    for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp; tmp = 0;
    of << "child { node {REPEAT Statment}\n";
    tmp = travCompound(p_stmp->stmt);
    of << "}\n";
    for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp;
    // of << "}\n";
    return lines;
}
int spc::ASTvis::travStmt(const std::shared_ptr<spc::ProcStmtNode>&p_stmp)
{
    if (p_stmp == nullptr) return 0;
    int tmp = 0, lines = 0;
    of << "child { node {CALL Statment";
    // tmp = travExpr(p_stmp->expr);
    // for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp;
    of << "}\n}\n";
    return lines;
}
int spc::ASTvis::travStmt(const std::shared_ptr<spc::AssignStmtNode>&p_stmp)
{
    if (p_stmp == nullptr) return 0;
    int tmp = 0, lines = 1;
    of << "child { node {ASSIGN Statment}";
    tmp += travExpr(p_stmp->lhs);
    tmp += travExpr(p_stmp->rhs);
    of << "}\n";
    for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp;
    // of << "}\n}\n";
    return lines;
}

int spc::ASTvis::travExpr(const std::shared_ptr<ExprNode>& expr)
{
    int tmp = 0, lines = 0;
    if (spc::is_ptr_of<spc::BinaryExprNode>(expr))
        tmp += travExpr(spc::cast_node<spc::BinaryExprNode>(expr));
    // tmp += travExpr(spc::cast_node<spc::UnaryExprNode>(expr));
    else if (spc::is_ptr_of<spc::ArrayRefNode>(expr))
        tmp += travExpr(spc::cast_node<spc::ArrayRefNode>(expr));
    else if (spc::is_ptr_of<spc::RecordRefNode>(expr))
        tmp += travExpr(spc::cast_node<spc::RecordRefNode>(expr));
    else if (spc::is_ptr_of<spc::CustomProcNode>(expr))
        tmp += travExpr(spc::cast_node<spc::CustomProcNode>(expr));
    else if (spc::is_ptr_of<spc::SysProcNode>(expr))
        tmp += travExpr(spc::cast_node<spc::SysProcNode>(expr));
    for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp;
    return lines;
}

int spc::ASTvis::travExpr(const std::shared_ptr<BinaryExprNode>& expr)
{
    if (expr == nullptr) return 0;
    int tmp = 0, lines = 1;

    of << "child { node {BINARY: ";
    switch (expr->op)
    {
        case spc::BinaryOp::Eq: of << "==";break;
        case spc::BinaryOp::Neq: of << "!=";break;
        case spc::BinaryOp::Leq: of << "<=";break;
        case spc::BinaryOp::Geq: of << ">=";break;
        case spc::BinaryOp::Lt: of << "<";break;
        case spc::BinaryOp::Gt: of << ">";break;
        case spc::BinaryOp::Plus: of << "+";break;
        case spc::BinaryOp::Minus: of << "-";break;
        case spc::BinaryOp::Truediv: of << "/";break;
        case spc::BinaryOp::Div: of << "//";break;
        case spc::BinaryOp::Mod: of << "\\%";break;
        case spc::BinaryOp::Mul: of << "*";break;
        case spc::BinaryOp::Or:  of << "|";break;
        case spc::BinaryOp::And: of << "&";break;
        case spc::BinaryOp::Xor: of << "\\^";break;
        default: of << "ERROR"; break;
    }
    of << "}\n";
    tmp += travExpr(expr->lhs);
    // for (int i=0; i<tmp; ++i) of << texNone;
    // lines += tmp; tmp = 0;
    tmp += travExpr(expr->rhs);
    for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp;
    of << "}\n";

    return lines;
}
// int spc::ASTvis::travExpr(const std::shared_ptr<spc::UnaryExprNode>& expr)
// {
//     if (expr == nullptr) return 0;
//     int tmp = 0, lines = 1;

//     of << "child { node {UNARY: ";
//     switch (expr->op)
//     {
//         case spc::UnaryOp::Neg: of << "-";break;
//         case spc::UnaryOp::Pos: of << "+";break;
//         case spc::UnaryOp::Not: of << "!";break;
//         default: of << "ERROR";break;
//     }
//     of << "}\n";
//     of << "}\n";
//     return lines;
// }
int spc::ASTvis::travExpr(const std::shared_ptr<spc::IdentifierNode>& expr)
{
    if (expr == nullptr) return 0;
    int tmp = 0, lines = 0;
    of << "child { node {ID " << expr->name;
    // tmp = travExpr(p_stmp->expr);
    for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp;
    of << "}\n}\n";
    return lines;
}
int spc::ASTvis::travExpr(const std::shared_ptr<spc::ArrayRefNode>& expr)
{
    if (expr == nullptr) return 0;
    int tmp = 0, lines = 0;
    of << "child { node {ARRAY REFERENCE";
    // tmp = travExpr(p_stmp->expr);
    for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp;
    of << "}\n}\n";
    return lines;
}
int spc::ASTvis::travExpr(const std::shared_ptr<spc::RecordRefNode>& expr)
{
    if (expr == nullptr) return 0;
    int tmp = 0, lines = 0;
    of << "child { node {RECORD REFERENCE";
    // tmp = travExpr(p_stmp->expr);
    for (int i=0; i<tmp; ++i) of << texNone;
    lines += tmp;
    of << "}\n}\n";
    return lines;
}
int spc::ASTvis::travExpr(const std::shared_ptr<spc::ProcNode>& expr)
{
    if (expr == nullptr) return 0;
    int tmp = 0, lines = 1;
    return lines;
}
int spc::ASTvis::travExpr(const std::shared_ptr<spc::CustomProcNode>& expr)
{
    if (expr == nullptr) return 0;
    int tmp = 0, lines = 0;

    of << "child { node {CustomFunc: ";
    of << expr->name->name;
    of << "}\n";
    of << "}\n";
    return lines;
}
int spc::ASTvis::travExpr(const std::shared_ptr<spc::SysProcNode>& expr)
{
    if (expr == nullptr) return 0;
    int tmp = 0, lines = 0;

    of << "child { node {SysFunc: ";
    switch (expr->name)
    {
        case spc::SysFunc::Read:   of << "read()"; break;
        case spc::SysFunc::Write:  of << "write()"; break;
        case spc::SysFunc::Writeln:of << "writeln()";  break;
        case spc::SysFunc::Abs: of << "abs()"; break;
        case spc::SysFunc::Chr: of << "chr()"; break;
        case spc::SysFunc::Odd: of << "odd()"; break;
        case spc::SysFunc::Ord: of << "ord()"; break;
        case spc::SysFunc::Pred: of << "pred()"; break;
        case spc::SysFunc::Sqr: of << "sqr()"; break;
        case spc::SysFunc::Sqrt: of << "sqrt()"; break;
        case spc::SysFunc::Succ: of << "succ()"; break;
    }
    of << "}\n";
    of << "}\n";
    return lines;
}
