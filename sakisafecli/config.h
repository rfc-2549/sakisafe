#pragma once
#include <curl/curl.h>
#include <libconfig.h>
#include <stdbool.h>


/* Parse the config file */
void
parse_config_file(FILE *config);

/* Print the current settings */
void
print_config();

/* Internal variables */

extern CURL *easy_handle;

extern char *buffer;
/* Config variables */

extern bool ipv6_flag, ipv4_flag, http_proxy_flag,
	socks_proxy_flag, silent_flag, paste_flag;

extern char *http_proxy_url, *socks_proxy_url;
extern char *ssh_key_path;

extern char *server;
extern const char *path;

