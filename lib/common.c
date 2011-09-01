/* libtinynotify -- common utility functions
 * (c) 2011 Michał Górny
 * 2-clause BSD-licensed
 */

#include "config.h"
#include "common_.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

void _mem_check(int res) {
	if (!res) {
		fputs("Memory allocation failed.\n", stderr);
		abort();
	}
}

void _property_assign_str(char** prop, const char* newval) {
	if (*prop)
		free(*prop);
	if (newval)
		_mem_assert(*prop = strdup(newval));
	else
		*prop = NULL;
}

#ifndef va_copy
#	ifdef __va_copy
#		define va_copy __va_copy
#	else
#		define va_copy(a, b) memcpy(&a, &b, sizeof(va_list))
#	endif
#endif

int _dual_vasprintf(char **outa, const char *fstra,
		const char** outb, const char *fstrb, va_list ap) {
	int first_len, ret;
	va_list first_ap;
	char *full_fstr;

	va_copy(first_ap, ap);
	first_len = vsnprintf(NULL, 0, fstra, first_ap);
	va_end(first_ap);
	assert(first_len >= 0);

	if (asprintf(&full_fstr, "%s\1%s", fstra, fstrb) == -1)
		return -1;
	ret = vasprintf(outa, full_fstr, ap);
	free(full_fstr);

	if (ret == -1)
		return -1;
	assert(ret >= first_len);

	assert((*outa)[first_len] == 1);
	(*outa)[first_len] = 0;
	*outb = &(*outa)[first_len+1];

	return ret;
}
