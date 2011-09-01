/* libtinynotify
 * (c) 2011 Michał Górny
 * 2-clause BSD-licensed
 */

#include "config.h"
#include "common_.h"
#include "session_.h"
#include "notification_.h"

#include "error.h"
#include "notification.h"
#include "event.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <assert.h>

#include <dbus/dbus.h>

#ifdef HAVE_LIBSTRL
#	include <strl.h>
#endif

static const dbus_uint32_t NOTIFICATION_NO_NOTIFICATION_ID = 0;

const NotificationCloseReason NOTIFICATION_CLOSED_BY_DISCONNECT = 'D';
const NotificationCloseReason NOTIFICATION_CLOSED_BY_EXPIRATION = 'E';
const NotificationCloseReason NOTIFICATION_CLOSED_BY_USER = 'U';
const NotificationCloseReason NOTIFICATION_CLOSED_BY_CALLER = 'C';

static void _notification_noop_on_close(Notification n, NotificationCloseReason r, void* user_data) {
}

static void _notification_free_on_close(Notification n, NotificationCloseReason r, void* user_data) {
	notification_free(n);
}

const NotificationCloseCallback NOTIFICATION_NO_CLOSE_CALLBACK = NULL;
const NotificationCloseCallback NOTIFICATION_NOOP_ON_CLOSE = _notification_noop_on_close;
const NotificationCloseCallback NOTIFICATION_FREE_ON_CLOSE = _notification_free_on_close;

void notification_bind_close_callback(Notification n,
		NotificationCloseCallback callback, void* user_data) {
	n->close_callback = callback;
	n->close_data = user_data;
}

struct _notify_dispatch_status {
	int dummy;
};

const NotifyDispatchStatus NOTIFY_DISPATCH_DONE = 0;
const NotifyDispatchStatus NOTIFY_DISPATCH_ALL_CLOSED = 1;
const NotifyDispatchStatus NOTIFY_DISPATCH_NOT_CONNECTED = 2;

const int NOTIFY_SESSION_NO_TIMEOUT = -1;


