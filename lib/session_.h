/* libtinynotify -- session interface
 * (c) 2011 Michał Górny
 * 2-clause BSD-licensed
 */

#pragma once
#ifndef _TINYNOTIFY_SESSION__H
#define _TINYNOTIFY_SESSION__H

#include <dbus/dbus.h>

#include "error.h"
#include "session.h"
#include "notification.h"

struct _notification_list {
	Notification n;
	struct _notification_list* next;
};

struct _notify_session {
	DBusConnection *conn;

	char* app_name;
	char* app_icon;

	NotifyError error;
	char* error_details;

	/* notifications with event callbacks */
	struct _notification_list* notifications;
};

void _notify_session_add_notification(NotifySession s, Notification n);
void _notify_session_remove_notification(NotifySession s, Notification n);

#endif /*_TINYNOTIFY_SESSION__H*/
