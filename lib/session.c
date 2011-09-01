/* libtinynotify -- session interface
 * (c) 2011 Michał Górny
 * 2-clause BSD-licensed
 */

#include "config.h"

#include "error.h"
#include "session.h"
#include "notification.h"
#include "event.h"

#include "common_.h"
#include "session_.h"
#include "notification_.h"
#include "event_.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef HAVE_LIBSTRL
#	include <strl.h>
#endif

const char* const NOTIFY_SESSION_NO_APP_NAME = NULL;
const char* const NOTIFY_SESSION_NO_APP_ICON = NULL;

void _notify_session_add_notification(NotifySession s, Notification n) {
	struct _notification_list *nl;

	/* add the notification only when actually useful */
	if (!n->close_callback) {
		/* XXX: drop it from list if already there */
		return;
	}

	for (nl = s->notifications; nl; nl = nl->next) {
		/* XXX: maybe we should send some kind of close(reason = replaced)? */
		if (nl->n == n)
			return;
	}

	if (!s->notifications) { /* add the filter */
		DBusError err;

		dbus_error_init(&err);
		dbus_bus_add_match(s->conn, "type='signal',"
				"interface='org.freedesktop.Notifications',"
				"member='NotificationClosed'", &err);
		_mem_assert(!dbus_error_is_set(&err));
	}

	_mem_assert(nl = malloc(sizeof(*nl)));
	nl->n = n;
	nl->next = s->notifications;
	s->notifications = nl;
}

void _notify_session_remove_notification(NotifySession s, Notification n) {
	struct _notification_list **prev;

	for (prev = &s->notifications; *prev; prev = &(*prev)->next) {
		struct _notification_list *n_l = *prev;

		if (n_l->n == n) {
			*prev = n_l->next;
			free(n_l);
			return;
		}
	}

	assert("reached if _notify_session_remove_notification() fails to find the notification");
}

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

NotifyError notify_session_connect(NotifySession s) {
	if (s->conn && !dbus_connection_get_is_connected(s->conn))
		notify_session_disconnect(s);

	if (!s->conn) {
		DBusError err;

		dbus_error_init(&err);
		s->conn = dbus_bus_get_private(DBUS_BUS_SESSION, &err);

		assert(!s->conn == dbus_error_is_set(&err));
		if (!s->conn) {
			char *err_msg = strdup(err.message);
			dbus_error_free(&err);
			return notify_session_set_error(s, NOTIFY_ERROR_DBUS_CONNECT, err_msg);
		} else
			dbus_connection_set_exit_on_disconnect(s->conn, FALSE);
	}

	return notify_session_set_error(s, NOTIFY_ERROR_NO_ERROR);
}

void notify_session_disconnect(NotifySession s) {
	if (s->conn) {
		struct _notification_list *nl;
		struct _notification_list *next;

		for (nl = s->notifications; nl; nl = next) {
			next = nl->next;
			_emit_closed(nl->n, NOTIFICATION_CLOSED_BY_DISCONNECT);
			free(nl);
		}
		s->notifications = NULL;

		dbus_connection_close(s->conn);
		dbus_connection_unref(s->conn);
		s->conn = NULL;
	}

	notify_session_set_error(s, NOTIFY_ERROR_NO_ERROR);
}

void notify_session_set_app_name(NotifySession s, const char* app_name) {
	_property_assign_str(&s->app_name, app_name);
}

void notify_session_set_app_icon(NotifySession s, const char* app_icon) {
	_property_assign_str(&s->app_icon, app_icon);
}
