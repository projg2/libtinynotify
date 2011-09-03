/* libtinynotify -- event-based API
 * (c) 2011 Michał Górny
 * 2-clause BSD-licensed
 */

#include "config.h"

#include "error.h"
#include "session.h"
#include "notification.h"
#include "event.h"

#include "session_.h"
#include "notification_.h"
#include "event_.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <dbus/dbus.h>

struct _notify_dispatch_status {
	int dummy;
};

const NotificationCloseReason NOTIFICATION_CLOSED_BY_DISCONNECT = 'D';
const NotificationCloseReason NOTIFICATION_CLOSED_BY_EXPIRATION = 'E';
const NotificationCloseReason NOTIFICATION_CLOSED_BY_USER = 'U';
const NotificationCloseReason NOTIFICATION_CLOSED_BY_CALLER = 'C';

const NotifyDispatchStatus NOTIFY_DISPATCH_DONE = 0;
const NotifyDispatchStatus NOTIFY_DISPATCH_ALL_CLOSED = 1;
const NotifyDispatchStatus NOTIFY_DISPATCH_NOT_CONNECTED = 2;

const int NOTIFY_SESSION_NO_TIMEOUT = -1;

static void _notification_noop_on_close(Notification n, NotificationCloseReason r, void* user_data) {
}

static void _notification_free_on_close(Notification n, NotificationCloseReason r, void* user_data) {
	notification_free(n);
}

const NotificationCloseCallback NOTIFICATION_NO_CLOSE_CALLBACK = NULL;
const NotificationCloseCallback NOTIFICATION_NOOP_ON_CLOSE = _notification_noop_on_close;
const NotificationCloseCallback NOTIFICATION_FREE_ON_CLOSE = _notification_free_on_close;

void _notification_event_init(Notification n) {
	notification_bind_close_callback(n, NOTIFICATION_NO_CLOSE_CALLBACK, NULL);
}

void _emit_closed(Notification n, NotificationCloseReason reason) {
	if (n->close_callback)
		n->close_callback(n, reason, n->close_data);
}

static void _notify_session_handle_message(DBusMessage *msg, NotifySession s) {
	assert(dbus_message_get_type(msg) == DBUS_MESSAGE_TYPE_SIGNAL);
	assert(!strcmp(dbus_message_get_interface(msg),
				"org.freedesktop.Notifications"));

	if (!strcmp(dbus_message_get_member(msg), "NotificationClosed")) {
		DBusError err;
		dbus_uint32_t id, reason;

		dbus_error_init(&err);
		if (!dbus_message_get_args(msg, &err,
				DBUS_TYPE_UINT32, &id,
				DBUS_TYPE_UINT32, &reason,
				DBUS_TYPE_INVALID)) {
			/* XXX: error handling? */
			dbus_error_free(&err);
		} else {
			struct _notification_list *nl;

			for (nl = s->notifications; nl; nl = nl->next) {
				if (nl->n->message_id == id) {
					NotificationCloseReason r;

					switch (reason) {
						case 1:
							r = NOTIFICATION_CLOSED_BY_EXPIRATION;
							break;
						case 2:
							r = NOTIFICATION_CLOSED_BY_USER;
							break;
						case 3:
							r = NOTIFICATION_CLOSED_BY_CALLER;
							break;
						default:
							r = 0;
					}

					_emit_closed(nl->n, r);
					_notify_session_remove_notification(s, nl->n);
					break;
				}
			}
		}
	} else
		assert(!"reached when invalid signal is received");
}

void notification_bind_close_callback(Notification n,
		NotificationCloseCallback callback, void* user_data) {
	n->close_callback = callback;
	n->close_data = user_data;
}

NotifyDispatchStatus notify_session_dispatch(NotifySession s, int timeout) {
	DBusMessage *msg;

	if (s->conn && !dbus_connection_get_is_connected(s->conn))
		notify_session_disconnect(s);
	if (!s->conn)
		return NOTIFY_DISPATCH_NOT_CONNECTED;

	dbus_connection_read_write_dispatch(s->conn, timeout);
	while ((msg = dbus_connection_pop_message(s->conn)))
		_notify_session_handle_message(msg, s);

	if (s->notifications)
		return NOTIFY_DISPATCH_DONE;
	else
		return NOTIFY_DISPATCH_ALL_CLOSED;
}
