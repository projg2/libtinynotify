/* libtinynotify -- event-based API
 * (c) 2011 Michał Górny
 * 2-clause BSD-licensed
 */

#pragma once
#ifndef _TINYNOTIFY_EVENT__H
#define _TINYNOTIFY_EVENT__H

#pragma GCC visibility push(hidden)

void _notification_event_init(Notification n);
void _emit_closed(Notification n, NotificationCloseReason reason);

#pragma GCC visibility pop
#endif /*_TINYNOTIFY_EVENT__H*/
