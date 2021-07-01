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

#include "stringobj.h"
#include "interpreter.h"
#include "safedefault.h"

typedef struct {
	string_t *token;
	string_t **restLine;

	// Counters
	int restLineLength;
	int restLineAllocatedLength;
} LineInfo;

/// Static prototypes ///
VirtualMachine* vm_init(FILE *stuff);
static int readLine(FILE *stuff, string_t *line);
static LineInfo* splitIntoTokens(string_t *line, VirtualMachine *vm);
static void interpret(LineInfo *info, VirtualMachine *vm);

static void restline_add(string_t *data, LineInfo *info);
static void interpret_print(LineInfo *restLine);
static void interpret_write(LineInfo *info);
static string_t* removeQuotes(string_t *data);

// Free Methods
static void lineinfo_free(LineInfo *l);
static void vm_free(VirtualMachine *vm);

// Exception Stuff
static void exceptionPrinter(int lineNum, char *message, bool isErrno);
static void waitToExit(int code);

void ignition(FILE *stuff) {
	// Debug
	printf("Ignition Started!\n");

	VirtualMachine *vm = vm_init(stuff);

	int readStatus = 0;
	do {
		string_t *line = string_init();
		readStatus = readLine(vm->location, line);

		// Debug
		printf("All Characters from File Stream: %s\n", line->string);

		LineInfo *info = splitIntoTokens(line, vm);

		// Debug
		printf("Split Tokens from line!\n");

		// In the debugging mode
		printf("\n");
		interpret(info, vm);
		printf("\n\n");

		// Debug
		printf("Interpreted a line!\n");

		lineinfo_free(info);

	} while (readStatus != EOF);

	vm_free(vm);
}

VirtualMachine* vm_init(FILE *stuff) {
	VirtualMachine *vm = (VirtualMachine*) calloc(1, sizeof(VirtualMachine));
	vm->location = stuff;
	vm->lineNum = 0;

	vm->numMethods = 500; // For now
	vm->methods = (Method**) calloc(vm->numMethods, sizeof(Method));

	return vm;
}

Method* method_init(string_t *name, ReturnType returnType) {
	Method *method = (Method*) calloc(1, sizeof(Method));

	method->name = name;
	method->returnType = returnType;

	return method;
}

void argumentarray_add(ArgumentType argument, Method *dest) {
	dest->arguments[dest->currentArgumentIndex] = argument;
	dest->currentArgumentIndex++;
}

static int readLine(FILE *stuff, string_t *line) {
	while (true) {
		char letter = safe_fgetc(stuff);

		if (letter == '\n' || letter == EOF) {
			return letter;
		}
		string_concat_c(letter, line);

	}
}

static LineInfo* splitIntoTokens(string_t *line, VirtualMachine *vm) {

	int colonIndex = string_indexof(':', line);

// Debug
	printf("Colon Index: %d\n", colonIndex);

// Safety First!
	if (colonIndex == 0) {
		throwException(SYNTAX_ERROR, vm);
	}

	LineInfo *info = (LineInfo*) calloc(1, sizeof(LineInfo));

	// Getting the token
	info->token = string_substring(0, colonIndex, line);
	info->restLine = (string_t**) calloc(10, sizeof(string_t*));
	info->restLineAllocatedLength = 10;
	info->restLineLength = 0;

	bool isString = false;
	int previousStartingIndex = colonIndex + 1;
	for (int i = colonIndex + 1; i < line->length; i++) {
		char letter = string_charat(i, line);

		if (letter == '"') {
			isString = !isString;
		} else if (!isString && letter == ',') {
			restline_add(string_substring(previousStartingIndex + 1, i, line),
					info);
			previousStartingIndex = i;
		}
	}
	restline_add(string_substring(previousStartingIndex, line->length, line),
			info);

	return info;
}

static void restline_add(string_t *data, LineInfo *info) {
	if (info->restLineLength + 1 > info->restLineAllocatedLength) {
		info->restLine = (string_t**) realloc(info->restLine, 10);
		info->restLineAllocatedLength += 10;
	}
	info->restLineLength++;
	info->restLine[info->restLineLength - 1] = data;
}

static void interpret(LineInfo *info, VirtualMachine *vm) {

	if (string_equalsignorecase(string_copyvalueof("print"), info->token)) {
		interpret_print(info);
	} else if (string_equalsignorecase(string_copyvalueof("write"),
			info->token)) {
		interpret_write(info);
	} else {
		throwException(SYNTAX_ERROR, vm);
	}
	vm->lineNum++;
}

static void interpret_print(LineInfo *info) {

	for (int i = 0; i < info->restLineLength; i++) {
		string_t *token = removeQuotes(info->restLine[i]);
		printf("%s", token->string);
		string_free(token);
	}

}

static void interpret_write(LineInfo *info) {
	// Don't know if info->restLine[0] exists tho??
	// Getting the file path
	string_t *filePath = removeQuotes(info->restLine[0]);
	FILE *writing = fopen(filePath->string, "w");

	// Getting the data to be written to file
	string_t *plainText = removeQuotes(info->restLine[1]);
	fputs(plainText->string, writing);

	fclose(writing);
}

static string_t* removeQuotes(string_t *data) {
	string_t *newData = string_init();
	int firstQuote = string_indexof('"', data);
	int secondQuote = string_lastindexof('"', data);

	for (int i = firstQuote + 1; i < secondQuote; i++) {
		string_concat_c(string_charat(i, data), newData);
	}
	return newData;
}

// Respective Free Methods
static void lineinfo_free(LineInfo *l) {
	string_free(l->token);
	for (int i = 0; i < l->restLineLength; i++) {
		string_free(l->restLine[i]);
	}
	free(l->restLine);
	free(l);
}

static void vm_free(VirtualMachine *vm) {
	fclose(vm->location);
	for (int i = 0; i < vm->numMethods; i++) {
		free(vm->methods[i]);
	}
	free(vm->methods);
	free(vm);
}

// Exception Stuff

void throwException(RuntimeException exception, VirtualMachine *vm) {
// print like Line #%d: (predefined message) and custom message\n
	int lineNum;
	if (vm != NULL) {
		lineNum = vm->lineNum;
	} else {
		lineNum = -1;
	}
	switch (exception) {
	case BUFFER_OVERFLOW:
		exceptionPrinter(lineNum, "Ah, a buffer overflow has occurred", false);
		break;
	case MEMORY_ALLOCATION_EXCEPTION:
		exceptionPrinter(lineNum, NULL, true);
		break;
	case INTEGER_OUT_OF_BOUNDS:
		exceptionPrinter(lineNum, "Accessed an integer out of bounds", false);
		break;
	case NULL_POINTER:
		exceptionPrinter(lineNum, "A null pointer exception occurred", false);
		break;

	case SYNTAX_ERROR:
		exceptionPrinter(lineNum, "A syntax error has occurred", false);
		break;
	default:
		exceptionPrinter(lineNum, NULL, true);
		break;
	}

// For debugging purposes
	waitToExit(exception);
}

static void exceptionPrinter(int lineNum, char *message, bool isErrno) {
	char temp[BUFSIZ];

	if (isErrno) {
		if (lineNum != -1)
			sprintf(temp, "Error on Line %d", lineNum);
		else
			sprintf(temp, "Error");

		perror(temp);
	} else {
		if (lineNum != -1)
			printf("Error on Line %d: %s\n", lineNum, message);
		else
			printf("Error: %s\n", message);
	}

}

static void waitToExit(int code) {
	printf(
			"The program has just ended due to an error, press any key to exit...");
	getchar();
	exit(code);
}
