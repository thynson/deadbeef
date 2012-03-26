if HAVE_MEDIA_KEY
pkglib_LTLIBRARIES = media_key.la
media_key_la_SOURCES = media_key.c
media_key_la_LDFLAGS = -module -lglib-2.0
media_key_la_LIBADD = $(LDADD) $(MEDIA_KEY_LIBS)
AM_CFLAGS=$(CFLAGS) $(MEDIA_KEY_CFLAGS) -std=c99
endif
