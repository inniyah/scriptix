/*
 * Scriptix - Lite-weight scripting interface
 * Copyright (c) 2002, AwesomePlay Productions, Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "scriptix.h"

/* internal function stuff */
void
_sx_function_mark (SX_SYSTEM *system, SX_VALUE *value) {
	if (value->data.func.cfunc == NULL) {
		sx_mark_value (system, value->data.func.args);
		sx_mark_value (system, value->data.func.body);
	} else {
		if (value->data.func.data != NULL) {
			sx_mark_value (system, value->data.func.data);
		}
	}
}

int
_sx_function_equal (SX_SYSTEM *system, SX_VALUE *one, SX_VALUE *two) {
	if (one->data.func.cfunc != NULL) {
		return one->data.func.cfunc == two->data.func.cfunc;
	} else {
		return one->data.func.args == two->data.func.args && one->data.func.body == two->data.func.body;
	}
}

SX_CLASS *
sx_init_function (SX_SYSTEM *system) {
	SX_CLASS *klass;

	klass = sx_new_core_class (system, sx_name_to_id ("Function"));
	if (klass == NULL) {
		return NULL;
	}

	klass->core->fmark = _sx_function_mark;
	klass->core->fequal = _sx_function_equal;

	return klass;
}

SX_VALUE *
sx_new_func (SX_SYSTEM *system, SX_VALUE *args, SX_VALUE *body) {
	SX_VALUE *value;

	if (!SX_ISBLOCK (system, body)) {
		return NULL;
	}
	if (!SX_ISNIL (system, args) && !SX_ISARRAY (system, args)) {
		return NULL;
	}
	
	value = (SX_VALUE *)sx_malloc (system, sizeof (SX_VALUE));
	if (value == NULL) {
		return NULL;
	}

	value->klass = system->cfunction;
	value->members = NULL;
	value->data.func.args = args;
	value->data.func.body = body;
	value->data.func.cfunc = NULL;
	value->locks = 0;
	value->gc_next = NULL;
	value->flags = 0;

	sx_add_gc_value (system, value);

	return value;
}

SX_VALUE *
sx_new_cfunc (SX_SYSTEM *system, sx_cfunc func, SX_VALUE *data) {
	SX_VALUE *value;
	
	sx_lock_value (data);
	value = (SX_VALUE *)sx_malloc (system, sizeof (SX_VALUE));
	sx_unlock_value (data);
	if (value == NULL) {
		return NULL;
	}

	value->klass = system->cfunction;
	value->members = NULL;
	value->data.func.cfunc = func;
	value->data.func.data = data;
	value->locks = 0;
	value->flags = 0;
	value->gc_next = NULL;

	sx_add_gc_value (system, value);

	return value;
}

SX_VALUE *
sx_define_cfunc (SX_SYSTEM *system, char *name, sx_cfunc func, SX_VALUE *data) {
	SX_VALUE *cfunc = sx_new_cfunc (system, func, data);
	if (cfunc == NULL) {
		return NULL;
	}

	return sx_define_system_var (system, sx_name_to_id (name), cfunc);
}
