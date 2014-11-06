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

VALUE *
sx_new_class (SYSTEM *system, VALUE *parent) {
	VALUE *value;

	sx_lock_value (parent);
	value = (VALUE *)sx_malloc (system, sizeof (VALUE));
	sx_unlock_value (parent);
	if (value == NULL) {
		return NULL;
	}

	value->type = SX_VALUE_CLASS;
	value->data.klass.parent = parent;
	value->data.klass.members = NULL;
	value->data.klass.data = NULL;
	value->data.klass.ref_data = NULL;
	value->data.klass.free_data = NULL;
	value->locks = 0;
	value->flags = 0;
	value->gc_next = NULL;

	sx_add_gc_value (system, value);

	return value;
}

VALUE *
sx_new_user_class (SYSTEM *system, VALUE *parent, void *data, void (*free_data)(void *data), void (*ref_data)(SYSTEM *system, void *data)) {
	VALUE *value = (VALUE *)sx_malloc (system, sizeof (VALUE));
	if (value == NULL) {
		return NULL;
	}

	value->type = SX_VALUE_CLASS;
	value->data.klass.parent = parent;
	value->data.klass.members = NULL;
	value->data.klass.data = data;
	value->data.klass.ref_data = ref_data;
	value->data.klass.free_data = free_data;
	value->locks = 0;
	value->flags = 0;
	value->gc_next = NULL;

	sx_add_gc_value (system, value);

	return value;
}

int
sx_class_is_a (VALUE *klass, VALUE *par) {
	if (!SX_ISCLASS (klass) || !SX_ISCLASS (par)) {
		return 0;
	}

	while (klass != NULL && klass != par) {
		klass = klass->data.klass.parent;
	}

	return klass != NULL;
}

VAR *
sx_set_member (SYSTEM *system, VALUE *klass, unsigned int id, VALUE *value) {
	VAR *var;

	for (var = klass->data.klass.members; var != NULL; var = var->next) {
		if (id == var->id) {
			var->value = value;
			return var;
		}
	}

	sx_lock_value (klass);
	sx_lock_value (value);

	var = (VAR *)sx_malloc (system, sizeof (VAR));

	sx_unlock_value (klass);
	sx_unlock_value (value);

	if (var == NULL) {
		return NULL;
	}

	var->id = id;
	var->value = value;
	var->next = klass->data.klass.members;
	klass->data.klass.members = var;

	return var;
}

VALUE *
sx_get_member (VALUE *klass, unsigned int id) {
	VAR *var;

	while (klass != NULL) {
		for (var = klass->data.klass.members; var != NULL; var = var->next) {
			if (id == var->id) {
				return var->value;
			}
		}
		klass = klass->data.klass.parent;
	}

	return sx_new_nil ();
}