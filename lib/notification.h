/* libtinynotify -- Notification instance
 * (c) 2011 Michał Górny
 * 2-clause BSD-licensed
 */

#ifndef _TINYNOTIFY_NOTIFICATION_H
#define _TINYNOTIFY_NOTIFICATION_H

/**
 * SECTION: Notification
 * @short_description: API to deal with a single notification
 * @include: tinynotify.h
 *
 * A single notification in libtinynotify is represented by a #Notification
 * instance. A new #Notification instance can be obtained using
 * notification_new(), and should be freed when no longer used using
 * notification_free().
 *
 * Although notifications aren't directly associated with sessions, they must
 * use one in order to access the message bus. Thus, all functions interacting
 * with the message bus require passing a #NotifySession explicitly.
 *
 * Such a function may store connection-specific information within
 * the #Notification (e.g. the notification ID). However, it is guaranteed that
 * it won't store any reference to the #NotifySession or any data contained
 * within it. In other words, one may safely free a #NotifySession after use,
 * and reuse the same #Notification in another session.
 */

/**
 * Notification
 *
 * A type describing a single notification.
 *
 * It should be created using notification_new(), and disposed using
 * notification_free().
 */

typedef struct _notification* Notification;

/**
 * NOTIFICATION_NO_BODY
 *
 * A constant specifying that the notification has no body (detailed message).
 */
extern const char* const NOTIFICATION_NO_BODY;

/**
 * notification_new
 * @summary: short text summary of the notification
 * @body: complete body text of the notification (or %NOTIFICATION_NO_BODY)
 *
 * Create and initialize a new libtinynotify notification.
 *
 * This function always succeeds. If it is unable to allocate the memory,
 * program execution will be aborted.
 *
 * Note: @summary & @body are printf()-style format strings by default. For
 * plain strings, please use notification_new_unformatted() instead.
 *
 * Returns: a newly-instantiated #Notification
 */
Notification notification_new(const char* summary, const char* body);

/**
 * notification_new_unformatted
 * @summary: short text summary of the notification
 * @body: complete body text of the notification (or %NOTIFICATION_NO_BODY)
 *
 * Create and initialize a new libtinynotify notification using unformatted
 * summary & body strings.
 *
 * This function always succeeds. If it is unable to allocate the memory,
 * program execution will be aborted.
 *
 * Returns: a newly-instantiated #Notification
 */
Notification notification_new_unformatted(const char* summary, const char* body);

/**
 * notification_free
 * @notification: the notification to free
 *
 * Free a libtinynotify notification.
 *
 * This function always succeeds. After a call to this function,
 * a #Notification is no longer valid.
 */
void notification_free(Notification notification);

/**
 * NOTIFICATION_DEFAULT_APP_ICON
 *
 * A constant specifying that the default app icon should be used (if specified
 * in #NotifySession).
 */
extern const char* const NOTIFICATION_DEFAULT_APP_ICON;

/**
 * NOTIFICATION_NO_APP_ICON
 *
 * A constant specifying that no app icon should ever be used (even if
 * #NotifySession specifies one).
 */
extern const char* const NOTIFICATION_NO_APP_ICON;

/**
 * notification_set_app_icon
 * @notification: notification to operate on
 * @app_icon: a new icon name
 *
 * Set the application icon for a single notification.
 *
 * The value should be either a name in freedesktop.org-compliant icon scheme,
 * or a file:// URI.
 *
 * If %NOTIFICATION_DEFAULT_APP_ICON is passed, the notification will be reset
 * to use default app icon specified in #NotifySession (if one is set). If
 * %NOTIFICATION_NO_APP_ICON is passed, the notification will not use any app
 * icon, even if #NotifySession specifies one. Otherwise, the given string will
 * be copied to #Notification.
 */
void notification_set_app_icon(Notification notification, const char* app_icon);

/**
 * NOTIFICATION_DEFAULT_EXPIRE_TIMEOUT
 *
 * A constant specifying that the default expire timeout should be used.
 */
extern const int NOTIFICATION_DEFAULT_EXPIRE_TIMEOUT;

/**
 * NOTIFICATION_NO_EXPIRE_TIMEOUT
 *
 * A constant specifying that the notification shall not expire.
 */
extern const int NOTIFICATION_NO_EXPIRE_TIMEOUT;

/**
 * notification_set_expire_timeout
 * @notification: notification to operate on
 * @expire_timeout: a new expiration timeout [ms]
 *
 * Set the expiration timeout for a notification, in milliseconds.
 *
 * If %NOTIFICATION_DEFAULT_EXPIRE_TIMEOUT is used, the notification expires
 * on a server-specified, default timeout. If %NOTIFICATION_NO_EXPIRE_TIMEOUT
 * is used, the notification doesn't expire and needs to be closed explicitly.
 */
void notification_set_expire_timeout(Notification notification, int expire_timeout);

/**
 * NotificationUrgency
 * @NOTIFICATION_URGENCY_LOW: low urgency level
 * @NOTIFICATION_URGENCY_NORMAL: normal urgency level
 * @NOTIFICATION_URGENCY_CRITICAL: critical urgency level
 *
 * Protocol-defined urgency levels, for notification_set_urgency().
 */

