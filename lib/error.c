/* libtinynotify -- session interface
 * (c) 2011 Michał Górny
 * 2-clause BSD-licensed
 */

#include "config.h"
#include "error.h"

#include <stdlib.h>

const NotifyError NOTIFY_ERROR_NO_ERROR = NULL;

static const struct notify_error _error_dbus_connect = { "Connecting to D-Bus failed: %s" };
const NotifyError NOTIFY_ERROR_DBUS_CONNECT = &_error_dbus_connect;
static const struct notify_error _error_dbus_send = { "Sending message over D-Bus failed: %s" };
const NotifyError NOTIFY_ERROR_DBUS_SEND = &_error_dbus_send;
static const struct notify_error _error_invalid_reply = { "Invalid reply received: %s" };
const NotifyError NOTIFY_ERROR_INVALID_REPLY = &_error_invalid_reply;
static const struct notify_error _error_no_notification_id = { "No notification-id is specified" };
const NotifyError NOTIFY_ERROR_NO_NOTIFICATION_ID = &_error_no_notification_id;
