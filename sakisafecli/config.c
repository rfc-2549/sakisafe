#include <libconfig.h>
#include <stdbool.h>
#include <stdio.h>
#include "options.h"
#include "config.h"

void
print_config()
{
	puts("Current configuration:");
	printf("Server: %s\n", server);
	if(socks_proxy_flag)
		printf("Socks proxy url: %s", socks_proxy_url);
	if(http_proxy_flag)
		printf("HTTP proxy url: %s", http_proxy_url);
	if(silent_flag)
		puts("Silent: true");
	else
		puts("Silent: false");
	if(ipv6_flag)
		printf("Force IPv6: true\n");
	else
		printf("Force IPv6: false\n");
	if(ipv4_flag)
		printf("Force IPv4: true\n");
	else
		printf("Force IPv4: false\n");

	return;
}

void
parse_config_file(FILE *config)
{

	config_t runtime_config;

	config_init(&runtime_config);
	config_read(&runtime_config, config);
	config_setting_t *cur;
	cur = config_lookup(&runtime_config, "server");
	
	if(config != NULL) {
		if(cur != NULL)
			server = (char *)config_setting_get_string(cur);
		cur = config_lookup(&runtime_config, "socks_proxy");
		if(cur != NULL)
			socks_proxy_url = (char *)config_setting_get_string(cur);
		cur = config_lookup(&runtime_config, "http_proxy");
		if(cur != NULL)
			http_proxy_url = (char *)config_setting_get_string(cur);
		cur = config_lookup(&runtime_config, "use_socks_proxy");
		if(cur != NULL)
			socks_proxy_flag = config_setting_get_bool(cur);
		cur = config_lookup(&runtime_config, "use_http_proxy");
		if(cur != NULL)
			http_proxy_flag = config_setting_get_bool(cur);
		cur = config_lookup(&runtime_config, "silent");
		if(cur != NULL)
			silent_flag = config_setting_get_bool(cur);
		cur = config_lookup(&runtime_config, "force_ipv6");
		if(cur != NULL)
			ipv6_flag = config_setting_get_bool(cur);
		cur = config_lookup(&runtime_config, "force_ipv4");
		if(cur != NULL)
			ipv4_flag = config_setting_get_bool(cur);
		cur = config_lookup(&runtime_config, "key");
		if(cur != NULL)
			ssh_key_path = (char *)config_setting_get_string(cur);
	}
}
