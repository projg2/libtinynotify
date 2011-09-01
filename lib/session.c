/* libtinynotify -- session interface
 * (c) 2011 Michał Górny
 * 2-clause BSD-licensed
 */

#include "config.h"
#include "common_.h"
#include "session_.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef HAVE_LIBSTRL
#	include <strl.h>
#endif

NotifySession notify_session_new(const char* app_name, const char* app_icon) {
	NotifySession s;

	_mem_assert(s = malloc(sizeof(*s)));
	s->conn = NULL;
	s->app_name = NULL;
	s->app_icon = NULL;
	s->error_details = NULL;
	s->notifications = NULL;

	notify_session_set_error(s, NOTIFY_ERROR_NO_ERROR);
	notify_session_set_app_name(s, app_name);
	notify_session_set_app_icon(s, app_icon);
	return s;
}

void notify_session_free(NotifySession s) {
	notify_session_disconnect(s);
	assert(!s->notifications);

	if (s->error_details)
		free(s->error_details);
	free(s->app_name);
	free(s->app_icon);
	free(s);
}

NotifyError notify_session_get_error(NotifySession s) {
	return s->error;
}

const char* notify_session_get_error_message(NotifySession s) {
	return s->error_details;
}

NotifyError notify_session_set_error(NotifySession s, NotifyError new_error, ...)
{
	va_list ap;

	if (s->error_details)
		free(s->error_details);
	s->error = new_error;
	if (!new_error)
		_mem_assert(s->error_details = strdup("No error"));
	else {
		va_start(ap, new_error);
		_mem_assert(vasprintf(&s->error_details, new_error->message, ap) != -1);
		va_end(ap);
	}

	return new_error;
}
