%{
#include <stdio.h>
#include "string.h"
#include <stdlib.h>
#include <unistd.h>
#include "config.h"

    struct config rc;
    int yyparse(void);
    void yyerror(const char *str) {
	   fprintf(stderr,"error: %s\n",str);
	   _exit(-1);
    }
	   %}


%token SERVERTOK HPTOK SPTOK USPTOK UHPTOK IP4TOK IP6TOK


%%
%%

conf_statements:
|
conf_statements conf_statement SEMICOLON
;

conf_statement:
statements
|
SERVERTOK quotedname
{
    rc.server = $2;
}

quotedname:
QUOTE WORD QUOTE
{
    $$=$2;
}

statements:
| statements statement
;

statement: WORD | quotedname

%%
