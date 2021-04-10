DESTDIR=
PREFIX=/usr/local

ddc: ddc.c
	$(CC) -o $@ $<

install:
	install -sD ddc $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm $(DESTDIR)$(PREFIX)/bin/ddc
