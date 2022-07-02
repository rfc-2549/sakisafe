%{
#include <stdio.h>
#include "parse.h"
%}

%%
=                      return EQUAL;
server                  return SERVERTOK;
ipv4                    return IP4TOK;
ipv6                    return IP6TOK;
silent                  return SILTOK;
[a-zA-Z][a-zA-Z0-9]*    yylval.val = !strcmp(yytext,"true"); return WORD;
(http|https).*[a-zA-Z0-9\/.-]+        yylval.str = strdup(yytext); return URL;
\n                      return SEMICOLON;
[ \t]+
\"                     return QUOTE;
;                      return SEMICOLON;
#.*

%%
