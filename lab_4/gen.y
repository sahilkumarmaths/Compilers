%{
#include "Label.h"
#include "AST.h"
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstdio>
using namespace std;
void yyerror(const string& msg);
extern int yylex();  // built via flex gen.l
%}
%union {
  std::string *s;
  float f;
  Expr *expr;
  Stmt *stmt;
  std::vector<Expr*> *exprs;
  std::vector<Stmt*> *stmts;
}
%token <s> ID
%token <f> FLOAT_LIT
%type <stmt> assign_stmt call_stmt block_stmt if_stmt while_stmt
%type <expr> expr
%type <exprs> actuals actual_list array_index
%type <stmt> stmt
%type <stmts> stmts
%token IF ELSE WHILE
%left OR
%left AND
%right '!'
%nonassoc EQ NE
%nonassoc '<' LE '>' GE
%left '+' '-'
%left '*' '/'
%right UMINUS
%expect 1
%%
prog         : prog stmt   {Label next; 
                            $2->genCode(next); 
                            delete $2;
                            cout << next << ":" << endl;}
             | 
             ;

stmt         : assign_stmt
             | call_stmt
             | block_stmt
             | if_stmt
             | while_stmt
             ;

assign_stmt  : ID '=' expr ';'         {$$ = new AssignStmt($1,$3);}
             ;

call_stmt    : ID '(' actuals ')' ';'  {$$ = new CallStmt($1,$3);}
             ;

actuals      : actual_list 
             |                         {$$ = new vector<Expr*>;}
             ;

actual_list  : actual_list ',' expr    {$1->push_back($3); $$ = $1;}
             | expr                    {$$ = new vector<Expr*>;
                                        $$->push_back($1);}
             ;

block_stmt   : '{' stmts '}'           {$$ = new BlockStmt($2);}
             ;

stmts        : stmts stmt  {$1->push_back($2); $$ = $1;}
             |             {$$ = new vector<Stmt*>;}
             ;

if_stmt      : IF '(' expr ')' stmt    {$$ = new IfStmt($3,$5);}
             | IF '(' expr ')' stmt ELSE stmt
                                       {$$ = new IfElseStmt($3,$5,$7);}
             ;

while_stmt   : WHILE '(' expr ')' stmt {$$ = new WhileStmt($3,$5);}
             ;

expr         : expr OR expr          {$$ = new OrExpr($1,$3);}
             | expr AND expr         {$$ = new AndExpr($1,$3);}
             | '!' expr              {$$ = new NotExpr($2);}
             | expr EQ expr          {$$ = new CmpExpr("==",$1,$3);}
             | expr NE expr          {$$ = new CmpExpr("!=",$1,$3);}
             | expr '<' expr         {$$ = new CmpExpr("<",$1,$3);}
             | expr LE expr          {$$ = new CmpExpr("<=",$1,$3);}
             | expr '>' expr         {$$ = new CmpExpr(">",$1,$3);}
             | expr GE expr          {$$ = new CmpExpr(">=",$1,$3);}
             | expr '+' expr         {$$ = new BinaryArithExpr("+",$1,$3);}
             | expr '-' expr         {$$ = new BinaryArithExpr("-",$1,$3);}
             | expr '*' expr         {$$ = new BinaryArithExpr("*",$1,$3);}
             | expr '/' expr         {$$ = new BinaryArithExpr("/",$1,$3);}
             | '-' expr %prec UMINUS {$$ = new UnaryArithExpr("-",$2);}
             | '+' expr %prec UMINUS {$$ = $2;}
             | '(' expr ')'          {$$ = $2;}
             | ID                    {$$ = new VarExpr($1);}
             | ID '(' actuals ')'    {$$ = new CallExpr($1,$3);}
             | FLOAT_LIT             {$$ = new ConstExpr($1);}
             | ID array_index        {$$ = new ArrayLoadExpr($1,$2);}
             ;

array_index : array_index '[' expr ']' {$1->push_back($3); $$ = $1;}
            | '[' expr ']'   {$$ = new vector<Expr*>; $$->push_back($2);}
            ;

%%

extern int yylineno;

void yyerror(const string& msg) {
  cerr << yylineno << ":" << msg << endl;
  exit(1);
}

extern FILE *yyin;

int main() {
  yyin = stdin;
  yyparse();
  return 0;
}
