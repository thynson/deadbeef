if HAVE_MEDIA_KEY
pkglib_LTLIBRARIES = media_key.la
media_key_la_SOURCES = media_key.c marshal.h marshal.c
marshal.h marshal.c: marshal.list
	$(AM_V_GEN)glib-genmarshal --prefix=marshal --header $< > $@
	$(AM_V_GEN)glib-genmarshal --prefix=marshal --body $< > $@

media_key_la_LDFLAGS = -module
media_key_la_LIBADD = $(LDADD) $(MEDIA_KEY_LIBS)
AM_CFLAGS=$(CFLAGS) $(MEDIA_KEY_CFLAGS) -std=c99
endif
