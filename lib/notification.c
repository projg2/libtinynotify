/* libtinynotify -- session interface
 * (c) 2011 Michał Górny
 * 2-clause BSD-licensed
 */

#include "config.h"

#include "error.h"
#include "session.h"
#include "notification.h"
#include "event.h"

#include "common_.h"
#include "session_.h"
#include "notification_.h"
#include "event_.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <dbus/dbus.h>

const char* const NOTIFICATION_NO_BODY = NULL;

const char* const NOTIFICATION_DEFAULT_APP_ICON = NULL;
const char* const NOTIFICATION_NO_APP_ICON = "";

const int NOTIFICATION_DEFAULT_EXPIRE_TIMEOUT = -1;
const int NOTIFICATION_NO_EXPIRE_TIMEOUT = 0;

const short int NOTIFICATION_NO_URGENCY = -1;
const char* const NOTIFICATION_NO_CATEGORY = NULL;

static const dbus_uint32_t NOTIFICATION_NO_NOTIFICATION_ID = 0;

Notification notification_new(const char* summary, const char* body) {
	Notification n = notification_new_unformatted(summary, body);
	notification_set_formatting(n, 1);
	return n;
}

Notification notification_new_unformatted(const char* summary, const char* body) {
	Notification n;

	assert(summary);

	_mem_assert(n = malloc(sizeof(*n)));
	/* can't use notification_set_summary() here because it has to free sth */
	_mem_assert(n->summary = strdup(summary));
	n->body = NULL;
	n->category = NULL;
	n->app_icon = NULL;
	n->message_id = NOTIFICATION_NO_NOTIFICATION_ID;

	notification_set_body(n, body);
	notification_set_formatting(n, 0);
	notification_set_expire_timeout(n, NOTIFICATION_DEFAULT_EXPIRE_TIMEOUT);
	notification_set_urgency(n, NOTIFICATION_NO_URGENCY);
	_notification_event_init(n);
	return n;
}

void notification_free(Notification n) {
	_notification_event_free(n);
	free(n->summary);
	if (n->body)
		free(n->body);
	if (n->app_icon)
		free(n->app_icon);
	free(n);
}

void notification_set_app_icon(Notification n, const char* app_icon) {
	_property_assign_str(&n->app_icon, app_icon);
}

void notification_set_expire_timeout(Notification n, int expire_timeout) {
	n->expire_timeout = expire_timeout;
}

void notification_set_urgency(Notification n, short int urgency) {
	n->urgency = urgency;
}

void notification_set_category(Notification n, const char* category) {
	_property_assign_str(&n->category, category);
}

static void _notification_append_hint(DBusMessageIter* subiter,
		const char* key, const char* hint_type, const void* hint_val) {
	DBusMessageIter dictiter, variter;

	_mem_assert(dbus_message_iter_open_container(subiter,
				DBUS_TYPE_DICT_ENTRY,
				NULL,
				&dictiter));

	_mem_assert(dbus_message_iter_append_basic(&dictiter,
				DBUS_TYPE_STRING, &key));

	_mem_assert(dbus_message_iter_open_container(&dictiter,
				DBUS_TYPE_VARIANT,
				hint_type,
				&variter));

	_mem_assert(dbus_message_iter_append_basic(&variter,
				*hint_type, hint_val));

	_mem_assert(dbus_message_iter_close_container(&dictiter, &variter));
	_mem_assert(dbus_message_iter_close_container(subiter, &dictiter));
}

