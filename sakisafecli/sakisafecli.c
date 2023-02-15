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

/* Config variables */

bool ipv6_flag = false, ipv4_flag = false, http_proxy_flag = false,
	socks_proxy_flag = false, silent_flag = false, paste_flag = false;

char *http_proxy_url, *socks_proxy_url;
char *ssh_key_path = NULL;

char *server = "https://lainsafe.delegao.moe";
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

	CURL *easy_handle = curl_easy_init();

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
	curl_easy_setopt(easy_handle, CURLOPT_USERAGENT, "curl");
	curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, buffer);

	curl_easy_setopt(easy_handle, CURLOPT_URL, server);

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
	curl_easy_setopt(easy_handle, CURLOPT_PROGRESSFUNCTION, progress);

	/* File name */

	/* TODO: make it iterate on args so you can upload multiple files
	 *  at once (sakisafecli file1 file2 ... filen)
	 */

	/* Process HTTP uploads */

	if(protocol == CURLPROTO_HTTP) {
		curl_mime *mime;
		mime = curl_mime_init(easy_handle);

		curl_easy_setopt(easy_handle, CURLOPT_MIMEPOST, mime);
		if(!mime) {
			fprintf(stderr, "Error initializing curl_mime\n");
		}

		curl_mimepart *file_data;
		file_data = curl_mime_addpart(mime);
		char *filename = argv[optind];

		if(paste_flag)
			filename = "/dev/stdin";

		curl_mime_filedata(file_data, filename);
		curl_mime_name(file_data, form_key);
		if(paste_flag)
			curl_mime_filename(file_data, "-");

		curl_easy_perform(easy_handle);
		if(!silent_flag)
			putchar('\n');
		puts(buffer);
		curl_mime_free(mime);

	}
	/* Process SCP uploads */
	else if(protocol == CURLPROTO_SCP) {
		curl_easy_setopt(
			easy_handle, CURLOPT_SSH_PRIVATE_KEYFILE, ssh_key_path);

		char path[256];
		char *filename = argv[optind];

		curl_easy_setopt(easy_handle, CURLOPT_UPLOAD, true);
		FILE *fp = fopen(filename, "r");
		if(fp == NULL) {
			fprintf(stderr, "%s", strerror(errno));
			exit(-1);
		}

		struct stat st;
		stat(argv[optind], &st);
		snprintf(path, 256, "%s/%s", server, filename);
		curl_easy_setopt(easy_handle, CURLOPT_READDATA, fp);
		curl_easy_setopt(
			easy_handle, CURLOPT_INFILESIZE_LARGE, (curl_off_t)st.st_size);

		int ret = curl_easy_perform(easy_handle);
		putchar('\n');
		if(ret != 0) {
			fprintf(stderr, "%i: %s\n", ret, curl_easy_strerror(ret));
		}

	} else {
		puts("Unsupported protocol");
		return -1;
	}

	curl_easy_cleanup(easy_handle);
	config_destroy(&runtime_config);
	free(buffer);
	return 0;
}

int
get_protocol(char *server)
{
	if(strstr(server, "http://") != NULL || strstr(server, "https://"))
		return CURLPROTO_HTTP;
	else if(strstr(server, "scp://") != NULL)
		return CURLPROTO_SCP;
	else
		return -1;
}
