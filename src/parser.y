%skeleton "lalr1.cc"
%require "3.0"
%debug

// 声明命名空间与类名，结合使用 spc::parser::
%define api.namespace {spc}
// 使得类型与token定义可以使用各种复杂的结构与类型
%define api.value.type variant
%locations
// 开启断言功能
%define parse.assert

// 生成各种头文件
%defines
// 导入必要的头文件，定义命名空间
%code requires {
    #include <iostream>
    #include <memory>
    #include <string>
    #include <stdexcept>
    #include "utils/ast.hpp"

    using namespace std;
    namespace spc {}
    using namespace spc;
    
    extern std::shared_ptr<ProgramNode> program;
    extern int line_no;
}


// 导入scanner和driver操作
%code {
    int yylex(spc::parser::semantic_type* lval, spc::parser::location_type* loc);
}

%locations
// 详细显示错误信息
%define parse.error verbose

// 定义terminal：token
%token PROGRAM ID CONST ARRAY VAR FUNCTION PROCEDURE PBEGIN END TYPE RECORD
%token INTEGER REAL CHAR STRING
%token SYS_CON SYS_FUNCT SYS_PROC SYS_TYPE STR_TYPE
%token IF THEN ELSE REPEAT UNTIL WHILE DO FOR TO DOWNTO CASE OF GOTO
%token ASSIGN EQUAL UNEQUAL LE LT GE GT
%token PLUS MINUS MUL DIV MOD TRUEDIV AND OR XOR NOT
%token DOT DOTDOT SEMI LP RP LB RB COMMA COLON

%type <std::shared_ptr<IntegerNode>> INTEGER
%type <std::shared_ptr<RealNode>> REAL
%type <std::shared_ptr<CharNode>> CHAR
%type <std::shared_ptr<StringNode>> STRING
%type <std::shared_ptr<IdentifierNode>> ID
%type <std::shared_ptr<SimpleTypeNode>> SYS_TYPE
%type <spc::SysFunc> SYS_PROC SYS_FUNCT
%type <spc::ForDirection> TO DOWNTO
%type <std::shared_ptr<ConstValueNode>> SYS_CON

%type <std::shared_ptr<ProgramNode>> program
%type <std::shared_ptr<RoutineHeadNode>> routine_head
%type <std::shared_ptr<RoutineList>> routine_part 
%type <std::shared_ptr<RoutineNode>> function_decl procedure_decl
%type <std::shared_ptr<ConstDeclList>> const_part const_expr_list
%type <std::shared_ptr<TypeDeclList>> type_part type_decl_list
%type <std::shared_ptr<VarDeclList>> var_part var_decl_list var_decl
%type <std::shared_ptr<ConstValueNode>> const_value
%type <std::shared_ptr<TypeNode>> type_decl simple_type_decl
%type <std::shared_ptr<StringTypeNode>> string_type_decl
%type <std::shared_ptr<ArrayTypeNode>> array_type_decl
%type <std::pair<std::shared_ptr<IdentifierList>, std::shared_ptr<TypeNode>>> field_decl
%type <std::shared_ptr<RecordTypeNode>> record_type_decl field_decl_list 
%type <std::pair<std::shared_ptr<ExprNode>, std::shared_ptr<ExprNode>>> array_range
%type <std::shared_ptr<TypeDeclNode>> type_definition 
%type <std::shared_ptr<IdentifierList>> name_list var_para_list
%type <std::shared_ptr<ParamList>> parameters para_decl_list para_type_list
%type <std::shared_ptr<AssignStmtNode>> assign_stmt
%type <std::shared_ptr<ProcStmtNode>> proc_stmt
%type <std::shared_ptr<CompoundStmtNode>> compound_stmt stmt_list stmt else_clause routine_body
%type <std::shared_ptr<IfStmtNode>> if_stmt
%type <std::shared_ptr<RepeatStmtNode>> repeat_stmt
%type <std::shared_ptr<WhileStmtNode>> while_stmt
%type <std::shared_ptr<StmtNode>> goto_stmt
%type <std::shared_ptr<ForStmtNode>> for_stmt
%type <spc::ForDirection> direction
%type <std::shared_ptr<CaseStmtNode>> case_stmt
%type <std::shared_ptr<CaseBranchList>> case_expr_list
%type <std::shared_ptr<CaseBranchNode>> case_expr
%type <std::shared_ptr<ExprNode>> expression expr term factor
%type <std::shared_ptr<ArgList>> args_list

