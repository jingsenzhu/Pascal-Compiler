%{
#include<iostream>
#include<stdio.h>
#include<string>    
#include "parser.hpp"

#undef YY_DECL
#define YY_DECL int yylex(spc::parser::semantic_type* lval, spc::parser::location_type* loc)

using token = spc::parser::token::yytokentype;
%}

NQUOTE [^']
%option caseless
%option noyywrap

%%

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
"DOWNTO"    {std::cout << yytext;  return token::DOWNTO;}
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
"READ"      {std::cout << yytext; return token::READ;}
"REPEAT"    {std::cout << yytext;  return token::REPEAT;}
"THEN"      {std::cout << yytext;  return token::THEN;}
"TO"        {std::cout << yytext;  return token::TO;}
"TYPE"      {std::cout << yytext;  return token::TYPE;}
"UNTIL"     {std::cout << yytext;  return token::UNTIL;}
"VAR"       {std::cout << yytext;  return token::VAR;}
"WHILE"     {std::cout << yytext;  return token::WHILE;}
"RECORD"    {std::cout << yytext; return token::RECORD;}

"FALSE"     {std::cout << yytext; return token::SYS_CON;}
"MAXINT"    {std::cout << yytext; return token::SYS_CON;}
"TRUE"      {std::cout << yytext; return token::SYS_CON;}
"ABS"       {std::cout << yytext; return token::SYS_FUNCT;}
"CHR"       {std::cout << yytext; return token::SYS_FUNCT;}
"ODD"       {std::cout << yytext; return token::SYS_FUNCT;}
"ORD"       {std::cout << yytext; return token::SYS_FUNCT;}
"PRED"      {std::cout << yytext; return token::SYS_FUNCT;}
"SQR"       {std::cout << yytext; return token::SYS_FUNCT;}
"SQRT"      {std::cout << yytext; return token::SYS_FUNCT;}
"SUCC"      {std::cout << yytext; return token::SYS_FUNCT;}
"WRITE"     {std::cout << yytext; return token::SYS_PROC;}
"WRITELN"   {std::cout << yytext; return token::SYS_PROC;}

"BOOLEAN"   {std::cout << yytext; return token::SYS_TYPE;}
"CHAR"      {std::cout << yytext; return token::SYS_TYPE;}
"INTEGER"   {std::cout << yytext; return token::SYS_TYPE;}
"REAL"      {std::cout << yytext; return token::SYS_TYPE;}
"STRING"    {std::cout << yytext; return token::STRING;}

[0-9]+      {std::cout << "Integer: " << yytext; return token::INTEGER;}
[0-9]+"."[0-9]+   {std::cout << "Real Number: " << yytext; return token::REAL;}
'{NQUOTE}'  {std::cout << "CHAR: " << yytext; return token::CHAR;}
'({NQUOTE}|'')+'  {std::cout << "STRING: " << yytext; yytext[yyleng-1] = 0; return token::STRING;}
[a-zA-Z_]([a-zA-Z0-9_])*  {std::cout << "IDD: " << yytext << " "; yytext[yyleng] = 0; return token::ID;}
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

