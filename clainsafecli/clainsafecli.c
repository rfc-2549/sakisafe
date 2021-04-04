#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <curl/curl.h>

#include "options.h"
#include "clainsafecli.h"

int
main(int argc, char **argv)
{
	struct curl_httppost *post = NULL;
	struct curl_httppost *last = NULL;

	bool tor_flag, i2p_flag;
	tor_flag = i2p_flag = false;
	bool ipv6_flag, ipv4_flag;
	ipv6_flag = ipv4_flag = false;
	char *token = NULL;
	long silent_flag = 0L;
	char *buffer = (char *)calloc(1024,sizeof(char));
	if(buffer == NULL) {
		fprintf(stderr,"Error allocating memory!\n");
		return -1;
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
		{"token" ,required_argument,0,'T'},
		{"i2p"   ,no_argument      ,0,'i'},
		{"silent",no_argument      ,0,'S'},
		{"ipv4"  ,no_argument      ,0,'4'},
		{"ipv6"  ,no_argument      ,0,'6'},
		{0       ,0                ,0, 0 }

	};

	int c = 0;
	while((c = getopt_long(argc,argv, "46hT:tiSs:",
				long_options,&option_index)) != -1) {
		switch(c) {
		case 's':
			server = optarg;
			break;
		case 'h':
			print_help();
			return 0;
			break;
		case 't':
			tor_flag = true;
			break;
		case 'i':
			i2p_flag = true;
			break;
		case 'S':
			silent_flag = true;
			break;
		case 'T':
			token = optarg;
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

	if(access(argv[optind],F_OK)) {
		fprintf(stderr,"Error opening file\n");
		return -1;
	}

	/* curl options */
	curl_easy_setopt(easy_handle,CURLOPT_WRITEFUNCTION, write_data);
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

	/* Which address to use */
	
	if(ipv6_flag)
		curl_easy_setopt(easy_handle,CURLOPT_IPRESOLVE,
			CURL_IPRESOLVE_V6);
	else if(ipv4_flag)
		curl_easy_setopt(easy_handle,CURLOPT_IPRESOLVE,
			CURL_IPRESOLVE_V4);
	else
		curl_easy_setopt(easy_handle,CURLOPT_IPRESOLVE,
			CURL_IPRESOLVE_WHATEVER);
	
	/* Form parameters */

	/* File name */

	/* TODO: make it iterate on args so you can upload multiple files
	 *  at once (clainsafecli file1 file2 ... filen)
	 */
	for(int i = optind; i<argc; i++) {
		curl_formadd(&post,&last,
			CURLFORM_COPYNAME,"file",
			CURLFORM_FILE,argv[i],
			CURLFORM_END);
		/* Actual file content */
		curl_formadd(&post,&last,
			CURLFORM_COPYNAME,"file",
			CURLFORM_COPYCONTENTS,argv[i],
			CURLFORM_END);
		if(token)
			curl_formadd(&post,&last,
				CURLFORM_COPYNAME,"token",
				CURLFORM_COPYCONTENTS,token,
				CURLFORM_END);

		curl_easy_setopt(easy_handle,CURLOPT_NOPROGRESS,silent_flag);
		curl_easy_setopt(easy_handle,CURLOPT_PROGRESSFUNCTION,progress);

		curl_easy_setopt(easy_handle,CURLOPT_HTTPPOST,post);

		curl_easy_perform(easy_handle);
		if(!silent_flag) puts("");

		puts(buffer);
	}
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

