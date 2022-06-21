#include <libconfig.h>
#include <stdbool.h>
#include "options.h"
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
	}
}
