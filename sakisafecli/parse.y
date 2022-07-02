%{
#include <stdio.h>
#include "string.h"
#include <stdlib.h>
#include <unistd.h>
#include "config.h"

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

%union YYSTYPE {
    int val;
    char *str;
}

%token SERVERTOK HPTOK SPTOK USPTOK UHPTOK IP4TOK IP6TOK SILTOK QUOTE
%token URL SEMICOLON WORD EQUAL

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
    if($2.val)
	   rc.ipv4_flag = true;
    else
	   rc.ipv4_flag = false;
}
|
IP6TOK quotedword
{
    if($2.val)
	   rc.ipv6_flag = true;
    else
	   rc.ipv6_flag = false;
}
|
SERVERTOK quotedname
{
    rc.server = $2.str;
}
|
HPTOK quotedname
{
    rc.http_proxy_url = $2.str;
}
|
SPTOK quotedname
{
    rc.socks_proxy_url = $2.str;
}
|
SILTOK quotedword
{
    if($2.val)
	   rc.silent_flag = true;
    else
	   rc.silent_flag = false;
}			 
;

quotedname:
EQUAL QUOTE URL QUOTE
{
    $$=$3;
}

quotedword:
EQUAL QUOTE WORD QUOTE
{
    $$=$3;
}

statements:
| statements statement
;

statement: URL | quotedname | quotedword

%%
