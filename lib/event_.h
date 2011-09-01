/* libtinynotify -- event-based API
 * (c) 2011 Michał Górny
 * 2-clause BSD-licensed
 */

#pragma once
#ifndef _TINYNOTIFY_EVENT__H
#define _TINYNOTIFY_EVENT__H

void _notification_event_init(Notification n);
void _emit_closed(Notification n, NotificationCloseReason reason);

#endif /*_TINYNOTIFY_EVENT__H*/
