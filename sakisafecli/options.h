#pragma once
/* clainsafecli options */

/* Default server you'll upload files to */
char *server                   = "https://lainsafe.kalli.st";

/* proxy urls, socks and http. in that order, by default they're
 * configured to be used for tor and i2p, but if you have another
 * socks/http proxy, you can set it here. 
 */

int socks_proxy_port = 9050; /* Tor */
int http_proxy_port  = 4444; /* I2P */

/* Enable "history" files and where to store that file */

char history_file_path[256];
const int enable_links_history = 1;
const char *path               = ".cache/lainsafelinks";

