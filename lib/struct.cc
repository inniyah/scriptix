/*
 * Scriptix - Lite-weight scripting interface
 * Copyright (c) 2002, 2003  AwesomePlay Productions, Inc.
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

#include "scriptix.h"
#include "system.h"

using namespace Scriptix;

SX_TYPEIMPL(Struct, "Struct", Value);

SX_NOMETHODS(Struct);

// Our static methods
SX_BEGINSMETHODS(Struct)
	SX_DEFNEW(Struct)
SX_ENDMETHODS

// Default undefined member operations
void
Struct::SetUndefMember (System* system, NameID id, Value* value)
{
	data[id] = value;
	Value::Mark(value);
}

Value*
Struct::GetUndefMember (System*, NameID)
{
	// no-op
	return NULL;
}

// Garbage collection
void
Struct::MarkChildren (void)
{
	for (datalist::iterator i = data.begin(); i != data.end(); i ++)
		Value::Mark(i->second);
}

// Member operations
void
Struct::SetMember (System* system, NameID id, Value* value)
{
	// try to set value
	datalist::iterator i = data.find(id);
	if (i != data.end()) {
		i->second = value;
		Value::Mark (value);
		return;
	}

	// undefined - set that way then
	SetUndefMember (system, id, value);
}

Value*
Struct::GetMember (System* system, NameID id)
{
	// try to find member
	datalist::iterator i = data.find(id);
	if (i != data.end()) {
		return i->second;
	}

	// undefined - get that way then
	return GetUndefMember (system, id);
}