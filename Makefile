install: lainsafecli
	cp lainsafecli /usr/bin/
	gzip -k doc/lainsafecli.1
	cp doc/lainsafecli.1.gz /usr/share/man/man1
