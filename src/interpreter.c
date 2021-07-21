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
 * TODO: Make a command, like "Music: sin of 30" that creates a wav file and then plays it if "play: "latest song""
 * TODO: Instead of C plugin to the Freeze Language, it will rely instead on a Javascript interpreter for things other than Freeze
 * TODO Optional: If there is a syntax error or a security loophole in Freeze code, let machine learning "auto-patch" it by modifying the software?
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

#include "../include/interpreter.h"
#include "../include/vm.h"
#include "../include/gc.h"
#include "../include/conditions.h"
#include "../include/parser.h"
#include "../include/stringobj.h"
#include "../include/safedefault.h"

//////////// The Process Cycle of the Interpreter /////////////////////
void ignition(VirtualMachine *vm) {
	printf("Ignition Started!\n");

	// First, we read all lines from file stream and parse it into a bunch of tokens
	readAllLines(vm);

	while (vm->lineNum < vm->code_manager->parsed_code_length) {
		interpret(vm->code_manager->parsed_code[vm->lineNum], vm);
	}
}

bool interpret(LineInfo *info, VirtualMachine *vm) {
	// New line
	vm->lineNum++;

	// If the token is end, comment, or a new line character, then don't check the others!
	if (string_equals(vm->end_function_name, info->token)
			|| string_equals(vm->comment_function_name, info->token)
			|| string_equals(vm->new_line_character, info->token)) {
		return true;
	}

	// Conditions first!
	if (string_equals(vm->if_condition, info->token)) {
		interpret_full_if_statement(info, vm);

	} else if (string_equals(vm->for_function_name, info->token)) {
		interpret_for(info, vm);

	} else if (string_equals(vm->print_function_name, info->token)) {
		interpret_print(info, vm);

	} else if (string_equals(vm->write_function_name, info->token)) {
		interpret_write(info);

	} else {
		// none of the functions defined in vm are not found
		throwException(SYNTAX_ERROR);
	}

	return false;
}

void interpret_print(LineInfo *info, VirtualMachine *vm) {
	for (int i = 0; i < info->restLineLength; i++) {
		parsed_arg_context_t *arg_context = parse_arg(info->restLine[i], vm);

		switch (arg_context->type) {
		case STRING_ARGUMENT_TYPE:
			string_t *raw_quote = (string_t*) arg_context->data;

			for (int j = 0; j < raw_quote->length; j++) {
				char letter = string_charat(j, raw_quote);

				if (letter == 92 && j + 1 < raw_quote->length) {
					char escape = string_charat(j + 1, raw_quote);
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
			break;
		case VARIABLE_ARGUMENT_TYPE:
			// %ld for printing long int
			safe_printf("%ld",
					(long int*) ((Variable*) arg_context->data)->data);
			break;
		default:
			break;
		}
	}
}

void interpret_write(LineInfo *info, VirtualMachine *vm) {
	// TODO: check if info->restLine[0] even exists

	// Getting the file path
	parsed_arg_context_t *file_path_context = parse_arg(info->restLine[0], vm);

	// Type Safety
	if (file_path_context->type != STRING_ARGUMENT_TYPE)
		throwException(TYPE_MISMATCH);

	FILE *file = safe_fopen((string_t*) file_path_context->data, "w");

	// Getting the data to be written to file
	parsed_arg_context_t *plain_text_context = parse_arg(info->restLine[1], vm);

	switch (plain_text_context->type) {
	case STRING_ARGUMENT_TYPE:
		safe_fputs(((string_t*) plain_text_context->data)->string, file);
		break;
	case VARIABLE_ARGUMENT_TYPE:
		Variable *var = (Variable *) plain_text_context->data; // Is var a dangling pointer?
		safe_fprintf(file, "%ld", *((long int *) var->data)); // * for deallocating the long int
		break;
	default:
		break; // or throw exception
	}

	// Free resources //

	fclose(file); // close the file first

	// free file_path_context
	string_free((string_t *) file_path_context->data);
	free(file_path_context);

	// free plain_text_context
	string_free((string_t *) plain_text_context->data);
	free(plain_text_context);
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
