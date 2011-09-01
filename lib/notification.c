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
#include "notification_.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

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
	notification_bind_close_callback(n, NOTIFICATION_NO_CLOSE_CALLBACK, NULL);
	return n;
}

void notification_free(Notification n) {
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
