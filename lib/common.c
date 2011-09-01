/* libtinynotify -- common utility functions
 * (c) 2011 Michał Górny
 * 2-clause BSD-licensed
 */

#include "config.h"
#include "common_.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
