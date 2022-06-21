#include <libconfig.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <err.h>
#include <getopt.h>
#include <unistd.h>
#include <curl/curl.h>

#include "options.h"
#include "sakisafecli.h"

/* Config variables */
char *socks_proxy_url, *http_proxy_url;

bool socks_proxy_flag = false, http_proxy_flag = false;
bool ipv6_flag = false, ipv4_flag = false;
bool silent_flag = false;
config_t runtime_config;

int
main(int argc, char **argv)
{
	#ifdef __OpenBSD__
	if(pledge("stdio rpath cpath inet dns unveil tmppath","") == -1) {
		err(1,"pledge");
		_exit(-1);
	}
	#endif

	

	char *form_key = "file";

	char *buffer = (char *)calloc(1024, sizeof(char));

	if(buffer == NULL) {
		fprintf(stderr, "Error allocating memory!\n");
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
		#if defined(__OpenBSD__) || defined(__FreeBSD__)
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
	
	CURL *easy_handle = curl_easy_init();
	curl_mime *mime;
	mime = curl_mime_init(easy_handle);

	if(!easy_handle) {
		fprintf(stderr, "Error initializing libcurl\n");
		return -1;
	}
	if(!mime) {
		fprintf(stderr, "Error initializing curl_mime\n");
	}
	
	if(argc == optind) {
		print_usage();
		free(buffer);
		curl_easy_cleanup(easy_handle);
		curl_mime_free(mime);
		return -1;
	}

	int option_index = 0;
	static struct option long_options[] = {
		{ "server", required_argument, 0, 's' },
		{ "help", no_argument, 0, 'h' },
		{ "socks-proxy", required_argument, 0, 'p' },
		{ "http-proxy", required_argument, 0, 'P' },
		{ "silent", no_argument, 0, 'S' },
		{ "ipv4", no_argument, 0, '4' },
		{ "ipv6", no_argument, 0, '6' },
		{ 0, 0, 0, 0 }
	};

	int c = 0;
	while((c = getopt_long(
			  argc, argv, "46hT:p:P:Ss:", long_options, &option_index)) !=
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
			case '?':
				print_usage();
				return 0;
				break;
			default:
				print_usage();
				return 0;
				break;
		}
	}

	if(access(argv[optind], F_OK)) {
		fprintf(stderr, "Error opening file\n");
		return -1;
	}

	/* curl options */
	curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, buffer);
	curl_easy_setopt(easy_handle, CURLOPT_URL, server);

	/* Proxy options */

	if(socks_proxy_flag && http_proxy_flag) {
		fprintf(stderr, "Socks_Proxy and HTTP_PROXY can't be used at once\n");
		return -1;
	} else if(socks_proxy_flag) {
		curl_easy_setopt(easy_handle, CURLOPT_PROXY, socks_proxy_url);
		curl_easy_setopt(
			easy_handle, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5_HOSTNAME);
	} else if(http_proxy_flag) {
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

	/* Form parameters */

	/* File name */

	/* TODO: make it iterate on args so you can upload multiple files
	 *  at once (sakisafecli file1 file2 ... filen)
	 */
	curl_mimepart *file_data;
	file_data = curl_mime_addpart(mime);
	for(int i = optind; i < argc; i++) {
		curl_mime_filedata(file_data, argv[i]);
		curl_mime_name(file_data, form_key);
	}
					   

		curl_easy_setopt(easy_handle, CURLOPT_NOPROGRESS, silent_flag);
		curl_easy_setopt(easy_handle, CURLOPT_PROGRESSFUNCTION, progress);
		
		curl_easy_setopt(easy_handle, CURLOPT_MIMEPOST, mime);
		curl_easy_perform(easy_handle);

		if(!silent_flag)
			putchar('\n');

		puts(buffer);

	curl_mime_free(mime);
	curl_easy_cleanup(easy_handle);

	free(buffer);
	config_destroy(&runtime_config);
	return 0;
}

void
parse_config_file(FILE *config)
{
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
