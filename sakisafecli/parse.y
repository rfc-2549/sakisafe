%{
#include <stdio.h>
#include "string.h"
#include <stdlib.h>
#include <unistd.h>
#include "config.h"
#define YYSTYPE char*
    struct config rc;
    int yyparse(void);
    void yyerror(const char *str) {
	   fprintf(stderr,"Error reading config file: %s\n",str);
	   _exit(-1);
    }

    int yywrap()
    {
	   return 1;
    }

%}


%token SERVERTOK HPTOK SPTOK USPTOK UHPTOK IP4TOK IP6TOK QUOTE URL SEMICOLON WORD


%%

conf_statements:
|
conf_statements conf_statement SEMICOLON
;

conf_statement:
statements
|
IP4TOK quotedword
{
    if(!strcmp($2,"true"))
	   rc.ipv4_flag = true;
    else
	   rc.ipv4_flag = false;
}
|
IP6TOK quotedword
{
    if(!strcmp($2,"true"))
	   rc.ipv6_flag = true;
    else
	   rc.ipv6_flag = false;
}
|
SERVERTOK quotedname
{
    rc.server = $2;
}
|
HPTOK quotedname
{
    rc.http_proxy_url = $2;
}
|
SPTOK quotedname
{
    rc.socks_proxy_url = $2;
}
;

quotedname:
QUOTE URL QUOTE
{
    $$=$2;
}

quotedword:
QUOTE WORD QUOTE
{
    $$=$2;
}

statements:
| statements statement
;

statement: URL | quotedname | quotedword

%%
