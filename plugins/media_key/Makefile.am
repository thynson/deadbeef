if HAVE_MEDIA_KEY
pkglib_LTLIBRARIES = media_key.la
media_key_la_SOURCES = media_key.c
media_key_la_LDFLAGS = -module
media_key_la_LIBADD = $(LDADD) $(NOTIFY_LIBS)
AM_CFLAGS = $(CFLAGS) $(NOTIFY_CFLAGS) -std=c99
endif
