#include <stdbool.h>
#include <stdio.h>

struct config
{
	bool ipv6_flag;
	bool ipv4_flag;
	bool http_proxy_flag;
	bool socks_proxy_flag;
	bool silent_flag;
	bool paste_flag;
	char *http_proxy_url;
	char *socks_proxy_url;
	char *server;
};

extern struct config rc;
extern FILE *yyin;
/* Init the config */
void
init_config(struct config *rc);

void
load_config();