%start program

%%

program: PROGRAM ID SEMI routine_head routine_body DOT{
        program = make_node<ProgramNode>($2, $4, $5);
    }
    ;

routine_head: const_part type_part var_part routine_part {
        $$ = make_node<RoutineHeadNode>($1, $3, $2, $4);
    }
    ;

const_part: CONST const_expr_list { $$=$2; }
    | { $$ = make_node<ConstDeclList>(); }
    ;

const_expr_list: const_expr_list ID EQUAL const_value SEMI  {
        $$ = $1; $$->append(make_node<ConstDeclNode>($2, $4));
    }
    | ID EQUAL const_value SEMI {
        $$ = make_node<ConstDeclList>(make_node<ConstDeclNode>($1, $3));
    }
    ;

const_value: INTEGER {$$ = $1;}
    | REAL    {$$ = $1;}
    | CHAR    {$$ = $1;}
    | STRING  {$$ = $1;}
    | SYS_CON {$$ = $1;}
    ;

type_part: TYPE type_decl_list {$$ = $2;}
    | {$$ = make_node<TypeDeclList>();}
    ;

type_decl_list: type_decl_list type_definition {
        $$ = $1; $$->append($2);
    }
    | type_definition {
        $$ = make_node<TypeDeclList>($1);
    }
    ;

type_definition: ID EQUAL type_decl SEMI {
        $$ = make_node<TypeDeclNode>($1, $3);
    }
    ;

type_decl: simple_type_decl {
        $$ = $1;
    }
    | array_type_decl {$$ = $1;}
    // | string_type_decl {$$ = $1;}
    // | record_type_decl {$$ = $1;}
    ;

simple_type_decl: SYS_TYPE {$$ = $1;}
    | ID {$$ = make_node<AliasTypeNode>($1);}
    | string_type_decl {$$ = $1;}
    | record_type_decl {$$ = $1;}
    ;

array_type_decl: ARRAY LB array_range RB OF type_decl {
        $$ = make_node<ArrayTypeNode>($3.first, $3.second, $6);
    }
    ;

string_type_decl: STR_TYPE {
        $$ = make_node<StringTypeNode>();
    }
    ;

array_range: const_value DOTDOT const_value { 
            // $$ = (make_node<IntegerNode>($1), make_node<IntegerNode>($3)); // 这咋写啊
        if (!is_ptr_of<IntegerNode>($1) || !is_ptr_of<IntegerNode>($3)) throw std::logic_error("\nArray index must be integer!");
        $$ = std::make_pair($1, $3);
    }
    | ID DOTDOT ID { 
        // $$ = (make_node<IntegerNode>($1), make_node<IntegerNode>($3)); //这咋写啊
        $$ = std::make_pair($1, $3);
    }
    ;

record_type_decl: RECORD field_decl_list END {
        $$ = $2;
    }
    ;

field_decl_list: field_decl_list field_decl {
        $$ = $1; $$->merge(make_node<RecordTypeNode>($2.first, $2.second));
    }
    | field_decl {$$ = make_node<RecordTypeNode>($1.first, $1.second);}
    ;

field_decl: name_list COLON type_decl SEMI {
        $$ = std::make_pair($1, $3);
    }
    ;

name_list: name_list COMMA ID {
        $$ = $1; $$->append($3);
    }
    | ID {$$ = make_node<IdentifierList>($1);}
    ;

var_part: VAR var_decl_list {$$ = $2;}
    | {$$ = make_node<VarDeclList>();}
    ;

var_decl_list: var_decl_list var_decl {
        $$ = $1; $$->merge(std::move($2));
    }
    | var_decl {$$ = $1;}
    ;

var_decl: name_list COLON type_decl SEMI {
        $$ = make_node<VarDeclList>();
        for (auto &name : $1->getChildren()) $$->append(make_node<VarDeclNode>(name, $3));
    }
    ;

routine_part: routine_part function_decl { 
        $$ = $1; $$->append($2);
    }
    | routine_part procedure_decl {$$ = $1; $$->append($2);}
    | {$$ = make_node<RoutineList>();}
    ;
