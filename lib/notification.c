/* libtinynotify -- session interface
 * (c) 2011 Michał Górny
 * 2-clause BSD-licensed
 */

#include "config.h"

#include "error.h"
#include "session.h"
#include "notification.h"

#include <stdlib.h>

const char* const NOTIFICATION_NO_BODY = NULL;

const char* const NOTIFICATION_DEFAULT_APP_ICON = NULL;
const char* const NOTIFICATION_NO_APP_ICON = "";

const int NOTIFICATION_DEFAULT_EXPIRE_TIMEOUT = -1;
const int NOTIFICATION_NO_EXPIRE_TIMEOUT = 0;

const short int NOTIFICATION_NO_URGENCY = -1;
const char* const NOTIFICATION_NO_CATEGORY = NULL;
