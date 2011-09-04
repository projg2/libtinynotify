ACLOCAL_AMFLAGS = -I m4
DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc

SUBDIRS = doc

pkgconfigdir = $(libdir)/pkgconfig
subincludedir = $(includedir)/tinynotify

lib_LTLIBRARIES = libtinynotify.la
pkgconfig_DATA = libtinynotify.pc
include_HEADERS = lib/tinynotify.h
subinclude_HEADERS = \
	lib/features.h \
	lib/error.h \
	lib/session.h \
	lib/notification.h \
	lib/event.h

libtinynotify_la_LDFLAGS = -version-info 2:0:2
libtinynotify_la_CPPFLAGS = $(DBUS_CFLAGS)
libtinynotify_la_LIBADD = $(DBUS_LIBS)
libtinynotify_la_SOURCES = \
	lib/common.c lib/common_.h \
	lib/error.c \
	lib/session.c lib/session_.h \
	lib/notification.c lib/notification_.h \
	lib/event.c lib/event_.h \
	$(include_HEADERS) $(subinclude_HEADERS)
