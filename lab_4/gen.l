%{
#include <string>
#include "AST.h"
#include "gen.tab.h"  // created by bison -d gen.ypp
%}
%%
[0-9]+(\.[0-9]*)?            {yylval.f = atof(yytext); return FLOAT_LIT;}
if                           {return IF;}
else                         {return ELSE;}
while                        {return WHILE;}
[a-zA-Z_][a-zA-Z_0-9]*       {yylval.s = new std::string(yytext); return ID;}
"||"                         {return OR;}
"&&"                         {return AND;}
"=="                         {return EQ;}
"!="                         {return NE;}
"<="                         {return LE;}
">="                         {return GE;}
[-\(\)\,\;\!\+\*\/\{\}\=\<\>] {return yytext[0];}
[ \t\v\f]                    { ; }
#.*                          { ; }
\n                           {yylineno++;}
%%
