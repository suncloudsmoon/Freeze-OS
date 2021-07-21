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

/*
 * vm.h
 *
 *  Created on: Jul 3, 2021
 *      Author: suncloudsmoon
 */

#ifndef SRC_VM_H_
#define SRC_VM_H_

#include <stdbool.h>

#include "gc.h"
#include "../deps/tinyexpr/tinyexpr.h"
#include "stringobj.h"

#define MANAGER_ALLOC_SIZE 10

// Exception Definitions
typedef enum {
	BUFFER_OVERFLOW = -5,
	MEMORY_ALLOCATION_EXCEPTION = -10,
	INTEGER_OUT_OF_BOUNDS = -15,
	NULL_POINTER = -20,
	SYNTAX_ERROR = -25,
	TYPE_MISMATCH = -30,
	OTHER_EXCEPTION = -35
} RuntimeException;

typedef enum {
	EQUALS_OPERATOR = 0,
	GREATER_THAN_OPERATOR = 1,
	GREATHER_THAN_OR_EQUALS_OPERATOR = 2,
	LESS_THAN_OPERATOR = 3,
	LESS_THAN_OR_EQUALS_OPERATOR = 4
} Operator;

// Keeping it simple
typedef enum {
	AND_LOGIC_GATE = 0, OR_LOGIC_GATE = 1
} LogicGate;

typedef struct {
	string_t *token;
	string_t **restLine;

	// Counters
	int restLineLength;
	int restLineAllocatedLength;
} LineInfo;

typedef struct {
	long int *value1, *value2;
	Operator op;
} if_vars_t;

typedef struct {

	// int i = 0
	string_t *iName;
	long *i;

	// i < 5
	if_vars_t *condition;

	// i++
	long increment;

} for_loop_t;

typedef struct {
	for_loop_t **loops;

	int loops_allocated_length;
	int loops_length;

	// What is our current loop?
	int loopCounter;
} for_loop_manager_t;

typedef struct {
	string_t *name; // like "print"
	Type arguments[ARGUMENTLIMIT];
	ReturnType returnType;

	// Counters
	int currentArgumentIndex;
} Method;

typedef struct {
	LineInfo **parsed_code;

	int parsed_code_allocated_length;
	int parsed_code_length;
} parsed_code_manager;

// The virtual machine is very much tied to the design of the operating system
// Can create/destroy VMs via this way
typedef struct {
	// File system
	string_t *rootDir, *systemDir, *systemDirRecycleBin, *userDir,
			*userDirRecycleBin;

	FILE *location, *logFile;
	parsed_code_manager *code_manager;

	int lineNum;

	// Manages the variables & garbage collection (not implemented yet)
	VariableManager *manager;

	Method **methods;
	int numMethods;

	// Loop Stuff
	for_loop_manager_t *loop_manager;

	// Language stuff
	string_t *print_function_name, *write_function_name, *for_function_name,
			*print_special_function_name;

	// Operators
	string_t *equals_operator, *greater_than_operator,
			*greater_than_or_equals_operator, *less_than_operator,
			*less_than_or_equals_operator;

	string_t *end_function_name;

	string_t *if_condition, *else_if_condition, *else_condition;

	// Logic Gates
	string_t *and_logic_gate, *or_logic_gate;

	string_t *addition_operator, *setting_equal_operator;

	string_t *comment_function_name, *new_line_character;

} VirtualMachine;

VirtualMachine* vm_init(FILE *stream);
void vm_free(VirtualMachine *vm);

// Exception functions
// Handling errors, creating safety
void throwException(RuntimeException exception);

#endif /* SRC_VM_H_ */
