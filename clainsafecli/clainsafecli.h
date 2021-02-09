#include <stdlib.h>

size_t
static write_data(void *buffer, size_t size, size_t nmemb,
	void *userp);

void
print_usage();

int progress_func(void* ptr, double TotalToDownload, double NowDownloaded, 
	double TotalToUpload, double NowUploaded);

int
store_link(const char *path, const char *buf);

void
print_help();
