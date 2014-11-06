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

#include "scriptix.h"

SX_DEFINE_CFUNC (sx_stdlib_get_tid) {
	sx_push_value (sx_thread, sx_new_num ((long)sx_thread >> 2));
}

SX_DEFINE_CFUNC (sx_stdlib_print) {
	unsigned int i;
	for (i = 0; i < sx_argc; i ++) {
		sx_print_value (sx_thread->system, sx_get_value (sx_thread, sx_top + i));
	}
	sx_push_value (sx_thread, NULL);
}

SX_DEFINE_CFUNC (sx_stdlib_printl) {
	sx_stdlib_print (sx_thread, sx_self, sx_data, sx_argc, sx_top);
	sx_thread->system->print_hook ("\r\n");
	sx_push_value (sx_thread, NULL);
}

SX_DEFINE_CFUNC (sx_stdlib_concat) {
	SX_VALUE *ret, *one, *two;

	if (sx_argc != 2) {
		sx_raise_error (sx_thread, sx_ArgumentError);
		return;
	}

	one = sx_get_value (sx_thread, sx_top);
	two = sx_get_value (sx_thread, sx_top + 1);

	if (sx_type_of (one) != sx_type_of (two)) {
		sx_raise_error (sx_thread, sx_TypeError);
		return;
	}

	if (SX_ISSTRING (one)) {
		ret = sx_new_str_len (sx_thread->system, NULL, one->data.str.len + one->data.str.len);
		if (ret == NULL) {
			sx_raise_error (sx_thread, sx_MemError);
			return;
		}
		if (one->data.str.len > 0) {
			strcpy (SX_TOSTR (ret), SX_TOSTR (one));
		}
		if (two->data.str.len > 0) {
			strcpy (SX_TOSTR (ret) + one->data.str.len, SX_TOSTR (two));
		}
		sx_push_value (sx_thread, ret);
	} else if (SX_ISARRAY (two)) {
		/* FIXME */
		sx_push_value (sx_thread, one);
	}
}

void
sx_init_stdlib (SX_SYSTEM *system) {
	sx_define_cfunc (system, "print", sx_stdlib_print, NULL);
	sx_define_cfunc (system, "printl", sx_stdlib_printl, NULL);
	sx_define_cfunc (system, "concat", sx_stdlib_concat, NULL);
	sx_define_cfunc (system, "get_tid", sx_stdlib_get_tid, NULL);
}
