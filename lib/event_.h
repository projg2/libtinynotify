/* libtinynotify -- event-based API
 * (c) 2011 Michał Górny
 * 2-clause BSD-licensed
 */

#pragma once
#ifndef _TINYNOTIFY_EVENT__H
#define _TINYNOTIFY_EVENT__H

/*<private_header>*/
#pragma GCC visibility push(hidden)

struct _notification_action_list {
	char* key;

	char* desc;
	NotificationActionCallback callback;
	void* callback_data;

	struct _notification_action_list* next;
};

void _notification_event_init(Notification n);
void _notification_event_free(Notification n);

void _emit_closed(Notification n, NotificationCloseReason reason);

#pragma GCC visibility pop
#endif /*_TINYNOTIFY_EVENT__H*/
