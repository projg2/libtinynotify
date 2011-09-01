/* libtinynotify -- common utility functions
 * (c) 2011 Michał Górny
 * 2-clause BSD-licensed
 */

#pragma once
#ifndef _TINYNOTIFY_COMMON__H
#define _TINYNOTIFY_COMMON__H

#include <stdarg.h>

#define _mem_assert(x) _mem_check(!!(x))

void _mem_check(int res);
void _property_assign_str(char** prop, const char* newval);
int _dual_vasprintf(char **outa, const char *fstra,
		const char** outb, const char *fstrb, va_list ap);

#endif /*_TINYNOTIFY_COMMON__H*/
