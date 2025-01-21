#include <libconfig.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <err.h>
#include <getopt.h>
#include <unistd.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <errno.h>
#ifdef use_libbsd
#include <bsd/string.h>
#endif
#include "curl/easy.h"
#include "options.h"
#include "config.h"
#include "funcs.h"
#include "sakisafecli.h"

CURL *easy_handle;

char *buffer;
/* Config variables */

bool ipv6_flag, ipv4_flag, http_proxy_flag,
	socks_proxy_flag, silent_flag, paste_flag;

char *http_proxy_url, *socks_proxy_url;
char *ssh_key_path;

char *server;
const char *path;


int
main(int argc, char **argv)
{
#ifdef __OpenBSD__
	if(pledge("stdio rpath cpath inet dns unveil tmppath", "") == -1) {
		err(1, "pledge");
		_exit(-1);
	}
#endif

	buffer = (char *)calloc(1024, sizeof(char));

	if(buffer == NULL) {
		fprintf(stderr, "Error allocating memory for buffer!\n");
		return -1;
	}
	char config_location[512];
	char *sakisafeclirc_env = getenv("SAKISAFECLIRC");

	if(sakisafeclirc_env == NULL) {
		snprintf(config_location, 512, "%s/.sakisafeclirc", getenv("HOME"));
		FILE *fp = fopen(config_location, "r");
		if(fp != NULL) {
			parse_config_file(fp);
			fclose(fp);
		}
	} else {
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(use_libbsd)
		strlcpy(config_location, sakisafeclirc_env, 512);
#else /* Linux sucks! */
		strncpy(config_location, sakisafeclirc_env, 512);
#endif
		FILE *fp = fopen(config_location, "r");
		if(fp != NULL) {
			parse_config_file(fp);
			fclose(fp);
		}
	}
	/* libcurl initialization */

	easy_handle = curl_easy_init();

	if(!easy_handle) {
		fprintf(stderr, "Error initializing libcurl\n");
		return -1;
	}

	if(argc == optind) {
		print_usage();
		free(buffer);
		curl_easy_cleanup(easy_handle);
		return -1;
	}
	init_sakisafe_options();
	int option_index = 0;
	static struct option long_options[] = {
		{ "server", required_argument, 0, 's' },
		{ "help", no_argument, 0, 'h' },
		{ "socks-proxy", required_argument, 0, 'p' },
		{ "http-proxy", required_argument, 0, 'P' },
		{ "silent", no_argument, 0, 'S' },
		{ "ipv4", no_argument, 0, '4' },
		{ "ipv6", no_argument, 0, '6' },
		{ "paste", no_argument, 0, 'x' },
		{ "key", required_argument, 0, 'k' },
		{ 0, 0, 0, 0 }
	};

	int c = 0;
	while(
		(c = getopt_long(
			 argc, argv, "46hT:p:P:Ss:xCk:", long_options, &option_index)) !=
		-1) {
		switch(c) {
			case 's':
				server = optarg;
				break;
			case 'h':
				print_help();
				free(buffer);
				curl_easy_cleanup(easy_handle);
				return 0;
				break;
			case 'p':
				socks_proxy_url = optarg;
				socks_proxy_flag = true;
				break;
			case 'P':
				http_proxy_url = optarg;
				http_proxy_flag = true;
				break;
			case 'S':
				silent_flag = true;
				break;
			case '4':
				ipv4_flag = true;
				break;
			case '6':
				ipv6_flag = true;
				break;
			case 'x':
				/* We don't want the progress bar in this case */
				silent_flag = true;
				paste_flag = true;
				break;
			case '?':
				print_usage();
				return 0;
				break;
			case 'C':
				print_config();
				return 0;
			case 'k':
				ssh_key_path = optarg;
				break;
			default:
				print_usage();
				return 0;
				break;
		}
	}

	if(access(argv[optind], F_OK) && !paste_flag) {
		fprintf(stderr, "Error opening file: %s\n", strerror(errno));
		return -1;
	}

	/* curl options */
	if(curl_easy_setopt(easy_handle, CURLOPT_USERAGENT, "curl") != 0)
		die("Error setting CURLOPT_USERAGENT");
	if(curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_data) != 0)
		die("Error setting CURLOPT_WRITEFUNCTION");
	if(curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, buffer) != 0)
		die("error setting CURLOPT_WRITEDATA");

	if(curl_easy_setopt(easy_handle, CURLOPT_URL, server) != 0)
		die("error setting CURLOPT_URL");
	
	int protocol = get_protocol(server);

	/* Proxy options */

	if(socks_proxy_flag && http_proxy_flag) {
		fprintf(stderr, "Socks_Proxy and HTTP_PROXY can't be used at once\n");
		return -1;
	} else if(socks_proxy_flag) {
		curl_easy_setopt(easy_handle, CURLOPT_PROXY, socks_proxy_url);
		curl_easy_setopt(
			easy_handle, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5_HOSTNAME);
	} else if(http_proxy_flag && protocol == CURLPROTO_HTTP) {
		curl_easy_setopt(easy_handle, CURLOPT_PROXY, http_proxy_url);
		curl_easy_setopt(easy_handle, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
	}

	/* Which address to use */

	if(ipv6_flag)
		curl_easy_setopt(easy_handle, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V6);
	else if(ipv4_flag)
		curl_easy_setopt(easy_handle, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
	else
		curl_easy_setopt(
			easy_handle, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_WHATEVER);

	/* Common options for both HTTP and SCP transfers */

	curl_easy_setopt(easy_handle, CURLOPT_NOPROGRESS, silent_flag);
	struct progress mem;
	curl_easy_setopt(easy_handle, CURLOPT_XFERINFODATA, &mem);
	curl_easy_setopt(easy_handle, CURLOPT_XFERINFOFUNCTION, progress);

	/* File name */

	/* TODO: make it iterate on args so you can upload multiple files
	 *  at once (sakisafecli file1 file2 ... filen)
	 */

	/* Process HTTP uploads */

	if(protocol == CURLPROTO_HTTP) {
		upload_file_http(argc, argv);
	}
	/* Process SCP uploads */
	else if(protocol == CURLPROTO_SCP) {
		upload_file_scp(argc, argv);
	} else {
		puts("Unsupported protocol");
		return -1;
	}

	curl_easy_cleanup(easy_handle);
	config_destroy(&runtime_config);
	free(buffer);
	return 0;
}

