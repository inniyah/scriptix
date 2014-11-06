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

#ifndef SCRIPTIX_H
#define SCFIPTIX_H

#ifdef __cplusplus__
extern "C" {
#endif

#define SCRIPTIX_VERSION "0.6"

#define STACK_CHUNK_SIZE 50
#define GC_THRESH_SIZE 200

#ifdef __GNUC__
	#define __INLINE__ inline
#else
	#define __INLINE__
#endif

#define VALUE_NIL 0
#define VALUE_NUM 1
#define VALUE_STRING 2
#define VALUE_BLOCK 3
#define VALUE_FUNC 4
#define VALUE_CFUNC 5
#define VALUE_ARRAY 6
#define VALUE_CLASS 7
#define VALUE_RANGE 8

#define NODE_VALUE 1
#define NODE_EXPR 2

#define OP_ADD 1
#define OP_SUBTRACT 2
#define OP_NEGATE 3
#define OP_CALL 4
#define OP_MULTIPLY 5
#define OP_DIVIDE 6
#define OP_GT 7
#define OP_LT 8
#define OP_OR 9
#define OP_AND 10
#define OP_GTE 11
#define OP_LTE 12
#define OP_EQUAL 13
#define OP_NEQUAL 14
#define OP_NOT 15
#define OP_LOOKUP 16
#define OP_ASSIGN 17
#define OP_IF 18
#define OP_WHILE 19
#define OP_BREAK 20
#define OP_RETURN 21
#define OP_INDEX 22
#define OP_PREINCREMENT 23
#define OP_POSTINCREMENT 24
#define OP_PREDECREMENT 25
#define OP_POSTDECREMENT 26
#define OP_NEWARRAY 27
#define OP_SETINDEX 28
#define OP_SIZEOF 29
#define OP_NEWCLASS 30
#define OP_SETMEMBER 31
#define OP_MEMBER 32
#define OP_NEWINSTANCE 33
#define OP_UNTIL 34
#define OP_TRY 35
#define OP_ISA 36
#define OP_TYPEOF 37
#define OP_RAISE 38
#define OP_EVAL 39
#define OP_FOR 40

#define VFLAG_MARK 0x01

#define SFLAG_GCOFF 0x01

#define STATE_READY 0
#define STATE_RUN 1
#define STATE_BREAK 2
#define STATE_RETURN 3
#define STATE_EXIT 4
#define STATE_ERROR 5

#define SCOPE_DEF 0
#define SCOPE_LOCAL 1
#define SCOPE_THREAD 2
#define SCOPE_GLOBAL 3

#define NUM_MARK 0x01

typedef struct scriptix_value VALUE;
typedef struct scriptix_node NODE;
typedef struct scriptix_var VAR;
typedef struct scriptix_context CONTEXT;
typedef struct scriptix_system SYSTEM;
typedef struct scriptix_thread THREAD;

extern __INLINE__ void *sx_malloc (SYSTEM *system, unsigned long size);
extern __INLINE__ void *sx_dupmem (SYSTEM *system, void *mem, unsigned long size);
#define sx_free free

#define new_nil() ((VALUE *)(NULL))
#define new_num(i) ((VALUE *)(((i) << 1) | NUM_MARK))
#define TO_INT(n) ((long)(n) >> 1)
#define TO_STR(s) ((s)->data.str.str)
extern VALUE *new_str (SYSTEM *system, char *str);
extern VALUE *new_block (SYSTEM *system);
extern VALUE *add_stmt (VALUE *block, NODE *node);
extern VALUE *new_func (SYSTEM *system, VALUE *args, VALUE *body);
extern VALUE *new_cfunc (SYSTEM *system, VALUE *(*func)(THREAD *, VALUE *, unsigned int, unsigned int));
extern VALUE *new_array (SYSTEM *system, unsigned int argc, VALUE **argv);
extern VALUE *new_stack_array (THREAD *thread, unsigned int argc, unsigned int top);
extern VALUE *new_class (SYSTEM *system, VALUE *parent);
extern VALUE *new_user_class (SYSTEM *system, VALUE *parent, void *data, void (*free_data)(void *data), void (*ref_data)(SYSTEM *system, void *data));
extern VALUE *new_range (SYSTEM *system, int start, int end, int step);
extern VALUE *copy_value (SYSTEM *system, VALUE *value);
extern void mark_value (SYSTEM *system, VALUE *value);
extern int is_true (VALUE *value);
extern int are_equal (VALUE *one, VALUE *two);
extern void print_value (VALUE *value);
extern __INLINE__ void lock_value (VALUE *value);
extern __INLINE__ void unlock_value (VALUE *value);
extern int class_is_a (VALUE *klass, VALUE *par);
extern VAR *set_member (SYSTEM *system, VALUE *klass, char *name, VALUE *value);
extern VALUE *get_member (VALUE *klass, char *name);
extern void free_value (VALUE *value);

#define type_of(v) ((v) == (NULL) ? VALUE_NIL : ((long)(v) & NUM_MARK) ? VALUE_NUM : (v)->type)
#define IS_NIL(v) ((v) == NULL)
#define IS_NUM(v) ((long)(v) & NUM_MARK)
#define IS_STRING(v) ((type_of ((v))) == VALUE_STRING)
#define IS_BLOCK(v) ((type_of ((v))) == VALUE_BLOCK)
#define IS_FUNC(v) ((type_of ((v))) == VALUE_FUNC)
#define IS_CFUNC(v) ((type_of ((v))) == VALUE_CFUNC)
#define IS_ARRAY(v) ((type_of ((v))) == VALUE_ARRAY)
#define IS_CLASS(v) ((type_of ((v))) == VALUE_CLASS)
#define IS_RANGE(v) ((type_of ((v))) == VALUE_RANGE)

extern NODE *new_node (SYSTEM *system, VALUE *value);
extern NODE *new_expr (SYSTEM *system, char op, int count, ...);
extern void free_node (NODE *node);
extern void mark_node (SYSTEM *system, NODE *node);

extern int eval (THREAD *thread, VALUE *value);
#define thread_break(t) ((t) & BFLAG_BREAK || (t) & BFLAG_RETURN || (t) & BFLAG_EXIT)

extern VALUE *define_cfunc (SYSTEM *system, char *name, VALUE *(*func)(THREAD *, VALUE *, unsigned int, unsigned int));

extern VALUE *do_define_var (THREAD *thread, VALUE *name, VALUE *value, int scope);
#define define_var(t,n,v) (do_define_var ((t), (n), (v), SCOPE_DEF))
#define define_local_var(t,n,v) (do_define_var ((t), (n), (v), SCOPE_LOCAL))
#define define_thread_var(t,n,v) (do_define_var ((t), (n), (v), SCOPE_THREAD))
extern VALUE *define_global_var (SYSTEM *system, VALUE *name, VALUE *value);
extern __INLINE__ VALUE *do_lookup_var (THREAD *thread, char *name, int scope);
#define lookup_var(t,n) (do_lookup_var ((t), (n), SCOPE_DEF))
#define lookup_local_var(t,n) (do_lookup_var ((t), (n), SCOPE_LOCAL))
#define lookup_thread_var(t,n) (do_lookup_var ((t), (n), SCOPE_THREAD))
extern VALUE *lookup_global_var (SYSTEM *system, char *name);
extern VAR *get_var (THREAD *thread, char *name, int scope);
#define free_var(v) sx_free ((v))

extern SYSTEM *create_system (int argc, char **argv);
extern void run_gc (SYSTEM *system);
extern void free_system (SYSTEM *system);

extern THREAD *create_thread (SYSTEM *system, VALUE *main);
extern VALUE *load_file (SYSTEM *system, char *file);
extern int run_thread (THREAD *thread);
extern void mark_thread (THREAD *thread);
extern void end_thread (THREAD *thread);
extern void free_thread (THREAD *thread);
extern THREAD *push_context (THREAD *thread, VALUE *block);
extern THREAD *pop_context (THREAD *thread);
extern VALUE *push_value (THREAD *thread, VALUE *value);
extern __INLINE__ VALUE *get_value (THREAD *thread, int);
extern __INLINE__ void pop_value (THREAD *thread, int start, unsigned int len);

/* standard library calls */
extern void init_stdlib (SYSTEM *system);

struct scriptix_value {
	char type;
	union {
		struct {
			char *str;
			unsigned int len;
		} str;
		struct {
			VALUE *args;
			VALUE *body;
		} func;
		struct {
			VALUE **list;
			unsigned int count;
			unsigned int size;
		} array;
		struct {
			VALUE *parent;
			VAR *members;
			void *data;
			void (*free_data)(void *data);
			void (*ref_data)(SYSTEM *system, void *data);
		} klass;
		struct {
			int start;
			int end;
			int step;
		} range;
		VALUE *(*cfunc)(THREAD *, VALUE *, unsigned int, unsigned int);
		NODE *nodes;
	} data;
	VALUE *gc_next;
	unsigned char flags;
	unsigned char locks;
};

struct scriptix_node {
	char type;
	union {
		VALUE *value;
		struct {
			unsigned char op;
			unsigned int count;
			NODE **nodes;
		} expr;
	} data;
	NODE *next;
};

struct scriptix_var {
	VALUE *name;
	VALUE *value;
	VAR *next;
};

struct scriptix_context {
	VAR *vars;
	VALUE *block;
};

struct scriptix_system {
	THREAD *threads;
	VAR *vars;
	VALUE *gc_values;
	unsigned char flags;
	unsigned int gc_count;
	unsigned int gc_thresh;
};

struct scriptix_thread {
	SYSTEM *system;
	VAR *vars;
	VALUE *main;
	VALUE *ret;
	int state;

	CONTEXT *context_stack;
	unsigned int context;
	unsigned int context_size;

	VALUE **data_stack;
	unsigned int data;
	unsigned int data_size;

	THREAD *next;
};

#ifdef __cplusplus__
} /* extern "C" */
#endif

#endif