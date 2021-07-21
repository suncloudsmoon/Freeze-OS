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
 * bytecode.c
 *
 *  Created on: Jul 4, 2021
 *      Author: suncloudsmoon
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/bytecode.h"
#include "../include/vm.h"
#include "../include/safedefault.h"
#include "../include/parser.h"

// Static Helper Methods
static int readLine(FILE *stuff, string_t *line);
static LineInfo* splitIntoTokens(string_t *line, VirtualMachine *vm);

void readAllLines(VirtualMachine *vm) {
	int readStatus = 0;
	do {
		string_t *line = string_init();
		readLine(vm->location, line);

		codemanager_add(splitIntoTokens(line, vm), vm->code_manager);

		// Free resources
		string_free(line);

		vm->lineNum++;
	} while (readStatus != EOF);

	vm->lineNum = 0; // Reset the line number so the interpreter starts from the beginning
}

// Reads one line from file stream
static int readLine(FILE *stuff, string_t *line) {
	while (true) {
		char letter = safe_fgetc(stuff);

		if (letter == '\n' || letter == EOF) {
			return letter;
		}
		string_concat_c(letter, line);
	}
}

// Code Manager //

parsed_code_manager* codemanager_init() {
	parsed_code_manager *manager = (parsed_code_manager*) calloc(1,
			sizeof(parsed_code_manager));

	manager->parsed_code = (LineInfo**) calloc(MANAGER_ALLOC_SIZE,
			sizeof(LineInfo*));
	manager->parsed_code_allocated_length = MANAGER_ALLOC_SIZE;
	manager->parsed_code_length = 0; // I think this is redundant

	return manager;
}

void codemanager_add(LineInfo *info, parsed_code_manager *manager) {
	if (manager->parsed_code_length + 1
			> manager->parsed_code_allocated_length) {
		manager->parsed_code = (LineInfo**) realloc(manager->parsed_code,
				10 * sizeof(LineInfo*));
		manager->parsed_code_allocated_length += MANAGER_ALLOC_SIZE;
	}

	manager->parsed_code[manager->parsed_code_length + 1] = info;
	manager->parsed_code_length++;
}

void codemanager_free(parsed_code_manager *manager) {
	// This may throw an error
	for (int i = 0; i < manager->parsed_code_allocated_length; i++) {
		lineinfo_free(manager->parsed_code[i]);
	}
	free(manager);
}

