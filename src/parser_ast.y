%skeleton "lalr1.cc"
%require "3.0"
%debug

//声明命名空间与类名，结合使用 spc::Parser::
%define api.IDspace {spc}
%define parser_class_ID {Parser}
//使得类型与token定义可以使用各种复杂的结构与类型
%define api.value.type variant
//开启断言功能
%define parse.assert

//生成各种头文件
%defines
//导入必要的头文件，定义命名空间
%code requires {
    #include <iostream>
    #include <memory>
    #include <string>
    #include <stdexcept>

    IDspace spc {
        class Driver;
        class Scanner;
    }
    using IDspace spc;
    
}

//定义参数传递
%parse-param {Scanner& scanner}
%parse-param {Driver& driver}

//导入scanner和driver操作
%code {
    #include "Driver.hpp"
    #include "Scanner.hpp"
    #undef yylex
    #define yylex scanner.yylex
}

%locations
//详细显示错误信息
%define parse.error verbose

//定义terminal：token
%token PROGRAM ID CONST ARRAY VAR FUNCTION PROCEDURE PBEGIN END TYPE RECORD
%token INTEGER REAL CHAR STRING
%token SYS_CON SYS_FUNCT SYS_PROC SYS_TYPE READ
%token IF THEN ELSE REPEAT UNTIL WHILE DO FOR TO DOWNTO CASE OF GOTO
%token ASSIGN EQUAL UNEQUAL LE LT GE GT
%token PLUS MINUS MUL DIV MOD TRUEDIV AND OR XOR NOT
%token DOT DOTDOT SEMI LP RP LB RB COMMA COLON

%type <std::string> program program_head
%type <std::string> routine routine_head routine_body
%type <std::string> const_part type_part var_part routine_part
%type <std::string> const_expr_list const_value
%type <std::string> type_decl_list type_decl type_definition
%type <std::string> simple_type_decl
%type <std::string> array_type_decl array_range
%type <std::string> record_type_decl
%type <std::string> field_decl field_decl_list name_list
%type <std::string> var_para_list var_decl_list var_decl
%type <std::string> function_decl function_head
%type <std::string> procedure_head procedure_decl
%type <std::string> parameters para_decl_list para_type_list
%type <std::string> stmt_list stmt 
%type <std::string> assign_stmt proc_stmt compound_stmt 
%type <std::string> if_stmt else_clause
%type <std::string> repeat_stmt while_stmt goto_stmt
%type <std::string> for_stmt direction 
%type <std::string> case_stmt case_expr_list case_expr
%type <std::string> expression expr term factor
%type <std::string> args_list

//不允许出现多个else
%nonassoc else_clause

%start program

%%

export: program {
        program = $1;
    }
    ;

program: program_head routine DOT{
        $$ = make_node<ProgramNode>($2);
    }
    ;

program_head: PROGRAM ID SEMI{
        printf("program_head: PROGRAM ID SEMI\n");
    }
    ;
//TBD
routine: routine_head routine_body {
        $$ = 
    }
    ;

routine_head: const_part type_part var_part routine_part {
        $$ = make_node<RoutineList>($1, $2, $3, $4);//
    }
    ;

