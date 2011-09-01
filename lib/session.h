/* libtinynotify -- session interface
 * (c) 2011 Michał Górny
 * 2-clause BSD-licensed
 */

#pragma once
#ifndef _TINYNOTIFY_SESSION_H
#define _TINYNOTIFY_SESSION_H

/**
 * SECTION: NotifySession
 * @short_description: API to deal with libtinynotify sessions
 * @include: tinynotify.h
 *
 * All interaction with libtinynotify is associated with a single session,
 * represented by the #NotifySession type. Before calling any of the library
 * functions, one must instantiate a new session using notify_session_new().
 * When done with it, one should free the session using notify_session_free().
 *
 * One must not pass custom, invalid or freed #NotifySession to any of
 * the functions. Otherwise, the results are unpredictable (and a segfault is
 * the most harmless of them). One must not attempt to modify or manually free
 * a #NotifySession instance, and one shall not assume anything specific about
 * its actual type. When in question, one should pass a pointer to it rather
 * than casting #NotifySession to one.
 */

/**
 * NotifySession
 *
 * A type describing a basic tinynotify session. It holds the data necessary for
 * the notifications to be sent, like the D-Bus connection. It also provides
 * a storage for errors.
 *
 * It should be created using notify_session_new(), and disposed using
 * notify_session_free().
 */

typedef struct _notify_session* NotifySession;

/**
 * notify_session_new
 * @app_name: default application name for the session
 * @app_icon: default application icon for the session
 *
 * Create and initialize a new libtinynotify session. Sets the default
 * @app_name (unless %NOTIFY_SESSION_NO_APP_NAME) and @app_icon (unless
 * %NOTIFY_SESSION_NO_APP_ICON).
 *
 * This function always succeeds. If it is unable to allocate the memory,
 * program execution will be aborted.
 *
 * Returns: a newly-instantiated NotifySession
 */
NotifySession notify_session_new(const char* app_name, const char* app_icon);

/**
 * notify_session_free
 * @session: the session to free
 *
 * Free a libtinynotify session. This handles disconnecting and other cleanup
 * as well.
 *
 * This function always succeeds. After a call to this function,
 * a #NotifySession is no longer valid.
 */
void notify_session_free(NotifySession session);

/**
 * notify_session_get_error
 * @session: session to operate on
 *
 * Get current error for @session.
 *
 * Returns: positive #NotifyError or %NOTIFY_ERROR_NO_ERROR if no error
 */
NotifyError notify_session_get_error(NotifySession session);

/**
 * notify_session_get_error_message
 * @session: session to operate on
 *
 * Get detailed error message for @session.
 *
 * Returns: a statically allocated or constant string (not to be freed)
 */
const char* notify_session_get_error_message(NotifySession session);

/**
 * notify_session_set_error
 * @session: session to operate on
 * @new_error: new error code
 * @...: additional arguments for error message format string
 *
 * Set a new error in session @session.
 *
 * Note: this function is mostly intended for internal use in submodules.
 *
 * Returns: same value as @new_error, for convenience.
 */
NotifyError notify_session_set_error(NotifySession session, NotifyError new_error, ...);

/**
 * notify_session_disconnect
 * @session: session to operate on
 *
 * Drop the connection to the D-Bus session bus.
 *
 * Note that calling this function is not obligatory. It will be called
 * by notify_session_free() anyway.
 *
 * If no connection is established already, this function does nothing.
 */
void notify_session_disconnect(NotifySession session);

/**
 * notify_session_connect
 * @session: session to operate on
 *
 * Establish a connection to the D-Bus session bus.
 *
 * Note that calling this function is not obligatory. If not used,
 * the connection will be established when sending the first notification.
 *
 * If a connection is established already, this function does nothing
 * and returns %NOTIFY_ERROR_NO_ERROR. If the connection was established
 * and got disconnected for some reason (e.g. by the remote end), this function
 * will try to re-establish it transparently.
 *
 * Returns: a #NotifyError or %NOTIFY_ERROR_NO_ERROR if connection succeeds.
 * For additional error details, see notify_session_get_error_message().
 */
NotifyError notify_session_connect(NotifySession session);

/**
 * NOTIFY_SESSION_NO_APP_NAME
 *
 * A constant specifying that no default app name is to be specified.
 */
extern const char* const NOTIFY_SESSION_NO_APP_NAME;

/**
 * notify_session_set_app_name
 * @session: session to operate on
 * @app_name: a new app name
 *
 * Set the default application name for notifications sent through this session.
 *
 * This should be the formal application name rather than an ID.
 *
 * If %NOTIFY_SESSION_NO_APP_NAME is passed, the default application name will
 * be cleared. Otherwise, the given string will be copied to #NotifySession.
 */
void notify_session_set_app_name(NotifySession session, const char* app_name);

/**
 * NOTIFY_SESSION_NO_APP_ICON
 *
 * A constant specifying that no default app icon is to be specified.
 */
extern const char* const NOTIFY_SESSION_NO_APP_ICON;

/**
 * notify_session_set_app_icon
 * @session: session to operate on
 * @app_icon: a new icon name
 *
 * Set the default application icon for notifications sent through this
 * session.
 *
 * The value should be either a name in freedesktop.org-compliant icon scheme,
 * or a file:// URI.
 *
 * If %NOTIFY_SESSION_NO_APP_ICON is passed, the default application icon will
 * be cleared. Otherwise, the given string will be copied to #NotifySession.
 */
void notify_session_set_app_icon(NotifySession session, const char* app_icon);

#endif /*_TINYNOTIFY_SESSION_H*/