// TBD
function_decl: FUNCTION ID parameters COLON simple_type_decl SEMI routine_head routine_body SEMI {
    $$ = make_node<RoutineNode>($2, $7, $8, $3, $5); 
    }
    ;

procedure_decl: PROCEDURE ID parameters SEMI routine_head routine_body SEMI {
        $$ = make_node<RoutineNode>($2, $5, $6, $3, make_node<VoidTypeNode>());
    }
    ;

parameters: LP para_decl_list RP { $$ = $2; }
    | LP RP { $$ = make_node<ParamList>(); }
    | { $$ = make_node<ParamList>(); }
    ;

para_decl_list: para_decl_list SEMI para_type_list {
        $$ = $1; $$->merge(std::move($3));
    }
    | para_type_list {$$ = $1;}
    ;

para_type_list: var_para_list COLON simple_type_decl {
        $$ = make_node<ParamList>();
        for (auto &name : $1->getChildren()) $$->append(make_node<ParamNode>(name, $3));
    }
    ;

var_para_list: VAR name_list {
        $$ = $2;
    }
    | name_list {$$ = $1;}
    ;

routine_body: compound_stmt {
        $$ = $1;
    }
    ;

compound_stmt: PBEGIN stmt_list END {
        $$ = $2;
    }
    ;

stmt_list: stmt_list stmt SEMI {
        $$ = $1; $$->merge(std::move($2));
    }
    | { $$ = make_node<CompoundStmtNode>(); } // y这里没定义
    ;

stmt: assign_stmt {$$ = make_node<CompoundStmtNode>($1);}
    | proc_stmt {$$ = make_node<CompoundStmtNode>($1);}
    | compound_stmt {$$ = $1;}
    | if_stmt {$$ = make_node<CompoundStmtNode>($1);}
    | repeat_stmt {$$ = make_node<CompoundStmtNode>($1);}
    | while_stmt {$$ = make_node<CompoundStmtNode>($1);}
    | for_stmt {$$ = make_node<CompoundStmtNode>($1);}
    | case_stmt {$$ = make_node<CompoundStmtNode>($1);}
    | goto_stmt {$$ = make_node<CompoundStmtNode>($1);}
    ;
// array ref
assign_stmt: ID ASSIGN expression {
        $$ = make_node<AssignStmtNode>($1, $3);
    }
    | ID LB expression RB ASSIGN expression {
        $$ = make_node<AssignStmtNode>(make_node<ArrayRefNode>($1, $3), $6);
    }
    | ID DOT ID ASSIGN expression {
        $$ = make_node<AssignStmtNode>(make_node<RecordRefNode>($1, $3), $5);
    }
    ;
// routine call
proc_stmt: ID {  $$ = make_node<ProcStmtNode>(make_node<CustomProcNode>($1)); }
    | ID LP RP {  $$ = make_node<ProcStmtNode>(make_node<CustomProcNode>($1)); }
    | ID LP args_list RP
        { $$ = make_node<ProcStmtNode>(make_node<CustomProcNode>($1, $3)); }
    | SYS_PROC LP RP
        { $$ = make_node<ProcStmtNode>(make_node<SysProcNode>($1)); }
    | SYS_PROC
        { $$ = make_node<ProcStmtNode>(make_node<SysProcNode>($1)); }
    | SYS_PROC LP args_list RP
        { $$ = make_node<ProcStmtNode>(make_node<SysProcNode>($1, $3)); };
//  | READ LP factor RP
//      { printf("proc_stmt: READ LP factor RP\n"); }
    

repeat_stmt: REPEAT stmt_list UNTIL expression {
        $$ = make_node<RepeatStmtNode>($4, $2); // $$->append($2);
    }
    ;

while_stmt: WHILE expression DO stmt {
        $$ = make_node<WhileStmtNode>($2, $4);
    }
    ;
// direction
for_stmt: FOR ID ASSIGN expression direction expression DO stmt {
        $$ = make_node<ForStmtNode>($5, $2, $4, $6, $8);
    }
    ;

direction: TO {$$ = ForDirection::To; }
    | DOWNTO {$$ = ForDirection::Downto;}
    ;

if_stmt: IF expression THEN stmt else_clause {
        $$ = make_node<IfStmtNode>($2, $4, $5);
    }
    ;

