#ifndef SAKISAFECLI_H
#define SAKISAFECLI_H
#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>

struct progress
{
	char *_private;
	size_t size;
};

size_t
write_data(void *buffer, size_t size, size_t nmemb, void *userp);

void
print_usage();

int
store_link(const char *path, const char *buf);

void
print_help();

void
parse_config_file(FILE *config);
int
get_protocol(char *server);

#endif /* SAKISAFECLI_H */