static NotifyError notification_update_va(Notification n, NotifySession s, va_list ap) {
	NotifyError ret;
	char *err_msg;
	char *f_summary;
	struct _notification_action_list *al;

	DBusMessage *msg, *reply;
	DBusMessageIter iter, subiter;
	DBusError err;

	const char *app_name = s->app_name ? s->app_name : "";
	dbus_uint32_t replaces_id = n->message_id;
	const char *app_icon = n->app_icon ? n->app_icon :
			s->app_icon ? s->app_icon : "";
	const char *summary = n->summary;
	const char *body = n->body ? n->body : "";
	dbus_int32_t expire_timeout = n->expire_timeout;

	if (notify_session_connect(s))
		return notify_session_get_error(s);

	if (n->formatting) {
		_mem_assert(_dual_vasprintf(&f_summary, summary,
					&body, body, ap) != -1);

		summary = f_summary;
	}

	_mem_assert(msg = dbus_message_new_method_call("org.freedesktop.Notifications",
				"/org/freedesktop/Notifications",
				"org.freedesktop.Notifications",
				"Notify"));

	_mem_assert(dbus_message_append_args(msg,
				DBUS_TYPE_STRING, &app_name,
				DBUS_TYPE_UINT32, &replaces_id,
				DBUS_TYPE_STRING, &app_icon,
				DBUS_TYPE_STRING, &summary,
				DBUS_TYPE_STRING, &body,
				DBUS_TYPE_INVALID));

	dbus_message_iter_init_append(msg, &iter);

	/* actions */
	_mem_assert(dbus_message_iter_open_container(&iter,
				DBUS_TYPE_ARRAY, DBUS_TYPE_STRING_AS_STRING, &subiter));
	for (al = n->actions; al; al = al->next) {
		_mem_assert(dbus_message_iter_append_basic(&subiter,
					DBUS_TYPE_STRING, &al->key));
		_mem_assert(dbus_message_iter_append_basic(&subiter,
					DBUS_TYPE_STRING, &al->desc));
	}
	_mem_assert(dbus_message_iter_close_container(&iter, &subiter));

	/* hints */
	_mem_assert(dbus_message_iter_open_container(&iter,
				DBUS_TYPE_ARRAY,
				DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
				DBUS_TYPE_STRING_AS_STRING
				DBUS_TYPE_VARIANT_AS_STRING
				DBUS_DICT_ENTRY_END_CHAR_AS_STRING,
				&subiter));

	/* -> urgency */
	if (n->urgency != NOTIFICATION_NO_URGENCY) {
		const unsigned char urgency = n->urgency;

		_notification_append_hint(&subiter, "urgency",
				DBUS_TYPE_BYTE_AS_STRING, &urgency);
	}
	/* -> category */
	if (n->category)
		_notification_append_hint(&subiter, "category",
				DBUS_TYPE_STRING_AS_STRING, &n->category);

	_mem_assert(dbus_message_iter_close_container(&iter, &subiter));

	_mem_assert(dbus_message_iter_append_basic(&iter,
				DBUS_TYPE_INT32, &expire_timeout));

	dbus_error_init(&err);
	reply = dbus_connection_send_with_reply_and_block(s->conn,
			msg, DBUS_TIMEOUT_INFINITE, &err);

	assert(!reply == dbus_error_is_set(&err));
	if (!reply) {
		err_msg = strdup(err.message);
		dbus_error_free(&err);
		ret = NOTIFY_ERROR_DBUS_SEND;
	} else {
		dbus_uint32_t new_id;

		assert(dbus_message_get_type(reply) == DBUS_MESSAGE_TYPE_METHOD_RETURN);

		if (!dbus_message_get_args(reply, &err,
					DBUS_TYPE_UINT32, &new_id,
					DBUS_TYPE_INVALID)) {
			err_msg = strdup(err.message);
			dbus_error_free(&err);
			ret = NOTIFY_ERROR_INVALID_REPLY;
		} else {
			n->message_id = new_id;
			err_msg = NULL;
			ret = NOTIFY_ERROR_NO_ERROR;

			_notify_session_add_notification(s, n);
		}

		dbus_message_unref(reply);
	}

	dbus_message_unref(msg);
	if (n->formatting)
		free(f_summary);
	return notify_session_set_error(s, ret, err_msg);
}

static NotifyError notification_send_va(Notification n, NotifySession s, va_list ap) {
	n->message_id = NOTIFICATION_NO_NOTIFICATION_ID;
	return notification_update_va(n, s, ap);
}

NotifyError notification_send(Notification n, NotifySession s, ...) {
	va_list ap;
	NotifyError ret;

	va_start(ap, s);
	ret = notification_send_va(n, s, ap);
	va_end(ap);

	return ret;
}

NotifyError notification_update(Notification n, NotifySession s, ...) {
	va_list ap;
	NotifyError ret;

	va_start(ap, s);
	ret = notification_update_va(n, s, ap);
	va_end(ap);

	return ret;
}

NotifyError notification_close(Notification n, NotifySession s) {
	NotifyError ret;
	char *err_msg;

	DBusMessage *msg, *reply;
	DBusError err;

	dbus_uint32_t id = n->message_id;

	if (id == NOTIFICATION_NO_NOTIFICATION_ID)
		return notify_session_set_error(s, NOTIFY_ERROR_NO_NOTIFICATION_ID);

	if (notify_session_connect(s))
		return notify_session_get_error(s);

	_mem_assert(msg = dbus_message_new_method_call("org.freedesktop.Notifications",
				"/org/freedesktop/Notifications",
				"org.freedesktop.Notifications",
				"CloseNotification"));

	_mem_assert(dbus_message_append_args(msg,
				DBUS_TYPE_UINT32, &id,
				DBUS_TYPE_INVALID));

	dbus_error_init(&err);
	reply = dbus_connection_send_with_reply_and_block(s->conn,
			msg, 5000 /* XXX */, &err);

	assert(!reply == dbus_error_is_set(&err));
	if (!reply) {
		err_msg = strdup(err.message);
		dbus_error_free(&err);
		ret = NOTIFY_ERROR_DBUS_SEND;
	} else {
		assert(dbus_message_get_type(reply) == DBUS_MESSAGE_TYPE_METHOD_RETURN);

		if (!dbus_message_get_args(reply, &err,
					DBUS_TYPE_INVALID)) {
			err_msg = strdup(err.message);
			dbus_error_free(&err);
			ret = NOTIFY_ERROR_INVALID_REPLY;
		} else {
			n->message_id = NOTIFICATION_NO_NOTIFICATION_ID;
			err_msg = NULL;
			ret = NOTIFY_ERROR_NO_ERROR;
		}

		dbus_message_unref(reply);
	}

	dbus_message_unref(msg);
	return notify_session_set_error(s, ret, err_msg);
}

void notification_set_formatting(Notification n, int formatting) {
	n->formatting = formatting;
}

void notification_set_summary(Notification n, const char* summary) {
	assert(summary);
	_property_assign_str(&n->summary, summary);
}

void notification_set_body(Notification n, const char* body) {
	_property_assign_str(&n->body, body);
}
