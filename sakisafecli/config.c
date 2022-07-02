#include <libconfig.h>
#include <stdbool.h>
#include <stdio.h>
#include "options.h"
#include "config.h"

void
print_config()
{
	puts("Current configuration:");
	printf("Server: %s\n",rc.server);
	if(rc.socks_proxy_flag)
		printf("Socks proxy url: %s",rc.socks_proxy_url);
	if(rc.http_proxy_flag)
		printf("HTTP proxy url: %s",rc.http_proxy_url);
	if(rc.silent_flag)
		puts("Silent: true");
	else
		puts("Silent: false");
	if(rc.ipv6_flag)
		printf("Force IPv6: true\n");
	else
		printf("Force IPv6: false\n");
	if(rc.ipv4_flag)
		printf("Force IPv4: true\n");
	else
		printf("Force IPv4: false\n");

	return;
}

void
init_config(struct config *rc)
{
	rc->http_proxy_flag = false;
	rc->socks_proxy_flag = false;
	rc->http_proxy_url = NULL;
	rc->socks_proxy_url = NULL;
	rc->ipv4_flag = false;
	rc->ipv6_flag = false;
	rc->silent_flag = false;
	rc->server = "https://lainsafe.delegao.moe";
}
void
load_config()
{
	yyin = fopen("/usr/home/qorg/.sakisafeclirc","r");
	yyparse();
}
