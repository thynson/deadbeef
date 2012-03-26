if HAVE_GNOME_MEDIA_KEY
pkglib_LTLIBRARIES = gnome_media_key.la
gnome_media_key_la_SOURCES = gnome_media_key.c
gnome_media_key_la_LDFLAGS = -module -lglib-2.0
gnome_media_key_la_LIBADD = $(LDADD) $(GNOME_MEDIA_KEY_LIBS)
AM_CFLAGS=$(CFLAGS) $(GNOME_MEDIA_KEY_CFLAGS) -std=c99
endif
