#ifndef OPTIONS_H
#define OPTIONS_H

#include <libconfig.h>
/* clainsafecli options */

/* Default server you'll upload files to */
extern char *server;

/* proxy urls, socks and http. in that order, by default they're
 * configured to be used for tor and i2p, but if you have another
 * socks/http proxy, you can set it here. 
 */

/* Enable "history" files and where to store that file */

extern char history_file_path[256];
extern const int enable_links_history;
extern const char *path;

/* Config file variables */
extern char *socks_proxy_url, *http_proxy_url;

extern bool socks_proxy_flag;
extern bool http_proxy_flag;
extern bool ipv6_flag;
extern bool ipv4_flag;
extern bool silent_flag;
extern config_t runtime_config;
extern char *ssh_key_path;
#endif  /* OPTIONS_H */
