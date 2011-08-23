AC_PREREQ([2.60])
AC_INIT([libtinynotify], [0.0.1])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.6 foreign dist-bzip2 no-dependencies -Wall])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES])

AC_PROG_CC_STDC
AM_PROG_CC_C_O
AC_USE_SYSTEM_EXTENSIONS
LT_INIT([disable-static])
GTK_DOC_CHECK([1.15])

AC_CHECK_FUNCS([strdup snprintf],, [
	AC_MSG_ERROR([One of the required library functions can not be found])
])
AC_SEARCH_LIBS([vasprintf], [strl],, [
	AC_MSG_ERROR([One of the required library functions can not be found.
You may install libstrl (http://ohnopub.net/~ohnobinki/libstrl/) to get a compat.])
])

AS_IF([test x"$ac_cv_search_vasprintf" = x"-lstrl"], [
	AC_DEFINE([HAVE_LIBSTRL], [1], [Define if we need to use libstrl])
])

PKG_CHECK_MODULES([DBUS], [dbus-1])

AC_ARG_ENABLE([debug],
	[AS_HELP_STRING([--disable-debug],
		[Disable debugging asserts])])
AS_IF([test x"$enable_debug" = x"no"], [
	AC_DEFINE([NDEBUG], [1], [Set this to disable debugging asserts])
])

AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile doc/Makefile libtinynotify.pc doc/example-mailclient.xml])
AC_OUTPUT
