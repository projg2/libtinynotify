/* libtinynotify -- event-based API
 * (c) 2011 Michał Górny
 * 2-clause BSD-licensed
 */

#pragma once
#ifndef _TINYNOTIFY_EVENT_H
#define _TINYNOTIFY_EVENT_H

/**
 * SECTION: NotifyEvent
 * @short_description: extended, event-based API
 * @include: tinynotify.h
 *
 * A more advanced functionality of libtinynotify is provided via the so-called
 * event API.
 *
 * The core of the event API are callbacks bound to #Notification -specific
 * events. When a notification with at least a single callback bound is send
 * through a particular #NotifySession, the notification becomes associated to
 * that session and it must not be freed before it is closed.
 *
 * Thus, one must either bind to the close event explicitly and use a callback
 * to free a #Notification afterwards, or ensure to disconnect the associated
 * #NotifySession first (which guarantees sending the close event).
 */

/**
 * NotificationCloseReason
 *
 * A reason for which the notification was closed.
 */

typedef unsigned char NotificationCloseReason;

/**
 * NOTIFICATION_CLOSED_BY_DISCONNECT
 *
 * A constant passed to #NotificationCloseCallback when the close event is
 * emitted because of the #NotifySession being disconnected.
 *
 * Note that this doesn't actually mean the notification was closed. It just
 * means that libtinynotify didn't receive a NotificationClosed signal before
 * the connection was interrupted. The notification may still be open, or be
 * long gone (if daemon failed to send the signal).
 */
extern const NotificationCloseReason NOTIFICATION_CLOSED_BY_DISCONNECT;

/**
 * NOTIFICATION_CLOSED_BY_EXPIRATION
 *
 * A constant passed to #NotificationCloseCallback when the notification was
 * closed because of the expiration timeout.
 */
extern const NotificationCloseReason NOTIFICATION_CLOSED_BY_EXPIRATION;

/**
 * NOTIFICATION_CLOSED_BY_USER
 *
 * A constant passed to #NotificationCloseCallback when the notification was
 * closed because of the user action.
 */
extern const NotificationCloseReason NOTIFICATION_CLOSED_BY_USER;

/**
 * NOTIFICATION_CLOSED_BY_CALLER
 *
 * A constant passed to #NotificationCloseCallback when the notification was
 * closed by a call to notification_close().
 */
extern const NotificationCloseReason NOTIFICATION_CLOSED_BY_CALLER;

/**
 * NotificationCloseCallback
 * @notification: the notification which was closed
 * @close_reason: reason for which the notification was closed
 * @user_data: additional user data pointer as passed
 *	to notification_bind_close_callback()
 *
 * The callback for notification closed event. It is invoked once and only once
 * per a single notification_send() call.
 *
 * After this event, no more events can be sent from the particular
 * #Notification until notification_send() or notification_update() is used.
 * Thus, this event is a good place to inject a simple GC.
 *
 * Note: the @close_reason parameter may contain a value not listed in constants
 * in this section. If it does so, one should assume the reason is unknown.
 */
typedef void (*NotificationCloseCallback)(Notification notification,
		NotificationCloseReason close_reason, void* user_data);

/**
 * NOTIFICATION_NOOP_ON_CLOSE
 *
 * A dummy callback function for notification_bind_close_callback(). It may be
 * used if one isn't interested in the notification being closed itself but
 * just wants notify_session_dispatch() to block until all notifications are
 * closed.
 */
extern const NotificationCloseCallback NOTIFICATION_NOOP_ON_CLOSE;

/**
 * NOTIFICATION_FREE_ON_CLOSE
 *
 * A callback function for notification_bind_close_callback() which frees
 * #Notification as soon as it's closed.
 *
 * Note: when using this callback, one must not use the same #Notification
 * after sending it for the first time as it will become invalid at a random
 * point in time.
 */
extern const NotificationCloseCallback NOTIFICATION_FREE_ON_CLOSE;

/**
 * NOTIFICATION_NO_CLOSE_CALLBACK
 *
 * A constant used to disable close callback.
 */
extern const NotificationCloseCallback NOTIFICATION_NO_CLOSE_CALLBACK;

/**
 * notification_bind_close_callback
 * @notification: notification to operate on
 * @callback: new callback function, or %NOTIFICATION_NO_CLOSE_CALLBACK
 *	to disable
 * @user_data: additional user data to pass to the callback
 *
 * Bind a callback function which will be executed when notification is closed,
 * or remove a current binding (when @callback is
 * %NOTIFICATION_NO_CLOSE_CALLBACK).
 *
 * A few standard callbacks are provided as well:
 * - %NOTIFICATION_NOOP_ON_CLOSE,
 * - %NOTIFICATION_FREE_ON_CLOSE.
 */
