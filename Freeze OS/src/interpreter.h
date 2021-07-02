/*
 * Copyright (c) 2021, suncloudsmoon and the Freeze OS contributors.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED

#include <stdbool.h>

#include "stringobj.h"

#define ARGUMENTLIMIT 1000

// Method stuff

typedef enum {
	BUFFER_OVERFLOW = -5,
	MEMORY_ALLOCATION_EXCEPTION = -10,
	INTEGER_OUT_OF_BOUNDS = -15,
	NULL_POINTER = -20,
	SYNTAX_ERROR = -25,
	OTHER_EXCEPTION = -30
} RuntimeException;

typedef enum {
	INTEGER_ARGUMENT_TYPE = 1, FLOAT_ARGUMENT_TYPE = 2, OBJECT_ARGUMENT_TYPE = 3, VARARGS_ARGUMENT_TYPE = 4 // String is also an object like Java
} ArgumentType;

typedef enum {
	VOID_RETURN_TYPE = 0, INTEGER_RETURN_TYPE = 1, FLOAT_RETURN_TYPE = 2, OBJECT_RETURN_TYPE = 3
} ReturnType;

typedef struct {
	string_t *name; // like "print"
	ArgumentType arguments[ARGUMENTLIMIT];
	ReturnType returnType;

	// Counters
	int currentArgumentIndex;
} Method;

typedef struct {
	long i;
	string_t *condition;
	long conditionArgument;
	long increment;

	string_t **lines;
	int lineLength;
	int allocatedLineLength;
} ForLoop;

// Can create/destroy VMs via this way
typedef struct {
	FILE *location;
	int lineNum;

	Method **methods;
	int numMethods;

	// Loop Stuff
	bool inForLoop;
	ForLoop *vars;

	bool inWhileLoop;

	string_t *print_function_name, *write_function_name, *for_function_name, *end_function_name, *print_special_function_name, *comment_function_name;

	// Operators
	string_t *equals_operator, *greater_than_operator, *greather_than_or_equals_operator, *less_than_operator, *less_than_or_equals_operator;

	string_t *new_line_character;
} VirtualMachine;

// Starts the virtual machine
void ignition(FILE *stuff);

// Handling errors, creating safety
void throwException(RuntimeException exception, VirtualMachine *vm);

#endif // INTERPRETER_H_INCLUDED
