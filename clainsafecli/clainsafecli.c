#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <curl/curl.h>

#include "options.h"

size_t static write_data(void *buffer, size_t size, size_t nmemb,
	void *userp)
{
	memcpy(userp, buffer, nmemb*size);
	return 0;
}

void
print_usage()
{
	printf("USAGE: clainsafecli [--tor|--i2p] [--server] file\n");
	return;
}

int
store_link(const char *path, const char *buf)
{
	FILE *fp = fopen(path,"a+");
	if(fp == NULL) {
		fprintf(stderr,"Error opening file %i: %s\n",errno,
			strerror(errno));
		return -1;
	}
	fwrite(buf,strlen(buf),1,fp);
	fputc('\n',fp);
	return 0;
}

void
print_help()
{
	printf("--server <server>: specifies the lainsafe server\n%s\n%s\n%s",
		"--tor: uses tor",
		"--help: print this message\n",
		"--i2p: uses i2p HTTP proxy"
		);
	return;
}

int
main(int argc, char **argv)
{
	struct curl_httppost *post = NULL;
	struct curl_httppost *last = NULL;

	int tor_flag, i2p_flag;
	tor_flag = i2p_flag = 0;
	
	char *buffer = (char *)calloc(1024,sizeof(char));
	if(buffer == NULL) {
		fprintf(stderr,"Error allocating memory!\n");
	}
	CURL *easy_handle = curl_easy_init();
	
	if(!easy_handle) {
		fprintf(stderr,"Error initializing libcurl\n");
		return -1;
	}
	if(argc == optind) {
	     print_usage();
		return -1;
	}
	
	int option_index = 0;
	static struct option long_options[] = {
		{"server",required_argument,0,'s'},
		{"help"  ,no_argument      ,0,'h'},
		{"tor"   ,no_argument      ,0,'t'},
		{"i2p"   ,no_argument      ,0,'i'},
		{0       ,0                ,0, 0 }
	};

	int c = 0;
	while((c = getopt_long(argc,argv, "htis:",
				long_options,&option_index)) != -1) {
		switch(c) {
		case 's':
			strncpy(server,optarg,256);
			break;
		case 'h':
			print_help();
			return 0;
			break;
		case 't':
			tor_flag = 1;
			break;
		case 'i':
			i2p_flag = 1;
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

	if(access(argv[optind],F_OK)) {
		fprintf(stderr,"Error opening file\n");
		return -1;
	}
	
	/* curl options */
	curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(easy_handle,CURLOPT_WRITEDATA,buffer);
	curl_easy_setopt(easy_handle,CURLOPT_URL,server);

	/* Proxy options */
	
	if(tor_flag && i2p_flag) {
		fprintf(stderr,"Tor and I2P can't be used at once\n");
		return -1;
	} else if(tor_flag) {
		curl_easy_setopt(easy_handle,CURLOPT_PROXY,tor_proxy_url);
		curl_easy_setopt(easy_handle,CURLOPT_PROXYTYPE,
			CURLPROXY_SOCKS5_HOSTNAME);
	} else if(i2p_flag) {
		curl_easy_setopt(easy_handle,CURLOPT_PROXY,i2p_proxy_url);
		curl_easy_setopt(easy_handle,CURLOPT_PROXYTYPE,
			CURLPROXY_HTTP);
	}

	/* Form parameters */

	/* File name */
	curl_formadd(&post,&last,
		CURLFORM_COPYNAME, "file",
		CURLFORM_FILE,argv[optind],
		CURLFORM_END);
	/* Actual file content */
	curl_formadd(&post,&last,
		CURLFORM_COPYNAME, "file",
		CURLFORM_COPYCONTENTS,argv[optind],
		CURLFORM_END);

	curl_easy_setopt(easy_handle,CURLOPT_HTTPPOST,post);
	
	curl_easy_perform(easy_handle);

	puts(buffer);
	
	curl_formfree(post);
	curl_easy_cleanup(easy_handle);

	/* Store link if needed */

	if(enable_links_history) {
		snprintf(history_file_path,256,"%s/%s",getenv("HOME"),path);
		store_link(history_file_path,buffer);
	}
	free(buffer);
	return 0;
}
