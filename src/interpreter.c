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
 * GOALS:
 * TODO: Make registry keys for the ".fz" and ".fzc" extension using the ifdef windows only with the icon of freeze programming language
 * TODO: make a recycle bin in the OS, in the user folder if possible
 * TODO: Make a instant start option for an application, in which the variable stack is stored to file & retrieved when the application is started, and directly go to the line where the program previously ended
 * TODO: add a built-in simple neural network library for performance
 * TODO: add built-in password protection ("Password Input: pass") using noob encryption built-in & erase input quickly by setting char array (string) to NULL
 * TODO: make ".freeze" to ".fz", and ".fzc" for bytecode
 * TODO: make a "permanent int age = 5" (only global is accepted), where if a file under the class hierarchy in the file system with the directory "Program A\\age.variable" (a txt file) has the age inside it, if the file isn't found then initialize the variable
 * TODO: Implement a designated amount of programs allowed to run at one (including background) as RAM, if more, then use paging
 * TODO: Implement multi-threading in the OS
 * TODO: When assigning variables - x = 5, y = 0, x = y, the x has the same pointer as y, so if y = 10, x = 10
 */

/*
 * Bytecode Syntax:
 * For
 * print
 * end
 * ?
 * i=0,i<5,i++
 * "hello, world"
 * NULL
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "interpreter.h"
#include "vm.h"
#include "gc.h"
#include "conditions.h"
#include "stringobj.h"
#include "safedefault.h"

//////////// The Process Cycle of the Interpreter /////////////////////
void ignition(VirtualMachine *vm) {

	// Debug
	printf("Ignition Started!\n");

	int readStatus = 0;
	do {
		string_t *line = string_init();
		readStatus = readLine(vm->location, line);

		// Debug
		printf("All Characters from File Stream: %s\n", line->string);

		LineInfo *info = splitIntoTokens(line, vm);

		// Debug
		printf("Split Tokens from line!\n");

		// Debug new line characters
		printf("\n");
		interpret(info, vm);
		printf("\n\n");

		// Debug
		printf("Interpreted a line!\n");

		lineinfo_free(info);

	} while (readStatus != EOF);

}

int readLine(FILE *stuff, string_t *line) {
	while (true) {
		char letter = safe_fgetc(stuff);

		if (letter == '\n' || letter == EOF) {
			return letter;
		}
		string_concat_c(letter, line);

	}
}

LineInfo* splitIntoTokens(string_t *line, VirtualMachine *vm) {

	LineInfo *info = lineinfo_init();

	if (string_startswith_s(vm->comment_function_name, line)) {
		info->token = vm->comment_function_name;
		return info;
	} else if (string_startswith_s(vm->new_line_character, line)) {
		info->token = vm->new_line_character;
		return info;
	}

	int colonIndex = string_indexof(':', line);

// Debug
	//printf("\nColon Index: %d\n", colonIndex);

// Safety First!
	if (colonIndex == 0) {
		throwException(SYNTAX_ERROR, vm);
	}

	// Getting the token
	info->token = string_substring(0, colonIndex, line);

	// Allocates & reads the rest of line
	info->restLine = (string_t**) calloc(10, sizeof(string_t*));
	info->restLineAllocatedLength = 10;
	info->restLineLength = 0;

	bool isString = false;
	// int previousStartingIndex = colonIndex + 1;
	string_t *temp = string_init();
	for (int i = colonIndex + 1; i < line->length; i++) {
		char letter = string_charat(i, line);

		if (letter == '"') {
			isString = !isString;
			string_concat_c(letter, temp);
		} else if (!isString) {
			if (letter == ',') {
				lineinfo_addsplits(string_copyvalueof(temp->string), info);
				string_set("", temp);
				// previousStartingIndex = i;
			} else if (letter != ' ') {
				string_concat_c(letter, temp);
			}

		} else {
			string_concat_c(letter, temp);
		}
	}
	lineinfo_addsplits(string_copyvalueof(temp->string), info);

	return info;
}

bool interpret(LineInfo *info, VirtualMachine *vm) {
	// New line
	vm->lineNum++;

	// If the token is end, comment, or a new line character, then don't check the others!
	if (string_equalsignorecase(vm->end_function_name, info->token)) {
		return true;
	} else if (string_equalsignorecase(vm->comment_function_name, info->token)
			|| string_equalsignorecase(vm->new_line_character, info->token)) {
		return false;
	}

	if (string_equalsignorecase(vm->print_function_name, info->token)) {
		interpret_print(info, vm);

	} else if (string_equalsignorecase(vm->write_function_name, info->token)) {
		interpret_write(info);

	} else if (string_equalsignorecase(vm->for_function_name, info->token)) {
		interpret_for(info, vm);

	} else {
		throwException(SYNTAX_ERROR, NULL);
	}

	return false;
}

void interpret_print(LineInfo *info, VirtualMachine *vm) {
	for (int i = 0; i < info->restLineLength; i++) {
		if (getVariableContext(info->restLine[i]) == IS_STRING) {
			string_t *result = removeQuotes(info->restLine[i]);
			for (int j = 0; j < result->length; j++) {
				char letter = string_charat(j, result);
				if (letter == 92 && j+1 < result->length) {
					char escape = string_charat(j+1, result);
					if (escape == 'n') {
						printf("\n");
					} else if (escape == 't') {
						printf("\t");
					}
					j++;
				} else {
					safe_printf("%c", letter);
				}

			}
		} else {
			Variable *var = varmanager_parsevariable(info->restLine[i],
					vm->manager);
			if (var == NULL) {
				return;
			}
			if (var->type == INTEGER_TYPE) {
				safe_printf("%d", *((int*) var->data));
			}
		}
	}
}

void interpret_write(LineInfo *info) {
// Don't know if info->restLine[0] exists tho??
// Getting the file path
	string_t *filePath = removeQuotes(info->restLine[0]);
	FILE *writing = safe_fopen(filePath->string, "w");

// Getting the data to be written to file
	string_t *plainText = removeQuotes(info->restLine[1]);
	safe_fputs(plainText->string, writing);

// Freeing resources
	safe_fclose(writing);
	string_free(filePath);
	string_free(plainText);
}

// Removes quotes from strings
string_t* removeQuotes(string_t *data) {
	string_t *newData = string_init();
	int firstQuote = string_indexof('"', data);
	int secondQuote = string_lastindexof('"', data);

	for (int i = firstQuote + 1; i <= secondQuote - 1; i++) {
		string_concat_c(string_charat(i, data), newData);
	}
	return newData;
}

////////////////// Line Info Stuff ///////////////////////

LineInfo* lineinfo_init() {
	LineInfo *info = (LineInfo*) safe_calloc(1, sizeof(LineInfo));
	info->token = (string_t*) safe_calloc(1, sizeof(string_t*));
	info->restLine = (string_t**) safe_calloc(10, sizeof(string_t*));

	info->restLineAllocatedLength = 10;
	info->restLineLength = 0; // Don't know if this is necessary

	return info;
}

void lineinfo_addsplits(string_t *data, LineInfo *info) {
	if (info->restLineLength + 1 > info->restLineAllocatedLength) {
		info->restLine = (string_t**) safe_realloc(info->restLine, 10);
		info->restLineAllocatedLength += 10;
	}
	info->restLineLength++;
	info->restLine[info->restLineLength - 1] = data;
}

// Respective Free Methods
void lineinfo_free(LineInfo *l) {
	string_free(l->token);
	for (int i = 0; i < l->restLineLength; i++) {
		string_free(l->restLine[i]);
	}
	free(l->restLine);
	free(l);
}

/////////////////////// Method Stuff //////////////////////////////////

Method* method_init(string_t *name, ReturnType returnType) {
	Method *method = (Method*) calloc(1, sizeof(Method));

	method->name = name;
	method->returnType = returnType;

	return method;
}

void method_free(Method *method) {
	string_free(method->name);
	free(method); // for now
}
