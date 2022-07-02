%{
#include <stdio.h>
#define YYSTYPE char*
#include "parse.h"
extern YYSTYPE yylval;
%}

%%
server                  return SERVERTOK;
ipv4                    return IP4TOK;
ipv6                    return IP6TOK;
[a-zA-Z][a-zA-Z0-9]*    yylval=strdup(yytext); return WORD;
[a-zA-Z0-9\/.-]+        yylval=strdup(yytext); return URL;
\n                      /* ignore EOL */;
[ \t]+
\"                     return QUOTE;
;                      return SEMICOLON;
%%