typedef enum {
	NOTIFICATION_URGENCY_LOW = 0,
	NOTIFICATION_URGENCY_NORMAL = 1,
	NOTIFICATION_URGENCY_CRITICAL = 2
} NotificationUrgency;

/**
 * NOTIFICATION_NO_URGENCY
 *
 * A constant specifying that no urgency level should be set in a notifcation.
 */
extern const short int NOTIFICATION_NO_URGENCY;

/**
 * notification_set_urgency
 * @notification: notification to operate on
 * @urgency: a new urgency level
 *
 * Set the urgency level for a notification.
 *
 * If set to %NOTIFICATION_NO_URGENCY, the current urgency level would be
 * cleared.
 */
void notification_set_urgency(Notification notification, short int urgency);

/**
 * NOTIFICATION_NO_CATEGORY
 *
 * A constant specifying that no category should be used.
 */
extern const char* const NOTIFICATION_NO_CATEGORY;

/**
 * notification_set_category
 * @notification: notification to operate on
 * @category: a new category, or %NOTIFICATION_NO_CATEGORY
 *
 * Set the category for a notification.
 *
 * If set to %NOTIFICATION_NO_CATEGORY, the current category will be cleared;
 * otherwise, the category string will be copied into #Notification.
 */
void notification_set_category(Notification notification, const char* category);

/**
 * notification_send
 * @notification: the notification to send
 * @session: session to send the notification through
 * @...: additional arguments for summary & body format strings
 *
 * Send a notification to the notification daemon.
 *
 * If summary and/or body contains any printf()-style directives,
 * their arguments should be passed to this function. If both of them do,
 * arguments for the summary format string should be specified first,
 * and arguments to the body format string should immediately follow.
 *
 * Note that in the latter case, both argument lists share the same namespace.
 * Thus, if positional placeholders (\%1$s) are used, they have to be used
 * in both strings, and have to refer to the positions relative to the start
 * of the argument list to the first format string.
 *
 * If notification is displayed successfully, the received message ID is stored
 * within the #Notification type. The notification_update() function can be
 * used to update the notification afterwards.
 *
 * Returns: a positive #NotifyError or %NOTIFY_ERROR_NO_ERROR
 */
NotifyError notification_send(Notification notification, NotifySession session, ...);

/**
 * notification_update
 * @notification: the notification being updated
 * @session: session to send the notification through
 * @...: additional arguments for summary & body format strings
 *
 * Send an updated notification to the notification daemon. This will
 * replace/update the notification sent previously to server with the same
 * #Notification instance.
 *
 * If the #Notification has no ID stored, notification_update() will work
 * like notification_send(), and obtain a new ID.
 *
 * If summary and/or body contains any printf()-style directives,
 * their arguments should be passed to this function. If both of them do,
 * arguments for the summary format string should be specified first,
 * and arguments to the body format string should immediately follow.
 *
 * If notification is updated successfully, the received message ID is stored
 * within the #Notification type. Further updates to it can be done using
 * notification_update() then.
 *
 * Returns: a positive #NotifyError or %NOTIFY_ERROR_NO_ERROR
 */
NotifyError notification_update(Notification notification, NotifySession session, ...);

/**
 * notification_close
 * @notification: the notification to close
 * @session: session to send the request through
 *
 * Request closing the notification sent previously to server.
 *
 * This function succeeds unless a communication error occurs (or no ID was
 * set). It is undefined whether the notification was closed due to it, before
 * it or the notification identifier was invalid.
 *
 * This function unsets the notification ID stored in #Notification -- it is no
 * longer valid after the notification is closed.
 *
 * Returns: a positive #NotifyError or %NOTIFY_ERROR_NO_ERROR
 */
NotifyError notification_close(Notification notification, NotifySession session);

/**
 * notification_set_formatting
 * @notification: notification to operate on
 * @formatting: zero (false) to disable, non-zero (true) to enable
 *
 * Enable or disable formatting within a #Notification. If formatting is
 * enabled, summary & body are expected to be printf()-style format strings; if
 * it is disabled, they are treated as plain strings.
 *
 * Note: this function shouldn't be used unless necessary. It is preferred to
 * create a new #Notification instead.
 */
void notification_set_formatting(Notification notification, int formatting);

/**
 * notification_set_summary
 * @notification: notification to operate on
 * @summary: a new summary (format string)
 *
 * Set the summary of a notification.
 *
 * Note: this function shouldn't be used unless necessary. It is preferred to
 * create a new #Notification instead, or use variant (formatted) summary
 * in the constructor.
 */
void notification_set_summary(Notification notification, const char* summary);

/**
 * notification_set_body
 * @notification: notification to operate on
 * @body: a new body (format string, or %NOTIFICATION_NO_BODY)
 *
 * Set (or unset) the body of a notification.
 *
 * Note: this function shouldn't be used unless necessary. It is preferred to
 * create a new #Notification instead, or use variant (formatted) body
 * in the constructor.
 */
void notification_set_body(Notification notification, const char* body);

#endif /*_TINYNOTIFY_NOTIFICATION_H*/
