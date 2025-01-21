#include <curl/system.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libconfig.h>
#include <curl/curl.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <getopt.h>
#include "sakisafecli.h"
#include "config.h"

size_t
write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
	memcpy(userp, buffer, nmemb * size);
	return 0;
}

void
print_usage()
{
	printf("USAGE: sakisafecli [-p socks proxy|-P http proxy] [-x] [-s] "
		  "[-6|-4] [--server] FILE\n");
	return;
}

void
print_help()
{
	printf("-s|--server: specifies the sakisafe "
		  "server\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s",
		  "-t|--token: Authentication token (https://u.kalli.st)",
		  "-P|--http-proxy: http proxy to use e.g. http://127.0.0.1:4444",
		  "-p|--socks-proxy: SOCK proxy to use e.g. 127.0.0.1:9050",
		  "-6|--ipv6: uses IPv6 only",
		  "-4|--ipv6: uses IPv4 only",
		  "-S|--silent: doesn't print progress",
		  "-x|--paste: read file from stdin",
		  "-C: print current settings",
		  "-h|--help: print this message.\n");
	return;
}

size_t
progress(void *clientp,
	    curl_off_t dltotal,
	    curl_off_t dlnow,
	    curl_off_t ultotal,
	    curl_off_t ulnow)
{
	/* I don't know why the fuck I have to do this */
	if(ultotal == 0) {
		ultotal++;
	}
	printf("\r%li uploaded of %li (\033[32;1m%li%%\033[30;0m)",
		  ulnow,
		  ultotal,
		  ulnow * 100 / ultotal);
	fflush(stdout);
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
void
die(const char *msg)
{
	fprintf(stderr, "%i: %s", errno, msg);
}

void
init_sakisafe_options()
{
	ipv6_flag = http_proxy_flag,
	socks_proxy_flag = paste_flag = silent_flag = false;

	ipv4_flag = true;

	ssh_key_path = NULL;
	server = "https://meth.cat";
	path = ".cache/sakisafelinks";
}

int
upload_file_http(int argc, char **argv)
{

	curl_mime *mime;
	mime = curl_mime_init(easy_handle);

	curl_easy_setopt(easy_handle, CURLOPT_MIMEPOST, mime);
	if(!mime) {
		fprintf(stderr, "Error initializing curl_mime\n");
		return -1;
	}

	curl_mimepart *file_data;
	file_data = curl_mime_addpart(mime);
	char *filename = argv[optind - 1];
	/* Get file from stdin */
	if(paste_flag)
		filename = "/dev/stdin";
	curl_mime_filedata(file_data, filename);
	curl_mime_name(file_data, "file");
	if(paste_flag)
		curl_mime_filename(file_data, "-");
	curl_easy_perform(easy_handle);
	if(!silent_flag)
		putchar('\n');
	puts(buffer);
	curl_mime_free(mime);
	return 0;
}

int
upload_file_scp(int argc, char **argv)
{
	curl_easy_setopt(easy_handle, CURLOPT_SSH_PRIVATE_KEYFILE, ssh_key_path);

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
	fclose(fp);
	return 0;
}
