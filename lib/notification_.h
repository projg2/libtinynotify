/* libtinynotify -- Notification instance
 * (c) 2011 Michał Górny
 * 2-clause BSD-licensed
 */

#pragma once
#ifndef _TINYNOTIFY_NOTIFICATION__H
#define _TINYNOTIFY_NOTIFICATION__H

#include <dbus/dbus.h>

#include "notification.h"
#include "event.h"

#pragma GCC visibility push(hidden)

struct _notification {
	char* summary;
	char* body;
	int formatting;

	NotificationCloseCallback close_callback;
	void* close_data;

	dbus_int32_t expire_timeout;
	NotificationUrgency urgency;
	char* category;

	char* app_icon;

	dbus_uint32_t message_id;
};

#pragma GCC visibility pop
#endif /*_TINYNOTIFY_NOTIFICATION__H*/