void notification_bind_close_callback(Notification notification,
		NotificationCloseCallback callback, void* user_data);

/**
 * NotificationActionCallback
 * @notification: the notification which was closed
 * @key: the key for invoked action
 * @user_data: additional user data pointer as passed
 *	to notification_bind_action()
 *
 * The callback for invoked action.
 */
typedef void (*NotificationActionCallback)(Notification notification,
		const char* key, void* user_data);

/**
 * NOTIFICATION_DEFAULT_ACTION
 *
 * A special (reserved) action key which makes the passed action a default one.
 * It corresponds to the `default` key reserved by the protocol.
 *
 * Note that the default action may be not be displayed as a regular action
 * (i.e. with its description).
 */
extern const char* const NOTIFICATION_DEFAULT_ACTION;

/**
 * NOTIFICATION_NO_ACTION
 *
 * A constant for callback used to disable/remove action.
 */
extern const NotificationActionCallback NOTIFICATION_NO_ACTION;

/**
 * notification_bind_action
 * @notification: notification to operate on
 * @key: a key for the new (or replaced) action, or %NOTIFICATION_DEFAULT_ACTION
 * @callback: a callback function, or %NOTIFICATION_NO_ACTION
 * @user_data: user data to pass to the callback
 * @description: human-readable action description
 *
 * Add an action to the notification and bind a callback function for it.
 * The callback function will be executed whenever user invokes the particular
 * action. Note that it may be called multiple times.
 *
 * The key must be unique per action. Special value %NOTIFICATION_DEFAULT_ACTION
 * corresponds to the default action.
 *
 * Actions are sent to server in the order of adding them. If one calls
 * notification_bind_action() again with the same key, the previous action will
 * be replaced by the new one without changing its position.
 *
 * If one calls notification_bind_action() with @callback ==
 * %NOTIFICATION_NO_ACTION, the existing action will be removed (if exists;
 * otherwise nothing will happen). Afterwards, adding the same action again will
 * move it to the end of action list.
 *
 * If @description is %NULL, it will default to @key. This is usually not a good
 * idea.
 */
void notification_bind_action(Notification notification,
		const char* key, NotificationActionCallback callback,
		void* user_data, const char* description);

/**
 * NotifyDispatchStatus
 *
 * A return value from notify_session_dispatch().
 *
 * Note that the %NOTIFY_DISPATCH_DONE constant is guaranteed always
 * to evaluate to a false value. Thus, one can use #NotifyDispatchStatus
 * as a boolean.
 */
typedef const int NotifyDispatchStatus;

/**
 * NOTIFY_DISPATCH_DONE
 *
 * A constant denoting that the notify_session_dispatch() completed
 * successfully, and there may be more messages to dispatch in the future.
 */
extern const NotifyDispatchStatus NOTIFY_DISPATCH_DONE;

/**
 * NOTIFY_DISPATCH_ALL_CLOSED
 *
 * A constant denoting that the notify_session_dispatch() completed
 * successfully, and doesn't expect any further events to come unless
 * a new notification is sent (all notifications were closed).
 */
extern const NotifyDispatchStatus NOTIFY_DISPATCH_ALL_CLOSED;

/**
 * NOTIFY_DISPATCH_NOT_CONNECTED
 *
 * A constant denoting that the notify_session_dispatch() failed because
 * the connection is not established (anymore). This could happen also
 * if the client was disconnected for some reason.
 *
 * As disconnect results in sending 'close' event for all open notifications,
 * this could be treated similar to %NOTIFY_DISPATCH_ALL_CLOSED.
 */
extern const NotifyDispatchStatus NOTIFY_DISPATCH_NOT_CONNECTED;

/**
 * NOTIFY_SESSION_NO_TIMEOUT
 *
 * A constant for notify_session_dispatch() denoting that the session should
 * block until a message is received.
 */
extern const int NOTIFY_SESSION_NO_TIMEOUT;

/**
 * notify_session_dispatch
 * @session: session to operate on
 * @timeout: max time to block in milliseconds, or %NOTIFY_SESSION_NO_TIMEOUT
 *
 * Perform any I/O necessary for D-Bus and dispatch any new messages.
 *
 * The return value can be treated as a boolean stating whether more events
 * are expected, and thus used to terminate the main loop. Note, however, that
 * if for some reason the notification daemon doesn't send NotificationClosed
 * signal, the program may deadlock waiting for it. In order to avoid that, one
 * should use a kind of timeout timer.
 *
 * Return value: %NOTIFY_DISPATCH_DONE (which evaluates to false) unless
 * no more events are expected to come
 */
NotifyDispatchStatus notify_session_dispatch(NotifySession session,
		int timeout);

#endif /*_TINYNOTIFY_EVENT_H*/