const_part: CONST const_expr_list {$$=$2}
    | {$$ = make_node<ConstDeclList>();//}
    ;

const_expr_list: const_expr_list ID EQUAL const_value SEMI  {
        $$ = $1; $$->add_child(make_node<ConstDeclNode>($2, $4));
    }
    | ID EQUAL const_value SEMI {
        $$ = make_node<ConstDeclList>(); $$->add_child(make_node<ConstDeclNode>($1, $3));
    }
    ;

const_value: INTEGER {$$ = $1;}
    | REAL    {$$ = $1;}
    | CHAR    {$$ = $1;}
    | STRING  {$$ = $1;}
    | SYS_CON {$$ = $1;}
    ;

type_part: TYPE type_decl_list {$$ = $2}
    | {$$ = make_node<TypeDeclList>();}
    ;

type_decl_list: type_decl_list type_definition {
        $$ = $1; $$->add_child($2);
    }
    | type_definition {
        $$ = make_node<TypeDeclList>();
        $$ ->add_child($1);
    }
    ;

type_definition: ID EQUAL type_decl SEMI {
        $$ = make_node<TypeDeclNode>($1, $3);//
    }
    ;

type_decl: simple_type_decl{
        $$ = $1;
    }
    | array_type_decl {$$ = $1;}
    | record_type_decl {$$ = $1;}
    ;

simple_type_decl: SYS_TYPE {$$ = $1;}
    | ID {$$ = make_node<AliasTypeNode>($1);}
    | LP name_list RP {$$ = make_node<SetTypeNode>(); $$->append($2);//lift}
    ;

array_type_decl: ARRAY LB array_range RB OF type_decl {
        $$ = make_node<ArrayTypeNode>($3, $6);
    }
    ;

array_range: const_value DOTDOT const_value
        { $$ = (make_node<IntegerNode>($1), make_node<IntegerNode>($3)); //这咋写啊}
    | ID DOTDOT ID
        { $$ = (make_node<IntegerNode>($1), make_node<IntegerNode>($3)); //这咋写啊}
    ;

record_type_decl: RECORD field_decl_list END {
        $$ = $2;
    }
    ;

field_decl_list: field_decl_list field_decl {
        $$ = $1; $$->append($2);
    }
    | field_decl {$$ = $1;}
    ;

field_decl: name_list COLON type_decl SEMI {
        //record
    }
    ;

name_list: name_list COMMA ID {
        $$ = $1; $$->append($3);
    }
    | ID {$$ = make_node<DeclNode>(); $$->append($1);//y的name node没东西啊}
    ;

var_part: VAR var_decl_list {$$ = $2;}
    | {$$ = make_node<VarDeclList>();}
    ;

var_decl_list: var_decl_list var_decl {
        $$ = $1; $$->append($2); //lift
    }
    | var_decl {$$ = $1;}
    ;

var_decl: name_list COLON type_decl SEMI {
        $$ = make_node<VarDeclList>();
        for (auto name : $1->children()) $$->add_child(make_node<VarDeclNode>(name, $3));
    }
    ;

routine_part: routine_part function_decl { 
        $$ = $1; $$->append($2);
    }
    | routine_part procedure_decl {$$ = $1; $$->append($2);}
    | {$$ = make_node<RoutineList>();}
    ;
//TBD
function_decl: function_head SEMI routine_head routine_body SEMI
        {$$ = make_node<RoutineNode>($1, $3); $$->append($4); }
    ;

function_head: FUNCTION ID parameters COLON simple_type_decl 
        { $$ = make_node<RoutineHeadNode>($2, $3, $5); $$->append($4); }
    ;

procedure_decl: procedure_head SEMI routine_head routine_body SEMI {
        $$ = make_node<RoutineNode>($1, $3); $$->append($4);
    }
    ;

procedure_head: PROCEDURE ID parameters {
        $$ = make_node<RoutineHeadNode>($2, $3, make_node<SimpleTypeNode>(Type::VOID)); $$->append($4);
    }
    ;

parameters: LP para_decl_list RP { $$ = $2; }
    | LP RP { $$ = make_node<ParamList>(); }
    | { $$ = make_node<ParamList>(); }
    ;

para_decl_list: para_decl_list SEMI para_type_list {
        $$ = $1; $$->append($3);
    }
    | para_type_list {$$ = $1;}
    ;

para_type_list: var_para_list COLON simple_type_decl {
        $$ = make_node<ParamList>();
          for (auto name : $1->children()) $$->add_child(make_node<ParamNode>(name, $3));
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
        $$ = make_node<CompoundStmtNode>(); $$->append($2);
    }
    ;

stmt_list: stmt_list stmt SEMI {
        $$ = $1; $$->add_child($2);
    }
    | {$$ = make_node<CompoundStmtNode>();//y这里没定义}
    ;

stmt: assign_stmt {$$ = $1;}
    | proc_stmt {$$ = $1;}
    | compound_stmt {$$ = $1;}
    | if_stmt {$$ = $1;}
    | repeat_stmt {$$ = $1;}
    | while_stmt {$$ = $1;}
    | for_stmt {$$ = $1;}
    | case_stmt {$$ = $1;}
    | goto_stmt {$$ = $1;}
    ;
//array ref
assign_stmt: ID ASSIGN expression {
        $$ = make_node<AssignStmtNode>($1, $3);
    }
    | ID LB expression RB ASSIGN expression {
        $$ = make_node<AssignStmtNode>(make_node<ArrayRefNode>($1, $3), $5);
    }
    | ID DOT ID ASSIGN expression {
        printf("stmt_list: ID DOT ID ASSIGN expression\n");
    }
    ;
//routine call
proc_stmt: ID {  $$ = make_node<ProcStmtNode>(make_node<RoutineCallNode>($1)); }
    | ID LP args_list RP
        { printf("proc_stmt: ID LP args_list RP\n"); }
    | SYS_PROC
        { printf("proc_stmt: SYS_PROC LP RP\n"); }
    | SYS_PROC LP args_list RP
        { printf("proc_stmt: SYS_PROC LP args_list RP\n"); }
    | READ LP factor RP
        { printf("proc_stmt: READ LP factor RP\n"); }
    ;

repeat_stmt: REPEAT stmt_list UNTIL expression {
        $$ = make_node<RepeatStmtNode>($4); $$->append($2);
    }
    ;

while_stmt: WHILE expression DO stmt {
        $$ = make_node<WhileStmtNode>($2, $4);
    }
    ;
//direction
for_stmt: FOR ID ASSIGN expression direction expression DO stmt {
        $$ = make_node<ForStmtNode>($5, $2, $4, $6, $8);
    }
    ;

direction: TO {$$ = ForDirection::To }
    | DOWNTO {$$ = ForDirection::Downto}
    ;

if_stmt: IF expression THEN stmt else_clause {
        $$ = make_node<IfStmtNode>($2, $4, $5);
    }
    ;

else_clause: ELSE stmt { $$ = $2; }
    | { $$ = make_node<CompoundStmtNode>(); }
    ;

case_stmt: CASE expression OF case_expr_list END {
        $$ = $4; is_ptr_of<CaseStmtNode>($$)->add_expr($2);//add_expr
    }
    ;

case_expr_list: case_expr_list case_expr { 
        $$ = $1; $$->add_child($2); 
    }
    | case_expr { $$ = make_node<CaseStmtNode>(); $$->add_child($1); }
    ;

case_expr: const_value COLON stmt SEMI { $$ = make_node<CaseBranchNode>($1, $3); }
    | ID COLON stmt SEMI { $$ = make_node<CaseBranchNode>($1, $3); }
    ;
//不会真有人写goto吧
goto_stmt: GOTO INTEGER {
        printf("goto_stmt: GOTO INTEGER\n");
    }
    ;

expression: expression GE expr { $$ = make_node<BinaryExprNode>(BinaryOp::GE, $1, $3); }
    | expression GT expr { $$ = make_node<BinaryExprNode>(BinaryOp::GT, $1, $3); }
    | expression LE expr { $$ = make_node<BinaryExprNode>(BinaryOp::LE, $1, $3); }
    | expression LT expr { $$ = make_node<BinaryExprNode>(BinaryOp::LT, $1, $3); }
    | expression EQUAL expr { $$ = make_node<BinaryExprNode>(BinaryOp::EQUAL, $1, $3); }
    | expression UNEQUAL expr { $$ = make_node<BinaryExprNode>(BinaryOp::UNEQUAL, $1, $3); }
    | expr { $$ = $1 }
    ;

expr: expr PLUS term { $$ = make_node<BinaryExprNode>(BinaryOp::PLUS, $1, $3); }
    | expr MINUS term { $$ = make_node<BinaryExprNode>(BinaryOp::MINUS, $1, $3); }
    | expr OR term { $$ = make_node<BinaryExprNode>(BinaryOp::OR, $1, $3); }
    | expr XOR term { $$ = make_node<BinaryExprNode>(BinaryOp::XOR, $1, $3); }
    | term { $$ = $1; }
    ;

term: term MUL factor { $$ = make_node<BinaryExprNode>(BinaryOp::MUL, $1, $3); }
    | term DIV factor { $$ = make_node<BinaryExprNode>(BinaryOp::DIV, $1, $3); }
    | term MOD factor { $$ = make_node<BinaryExprNode>(BinaryOp::MOD, $1, $3); }
    | term AND factor { $$ = make_node<BinaryExprNode>(BinaryOp::AND, $1, $3); }
    | term TRUEDIV factor { $$ = make_node<BinaryExprNode>(BinaryOp::TRUEDIV, $1, $3);  }
    | factor { $$ = $1; }
    ;
//call node & ref node
factor: ID { $$ = $1; }
    | ID LP args_list RP
        { printf("factor: ID LP args_list RP\n"); }
    | SYS_FUNCT LP args_list RP
        { printf("factor: SYS_FUNCT LP args_list RP\n"); }
    | const_value { $$ = $1; }
    | LP expression RP { $$ = $2; }
    | NOT factor
        { $$ = make_node<BinaryExprNode>(BinaryOp::XOR, make_node<BooleanNode>(true), $2); }
    | MINUS factor
        { $$ = make_node<BinaryExprNode>(BinaryOp::SUB, make_node<IntegerNode>(0), $2); }
    | ID LB expression RB
        { printf("factor: ID LB expression RB\n"); }
    | ID DOT ID
        { printf("factor: ID DOT ID\n"); }
    ;

args_list: args_list COMMA expression {
        $$ = $1; $$->add_child($3);
    }
    | expression {
        $$ = make_node<ArgListNode>(); $$->add_child($1);
    }
    ;

%%
    /*
void spc::Parser::error(const spc::location_type &loc, const std::string& msg) {
    std::cerr << loc << ": " << msg << std::endl;
    throw std::logic_error("Syntax error: invalid syntax");
}
    */