else_clause: ELSE stmt { $$ = $2; }
    | { $$ = nullptr; }// $$ = make_node<CompoundStmtNode>();
    ;

case_stmt: CASE expression OF case_expr_list END {
        $$ = make_node<CaseStmtNode>($2, std::move($4));
    }
    ;

case_expr_list: case_expr_list case_expr { 
        $$ = $1; $$->append($2); 
    }
    | case_expr { $$ = make_node<CaseBranchList>($1); }
    ;

case_expr: const_value COLON stmt SEMI {
        if (!is_ptr_of<IntegerNode>($1) && !is_ptr_of<CharNode>($1))
            throw std::logic_error("\nCase branch must be integer type!");
        $$ = make_node<CaseBranchNode>($1, $3); 
    }
    | ID COLON stmt SEMI { $$ = make_node<CaseBranchNode>($1, $3); }
    ;
// 不会真有人写goto吧
goto_stmt: GOTO INTEGER {
        throw std::logic_error("\nGoto not supported yet");
    }
    ;

expression: expression GE expr { $$ = make_node<BinaryExprNode>(BinaryOp::Geq, $1, $3); }
    | expression GT expr { $$ = make_node<BinaryExprNode>(BinaryOp::Gt, $1, $3); }
    | expression LE expr { $$ = make_node<BinaryExprNode>(BinaryOp::Leq, $1, $3); }
    | expression LT expr { $$ = make_node<BinaryExprNode>(BinaryOp::Lt, $1, $3); }
    | expression EQUAL expr { $$ = make_node<BinaryExprNode>(BinaryOp::Eq, $1, $3); }
    | expression UNEQUAL expr { $$ = make_node<BinaryExprNode>(BinaryOp::Neq, $1, $3); }
    | expr { $$ = $1; }
    ;

expr: expr PLUS term { $$ = make_node<BinaryExprNode>(BinaryOp::Plus, $1, $3); }
    | expr MINUS term { $$ = make_node<BinaryExprNode>(BinaryOp::Minus, $1, $3); }
    | expr OR term { $$ = make_node<BinaryExprNode>(BinaryOp::Or, $1, $3); }
    | expr XOR term { $$ = make_node<BinaryExprNode>(BinaryOp::Xor, $1, $3); }
    | term { $$ = $1; }
    ;

term: term MUL factor { $$ = make_node<BinaryExprNode>(BinaryOp::Mul, $1, $3); }
    | term DIV factor { $$ = make_node<BinaryExprNode>(BinaryOp::Div, $1, $3); }
    | term MOD factor { $$ = make_node<BinaryExprNode>(BinaryOp::Mod, $1, $3); }
    | term AND factor { $$ = make_node<BinaryExprNode>(BinaryOp::And, $1, $3); }
    | term TRUEDIV factor { $$ = make_node<BinaryExprNode>(BinaryOp::Truediv, $1, $3);  }
    | factor { $$ = $1; }
    ;
// call node & ref node
factor: ID { $$ = $1; }
    | ID LP args_list RP
        { $$ = make_node<CustomProcNode>($1, $3); }
    | SYS_FUNCT LP args_list RP
        { $$ = make_node<SysProcNode>($1, $3); }
    | const_value { $$ = $1; }
    | LP expression RP { $$ = $2; }
    | NOT factor
        { $$ = make_node<BinaryExprNode>(BinaryOp::Xor, make_node<BooleanNode>(true), $2); }
    | MINUS factor
        { $$ = make_node<BinaryExprNode>(BinaryOp::Minus, make_node<IntegerNode>(0), $2); }
    | PLUS factor { $$ = $2; }
    | ID LB expression RB
        { $$ = make_node<ArrayRefNode>($1, $3); }
    | ID DOT ID
        { $$ = make_node<RecordRefNode>($1, $3); }
    ;

args_list: args_list COMMA expression {
        $$ = $1; $$->append($3);
    }
    | expression {
        $$ = make_node<ArgList>($1);// $$->add_child($1);
    }
    ;

%%

void spc::parser::error(const spc::parser::location_type &loc, const std::string& msg) {
    std::cerr << std::endl << "Parser: Error at " << loc << ":" << std::endl;
    std::string msg2 = msg;
    msg2[0] = toupper(msg2[0]);
    throw std::logic_error(msg2);
}
