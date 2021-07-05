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
 * vm.c
 *
 *  Created on: Jul 3, 2021
 *      Author: suncloudsmoon
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <errno.h>

#include "vm.h"
#include "gc.h"
#include "stringobj.h"

static VirtualMachine *predefinedVM;

// Static Prototypes
static void exceptionPrinter(int lineNum, char *message, bool isErrno);
static void waitToExit(int code);

VirtualMachine* vm_init(FILE *stream) {
	VirtualMachine *vm = (VirtualMachine*) calloc(1, sizeof(VirtualMachine));
	vm->location = stream;
	vm->lineNum = 0;

	vm->manager = varmanager_init();

	vm->numMethods = 500; // For now
	vm->methods = (Method**) safe_calloc(vm->numMethods, sizeof(Method));

	// Set up the error system
	predefinedVM = vm; // Is this a memory leak?
	throwException(0, NULL);

	// Freeze Programming Language Definitions //

	// Default functions of the programming language
	vm->print_function_name = string_copyvalueof("print");
	vm->print_special_function_name = string_copyvalueof("print special");
	vm->write_function_name = string_copyvalueof("write");
	vm->for_function_name = string_copyvalueof("for"); // "loop" for some reason does not work
	vm->end_function_name = string_copyvalueof("end");

	// Operators
	vm->equals_operator = string_copyvalueof("==");
	vm->greater_than_operator = string_copyvalueof(">");
	vm->greather_than_or_equals_operator = string_copyvalueof(">=");
	vm->less_than_operator = string_copyvalueof("<");
	vm->less_than_or_equals_operator = string_copyvalueof("<=");

	vm->addition_operator = string_copyvalueof("+");

	vm->setting_equal_operator = string_copyvalueof("->");

	// General Ones
	vm->comment_function_name = string_copyvalueof("//");
	vm->new_line_character = string_copyvalueof("\\n");

	return vm;
}

void vm_free(VirtualMachine *vm) {
//	fclose(vm->location);
	for (int i = 0; i < vm->numMethods; i++) {
		free(vm->methods[i]);
	}
	free(vm->methods);
	free(vm);
}

////////////////// Exception Handling /////////////////////

void throwException(RuntimeException exception, const VirtualMachine *vm) {

// print like Line #%d: (predefined message) and custom message\n
	switch (exception) {
	case BUFFER_OVERFLOW:
		exceptionPrinter(predefinedVM->lineNum,
				"Ah, a buffer overflow has occurred", false);
		waitToExit(exception);
		break;
	case MEMORY_ALLOCATION_EXCEPTION:
		exceptionPrinter(predefinedVM->lineNum, NULL, true);
		waitToExit(exception);
		break;
	case INTEGER_OUT_OF_BOUNDS:
		exceptionPrinter(predefinedVM->lineNum,
				"Accessed an integer out of bounds", false);
		waitToExit(exception);
		break;
	case NULL_POINTER:
		exceptionPrinter(predefinedVM->lineNum,
				"A null pointer exception occurred", false);
		waitToExit(exception);
		break;
	case SYNTAX_ERROR:
		exceptionPrinter(predefinedVM->lineNum, "A syntax error has occurred",
		false);
		waitToExit(exception);
		break;
	case OTHER_EXCEPTION:
		exceptionPrinter(predefinedVM->lineNum, NULL,
		true);
		waitToExit(exception);
		break;
	default:
		break;
	}

}

static void exceptionPrinter(int lineNum, char *message, bool isErrno) {
	char temp[BUFSIZ];

	if (isErrno) {
		sprintf(temp, "Error on Line #%d", lineNum);
		perror(temp);
	} else {
		printf("Error on Line #%d: %s\n", lineNum, message);
	}

}

static void waitToExit(int code) {
	printf(
			"The program has just ended due to an error, press any key to exit...");
	getchar();
	exit(code);
}
