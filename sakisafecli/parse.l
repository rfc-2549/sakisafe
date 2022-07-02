%{
#include <stdio.h>
#define YYSTYPE char*
#include "y.tab.h"
extern YYSTYPE yylval;
%}

%%
server                  return SERVERTOK;
http_proxy              return HPTOK;
socks_proxy             return SPTOK;
use_socks_proxy         return USPTOK;
use_http_proxy          return UHPTOK;
force_ipv4              return IP4TOK;
force_ipv6              return IP6TOK;
\n                      /* ignore EOL */;
[a-zA-Z][a-zA-Z0-9]*    yylval=strdup(yytext); return WORD;
%%
