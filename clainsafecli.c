#include <stdio.h>
#include <unistd.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

size_t static write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
     memcpy(userp, buffer, nmemb*size);
     return 0;
}

int
main(int argc, char **argv)
{
	struct curl_httppost *post=NULL;
	struct curl_httppost *last=NULL;

	char *buffer = (char *)calloc(1024,sizeof(char));
	char server[256] = "https://lainsafe.kalli.st";
	
	CURL *easy_handle = curl_easy_init();
	if(!easy_handle) {
		fprintf(stderr,"Error initializing libcurl\n");
		return -1;
	}
	if(argc == optind) {
		printf("%s server file\n",argv[0]);
		return -1;
	}
	int option_index = 0;

	static struct option long_options[] = {
		{"server",required_argument,0,'s'},
		{0       ,0                ,0, 0 }
	};
	int c = 0;
	while((c = getopt_long(argc,argv, "s:",long_options,&option_index)) != -1) {
		switch(c) {
		case 's':
			strncpy(server,optarg,256);
			break;
		case '?':
			fprintf(stderr,"Unrecognized argument\n");
			return -1;
			break;
		default:
			return -1;
			break;
		}

	}
	curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(easy_handle,CURLOPT_WRITEDATA,buffer);
	curl_easy_setopt(easy_handle,CURLOPT_URL,server);
	
	curl_formadd(&post,&last,
		CURLFORM_COPYNAME, "file",
		CURLFORM_FILE,argv[optind],
		CURLFORM_END);

	curl_formadd(&post,&last,
		CURLFORM_COPYNAME, "file",
		CURLFORM_COPYCONTENTS,argv[optind],
		CURLFORM_END);

	curl_easy_setopt(easy_handle,CURLOPT_HTTPPOST,post);
	
	curl_easy_perform(easy_handle);

	puts(buffer);
	free(buffer);
	curl_formfree(post);
	curl_easy_cleanup(easy_handle);
	
	return 0;
}
