%{
    #include<stdio.h>
    #include<string>
    #include<stdlib.h>
    extern int yylex();
    void yyerror(const char *s) { printf("ERROR: %s\n", s); exit(1); }
%}

%union {
    std::string *string;
    int token;
}

%token <string> TIDENTIFIER TINTEGER TDOUBLE
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TDOT WHILE
%token <token> TPLUS TMINUS TMUL TDIV

/* Operator precedence for mathematical operators */
%left TPLUS TMINUS
%left TMUL TDIV

%start program

%%

program : stmts {printf("Start state\n");}
        ;
        
stmts : stmt
      | stmts stmt
      ;

stmt : var_decl | func_decl
     | expr | WHILE TLPAREN expr TRPAREN TLBRACE stmts TRBRACE
     ;

block : TLBRACE stmts TRBRACE
      | TLBRACE TRBRACE
      ;

var_decl : ident ident
         | ident ident TEQUAL expr 
         ;
        
func_decl : ident ident TLPAREN func_decl_args TRPAREN block 
          ;
    
func_decl_args : /*blank*/  
          | var_decl
          | func_decl_args TCOMMA var_decl 
          ;

ident : TIDENTIFIER
      ;

numeric : TINTEGER
        | TDOUBLE
        ;
    
expr : ident TEQUAL expr
     | ident TLPAREN call_args TRPAREN 
     | ident
     | numeric
     | expr comparison expr
     | TLPAREN expr TRPAREN
     ;
    
call_args : /*blank*/  
          | expr 
          | call_args TCOMMA expr 
          ;

comparison : TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE 
           | TPLUS | TMINUS | TMUL | TDIV
           ;

%%
