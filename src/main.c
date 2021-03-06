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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <string.h>

#include "../include/test.h"
#include "../include/vm.h"
#include "../include/interpreter.h"

void testeverything();

// What do the arguments mean exactly?
int main(int argc, char **argv) {

	const char *defaultTest = "test\\Test #1.fz";

	FILE *test = stdin;

	/* If we are passed an argument, treat it as an input file name */
	if (argc > 1) {
		test = fopen(argv[1], "r");

	} else {
		test = fopen(defaultTest, "r");
	}

	/* Check for fopen failure */
	if (test == NULL) {
		perror("Failed to open interpreter file: ");
		exit(EXIT_FAILURE);
	}

	// Starting the interpreter
	VirtualMachine *vm = vm_init(test);
	ignition(vm);
	vm_free(vm);

	if (test != stdin) {
		fclose(test);
	}

	// Unit Testing?
	//	testeverything();

	return 0;
}

void testeverything() {
	// The most annoying...
	test_string();
}
