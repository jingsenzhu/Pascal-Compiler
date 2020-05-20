%{
#include<iostream>
#include<stdio.h>
#include<string>    

%}

%option c++
%option noyywrap

%%

"("     {return LP}
")"     {return RP}
"["     {return LB}
"]"     {return RB}
"."     {return DOT}
";"     {return COMMA}
","     {return COLON}
"*"     {return MUL}
"/"     {return DIV}
"+"     {return PLUS}
"-"     {return MINUS}
">="    {return GE}
">"     {return GT}
"<="    {return LE}
"<"     {return LT}
"="     {return EQUAL}
":="    {return ASSIGN}

"AND"       {std::cout << yytext;  return AND;}
"ARRAY"     {std::cout << yytext;  return ARRAY;}
"CONST"     {std::cout << yytext;  return CONST;}
"DIV"       {std::cout << yytext;  return DIV;}
"MUL"       {std::cout << yytext;  return MUL;}
"DO"        {std::cout << yytext;  return DO;}
"DOWNTO"    {std::cout << yytext;  return DOWNTO;}
"ELSE"      {std::cout << yytext;  return ELSE;}
"END"       {std::cout << yytext;  return END;}
"FOR"       {std::cout << yytext;  return FOR;}
"FUNCTION"  {std::cout << yytext;  return FUNCTION;}
"IF"        {std::cout << yytext;  return IF;}
"MOD"       {std::cout << yytext;  return MOD;}
"NOT"       {std::cout << yytext;  return NOT;}
"OF"        {std::cout << yytext;  return OF;}
"OR"        {std::cout << yytext;  return OR;}
"XOR"       {std::cout << yytext;  return XOR;}
"BEGIN"     {std::cout << yytext;  return PBEGIN;}
"PROCEDURE" {std::cout << yytext;  return PROCEDURE;}
"PROGRAM"   {std::cout << yytext;  return PROGRAM;}
"REPEAT"    {std::cout << yytext;  return REPEAT;}
"THEN"      {std::cout << yytext;  return THEN;}
"TO"        {std::cout << yytext;  return TO;}
"TYPE"      {std::cout << yytext;  return TYPE;}
"UNTIL"     {std::cout << yytext;  return UNTIL;}
"VAR"       {std::cout << yytext;  return VAR;}
"WHILE"     {std::cout << yytext;  return WHILE;}
"TRUE"      {std::cout << yytext; return TRUE;}
"FALSE"     {std::cout << yytext; return FALSE;}
"RECORD"    {std::cout << yytext; return RECORD;}
"WRITE"     {std::cout << yytext; return SYS_FUNC;}
"WRITELN"   {std::cout << yytext; return SYS_FUNC;}
"READ"      {std::cout << yytext; return SYS_FUNC;}
"READLN"    {std::cout << yytext; return SYS_FUNC;}
"INTEGER"   {std::cout << yytext; return SIMPLE_TYPE;}
"LONGINT"   {std::cout << yytext; return SIMPLE_TYPE;}
"BOOLEAN"   {std::cout << yytext; return SIMPLE_TYPE;}
"REAL"      {std::cout << yytext; return SIMPLE_TYPE;}
"STRING"    {std::cout << yytext; return STRING;}
[+|-][0-9]+      {std::cout << "Sign Integer: " << yytext; return SIGNEDDIGSEQ;}
[0-9]+      {std::cout << "Integer: " << yytext; return DIGSEQ;}
[+|-][0-9]+"."[0-9]+   {std::cout << "Sign Real Number: " << yytext; return SIGNEDREALNUMBER;}
[0-9]+"."[0-9]+   {std::cout << "Real Number: " << yytext; return REALNUMBER;}
'({NQUOTE}|'')+'  {std::cout << "STRING: " << yytext; yytext[yyleng-1] = 0; return LITERAL;}
[a-zA-Z]([a-zA-Z0-9])*  {std::cout << "IDD: " << yytext << " "; yytext[yyleng] = 0; return IDENTIFIER;}
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
