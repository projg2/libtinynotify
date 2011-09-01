/* libtinynotify -- session interface
 * (c) 2011 Michał Górny
 * 2-clause BSD-licensed
 */

#pragma once
#ifndef _TINYNOTIFY_ERROR_H
#define _TINYNOTIFY_ERROR_H

/**
 * SECTION: NotifyError
 * @short_description: API to deal with libtinynotify errors
 * @include: tinynotify.h
 *
 * All actual error handling in libtinynotify is done within the bounds of
 * a #NotifySession. Each of the libtinynotify functions (except for
 * notify_session_new(), notify_session_free() and error grabbing funcs) store
 * their results and any additional error details in the corresponding
 * #NotifySession.
 *
 * After a call to such a function, one may get its error status using
 * notify_session_get_error(). If a function returns #NotifyError, then it is
 * guaranteed that the return value is equal to the result of calling
 * notify_session_get_error() immediately after the function.
 *
 * One may assume that %NOTIFY_ERROR_NO_ERROR will always evaluate to false.
 * Thus, #NotifyError can be used as a boolean result as well.
 *
 * A more detailed error description can be obtained using
 * notify_session_get_error_message(). It can contain additional details not
 * available via #NotifyError like backend error messages.
 *
 * The libtinynotify functions don't check for existing error conditions. It is
 * unnecessary to reset the error within #NotifySession (and thus there is no
 * function implementing such a thing). Calling another function will implicitly
 * reset the current error status, and replace with its own result.
 */

/**
 * NotifyError
 *
 * A tinynotify error code.
 *
 * Note that %NOTIFY_ERROR_NO_ERROR is guaranteed to be always false. Thus, one
 * can use this type as a boolean for error indication.
 */

struct notify_error {
	const char* const message;
};

typedef const struct notify_error* NotifyError;

/**
 * NOTIFY_ERROR_NO_ERROR
 *
 * A constant denoting that no error occured.
 */
extern const NotifyError NOTIFY_ERROR_NO_ERROR;

/**
 * NOTIFY_ERROR_DBUS_CONNECT
 *
 * An error occuring while establishing the D-Bus connection.
 */
extern const NotifyError NOTIFY_ERROR_DBUS_CONNECT;
/**
 * NOTIFY_ERROR_DBUS_SEND
 *
 * An error occuring while trying to send the D-Bus message.
 */
extern const NotifyError NOTIFY_ERROR_DBUS_SEND;
/**
 * NOTIFY_ERROR_INVALID_REPLY
 *
 * An error denoting that the return value from a D-Bus method call is invalid.
 */
extern const NotifyError NOTIFY_ERROR_INVALID_REPLY;
/**
 * NOTIFY_ERROR_NO_NOTIFICATION_ID
 *
 * An error denoting that the #Notification has no ID associated while it is
 * necessary for the function to proceed (e.g. when using notification_close()
 * on an unsubmitted notification).
 */
extern const NotifyError NOTIFY_ERROR_NO_NOTIFICATION_ID;

#endif /*_TINYNOTIFY_ERROR_H*/
