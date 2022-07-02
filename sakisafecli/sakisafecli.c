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
#include "config.h"
#include "funcs.h"
#include "sakisafecli.h"

/* Config variables */

const char *path = ".cache/sakisafelinks";
int
main(int argc, char **argv)
{
#ifdef __OpenBSD__
	if(pledge("stdio rpath cpath inet dns unveil tmppath", "") == -1) {
		err(1, "pledge");
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
	
	init_config(&rc);
	load_config();
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
		{ "paste", no_argument, 0, 'x' },
		{ 0, 0, 0, 0 }
	};

	int c = 0;
	while((c = getopt_long(
			  argc, argv, "46hT:p:P:Ss:xC", long_options, &option_index)) !=
		 -1) {
		switch(c) {
			case 's':
				rc.server = optarg;
				break;
			case 'h':
				print_help();
				free(buffer);
				curl_easy_cleanup(easy_handle);
				return 0;
				break;
			case 'p':
				rc.socks_proxy_url = optarg;
				rc.socks_proxy_flag = true;
				break;
			case 'P':
				rc.http_proxy_url = optarg;
				rc.http_proxy_flag = true;
				break;
			case 'S':
				rc.silent_flag = true;
				break;
			case '4':
				rc.ipv4_flag = true;
				break;
			case '6':
				rc.ipv6_flag = true;
				break;
			case 'x':
				/* We don't want the progress bar in this case */
				rc.silent_flag = true;
				rc.paste_flag = true;
				break;
			case '?':
				print_usage();
				return 0;
				break;
			case 'C':
				print_config();
				return 0;
			default:
				print_usage();
				return 0;
				break;
		}
	}

	if(access(argv[optind], F_OK) && !rc.paste_flag) {
		fprintf(stderr, "Error opening file\n");
		return -1;
	}

	/* curl options */
	curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, buffer);
	curl_easy_setopt(easy_handle, CURLOPT_URL, rc.server);

	/* Proxy options */

	if(rc.socks_proxy_flag && rc.http_proxy_flag) {
		fprintf(stderr, "Socks_Proxy and HTTP_PROXY can't be used at once\n");
		return -1;
	} else if(rc.socks_proxy_flag) {
		curl_easy_setopt(easy_handle, CURLOPT_PROXY, rc.socks_proxy_url);
		curl_easy_setopt(
			easy_handle, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5_HOSTNAME);
	} else if(rc.http_proxy_flag) {
		curl_easy_setopt(easy_handle, CURLOPT_PROXY, rc.http_proxy_url);
		curl_easy_setopt(easy_handle, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
	}

	/* Which address to use */

	if(rc.ipv6_flag)
		curl_easy_setopt(easy_handle, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V6);
	else if(rc.ipv4_flag)
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
	char *filename = argv[optind];

	if(rc.paste_flag)
		filename = "/dev/stdin";

	curl_mime_filedata(file_data, filename);
	curl_mime_name(file_data, form_key);
	if(rc.paste_flag)
		curl_mime_filename(file_data, "-");

	curl_easy_setopt(easy_handle, CURLOPT_NOPROGRESS, rc.silent_flag);
	curl_easy_setopt(easy_handle, CURLOPT_PROGRESSFUNCTION, progress);

	curl_easy_setopt(easy_handle, CURLOPT_MIMEPOST, mime);
	curl_easy_perform(easy_handle);

	if(!rc.silent_flag)
		putchar('\n');

	puts(buffer);

	curl_mime_free(mime);
	curl_easy_cleanup(easy_handle);

	free(buffer);
	return 0;
}
