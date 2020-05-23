%{
#include<iostream>
#include<stdio.h>
#include<string>    
#include "utils/ast.hpp"

#undef YY_DECL
#define YY_DECL int yylex(spc::parser::semantic_type* lval, spc::parser::location_type* loc)

using token = spc::parser::token::yytokentype;
%}

NQUOTE [^']
%option caseless
%option noyywrap

%%
%{
    spc::parser::semantic_type* yylval = lval;
%}

"("     {return token::LP;}
")"     {return token::RP;}
"["     {return token::LB;}
"]"     {return token::RB;}
"."     {return token::DOT;}
".."    {return token::DOTDOT;}
";"     {return token::SEMI;}
","     {return token::COMMA;}
":"     {return token::COLON;}
"*"     {return token::MUL;}
"/"     {return token::TRUEDIV;}
"+"     {return token::PLUS;}
"-"     {return token::MINUS;}
">="    {return token::GE;}
">"     {return token::GT;}
"<="    {return token::LE;}
"<"     {return token::LT;}
"<>"    {return token::UNEQUAL;}
"="     {return token::EQUAL;}
":="    {return token::ASSIGN;}

"AND"       {std::cout << yytext;  return token::AND;}
"ARRAY"     {std::cout << yytext;  return token::ARRAY;}
"CASE"      {std::cout << yytext;  return token::CASE;}
"CONST"     {std::cout << yytext;  return token::CONST;}
"DIV"       {std::cout << yytext;  return token::DIV;}
"MOD"       {std::cout << yytext;  return token::MOD;}
"DO"        {std::cout << yytext;  return token::DO;}
"DOWNTO"    {
    std::cout << yytext;  
    yylval->build<spc::ForDirection>(spc::ForDirection::Downto);
    return token::DOWNTO;
}
"ELSE"      {std::cout << yytext;  return token::ELSE;}
"END"       {std::cout << yytext;  return token::END;}
"FOR"       {std::cout << yytext;  return token::FOR;}
"FUNCTION"  {std::cout << yytext;  return token::FUNCTION;}
"GOTO"      {std::cout << yytext;  return token::GOTO;}
"IF"        {std::cout << yytext;  return token::IF;}
"NOT"       {std::cout << yytext;  return token::NOT;}
"OF"        {std::cout << yytext;  return token::OF;}
"OR"        {std::cout << yytext;  return token::OR;}
"XOR"       {std::cout << yytext;  return token::XOR;}
"BEGIN"     {std::cout << yytext;  return token::PBEGIN;}
"PROCEDURE" {std::cout << yytext;  return token::PROCEDURE;}
"PROGRAM"   {std::cout << yytext;  return token::PROGRAM;}
"READ"      {
    std::cout << yytext;  
    yylval->build<spc::SysFunc>(spc::SysFunc::Read);
    return token::SYS_PROC;
}
"REPEAT"    {std::cout << yytext;  return token::REPEAT;}
"THEN"      {std::cout << yytext;  return token::THEN;}
"TO"        {
    std::cout << yytext; 
    yylval->build<spc::ForDirection>(spc::ForDirection::To); 
    return token::TO;
}
"TYPE"      {std::cout << yytext;  return token::TYPE;}
"UNTIL"     {std::cout << yytext;  return token::UNTIL;}
"VAR"       {std::cout << yytext;  return token::VAR;}
"WHILE"     {std::cout << yytext;  return token::WHILE;}
"RECORD"    {std::cout << yytext;  return token::RECORD;}

"FALSE"     {
    std::cout << yytext; 
    yylval->build<std::shared_ptr<BooleanNode>>(make_node<BooleanNode>(false)); 
    return token::SYS_CON;
}
"MAXINT"    {
    std::cout << yytext; 
    yylval->build<std::shared_ptr<IntegerNode>>(make_node<IntegerNode>(std::numeric_limits<int>::max()));
    return token::SYS_CON;
}
"TRUE"      {
    std::cout << yytext;
    yylval->build<std::shared_ptr<BooleanNode>>(make_node<BooleanNode>(true)); 
    return token::SYS_CON;
}
"ABS"       {
    std::cout << yytext;
    yylval->build<spc::SysFunc>(spc::SysFunc::Abs);
    return token::SYS_FUNCT;
}
"CHR"       {
    std::cout << yytext; 
    yylval->build<spc::SysFunc>(spc::SysFunc::Chr);
}
"ODD"       {
    std::cout << yytext; 
    yylval->build<spc::SysFunc>(spc::SysFunc::Odd);
    return token::SYS_FUNCT;
}
"ORD"       {
    std::cout << yytext; 
    yylval->build<spc::SysFunc>(spc::SysFunc::Ord);
    return token::SYS_FUNCT;
}
"PRED"      {
        std::cout << yytext; 
    yylval->build<spc::SysFunc>(spc::SysFunc::Pred);
    return token::SYS_FUNCT;
}
"SQR"       {
    std::cout << yytext; 
    yylval->build<spc::SysFunc>(spc::SysFunc::Sqr);
    return token::SYS_FUNCT;
}
"SQRT"      {
    std::cout << yytext; 
    yylval->build<spc::SysFunc>(spc::SysFunc::Sqrt);
    return token::SYS_FUNCT;
}
"SUCC"      {
    std::cout << yytext; 
    yylval->build<spc::SysFunc>(spc::SysFunc::Succ);
    return token::SYS_FUNCT;
}
"WRITE"     {
    std::cout << yytext; 
    yylval->build<spc::SysFunc>(spc::SysFunc::Write);
    return token::SYS_PROC;
}
"WRITELN"   {
    std::cout << yytext; 
    yylval->build<spc::SysFunc>(spc::SysFunc::Writeln);
    return token::SYS_PROC;
}

"BOOLEAN"   {
    std::cout << yytext; 
    yylval->build<std::shared_ptr<SimpleTypeNode>>(make_node<SimpleTypeNode>(spc::Type::Bool));
    return token::SYS_TYPE;
}
"CHAR"      {
    std::cout << yytext; 
    yylval->build<std::shared_ptr<SimpleTypeNode>>(make_node<SimpleTypeNode>(spc::Type::Char));  
    return token::SYS_TYPE;
}
"INTEGER"   {
    std::cout << yytext; 
    yylval->build<std::shared_ptr<SimpleTypeNode>>(make_node<SimpleTypeNode>(spc::Type::Int)); 
    return token::SYS_TYPE;
}
"REAL"      {
    std::cout << yytext;
    yylval->build<std::shared_ptr<SimpleTypeNode>>(make_node<SimpleTypeNode>(spc::Type::Real)); 
    return token::SYS_TYPE;
}
"STRING"    {
    std::cout << yytext; 
    yylval->build<std::shared_ptr<SimpleTypeNode>>(make_node<SimpleTypeNode>(spc::Type::String)); 
    return token::STRING;
}

[0-9]+      {
    std::cout << "Integer: " << yytext;
    yylval->build<std::shared_ptr<IntegerNode>>(make_node<IntegerNode>(atoi(yytext))); 
    return token::INTEGER;
}
[0-9]+"."[0-9]+("e"[+-]?[0-9]+)?   {
    std::cout << "Real Number: " << yytext; 
    yylval->build<std::shared_ptr<RealNode>>(make_node<RealNode>(atof(yytext))); 
    return token::REAL;
}
'{NQUOTE}'  {
    std::cout << "CHAR: " << yytext; 
    yylval->build<std::shared_ptr<CharNode>>(make_node<CharNode>(yytext[1])); 
    return token::CHAR;
}
'({NQUOTE}|'')+'  {
    std::cout << "STRING: " << yytext; 
    yytext[yyleng-1] = 0; 
    yylval->build<std::shared_ptr<StringNode>>(make_node<StringNode>(yytext + 1)); 
    return token::STRING;
}
[a-zA-Z_]([a-zA-Z0-9_])*  {
    std::cout << "IDD: " << yytext << " "; 
    yytext[yyleng] = 0;
    yylval->build<std::shared_ptr<IdentifierNode>>(make_node<IdentifierNode>(yytext));  
    return token::ID;
}
[\t\f]    {std::cout << ' '; continue;}
[\n\r]     {std::cout << std::endl;}

"(*" {
    char c;
    while(c = yyinput()) 
    {
        if(c == '*') 
        {
            if((c = yyinput()) == ')')
                break;
            else unput(c);
        }
    }
}
"{" {
    char c;
    while(c = yyinput()) 
    {
        if(c == '}') break;
    }
}
"//" {
    char c;
    while(c = yyinput()) 
    {
        if(c == '\n') 
        {
            break;
        }
        else if(c == EOF) {
            break;
        }
    }
}

. {}

%%

