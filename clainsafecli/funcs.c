#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "clainsafecli.h"

size_t
write_data(void *buffer, size_t size, size_t nmemb,
	void *userp)
{
	memcpy(userp, buffer, nmemb*size);
	return 0;
}

void
print_usage()
{
	printf("USAGE: clainsafecli [--socks-proxy=socks_address|--http_proxy=proxy_address] [-6|-4] [--server] file\n");
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
	fclose(fp);
	return 0;
}

void
print_help()
{
	printf("--server <server>: specifies the lainsafe server\n%s\n%s\n%s\n%s\n%s\n%s\n%s",
		"-t|--token: Authentication token (https://u.kalli.st)",
		"--tor: uses tor.",
		"--i2p: uses i2p.",
		"-6|--ipv6: uses IPv6 only.",
		"-4|--ipv6: uses IPv4 only.",
		"--silent: doesn't print progress.",
		"--help: print this message.\n");
	return;
}

void
progress(void *clientp,
	double dltotal,
	double dlnow,
	double ultotal,
	double ulnow)
{
	/* So I don't get a warning */
     dltotal += 1;
	dlnow   += 1;
	printf("\r%0.f uploaded of %0.f (\E[32;1m%0.f%%\E[30;0m)",ulnow,ultotal,
		ulnow*100/ultotal);
	fflush(stdout);
	
}

