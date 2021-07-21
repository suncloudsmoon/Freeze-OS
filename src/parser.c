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
 * parser.c
 *
 *  Created on: Jul 18, 2021
 *      Author: suncloudsmoon
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <ctype.h>

#include "../include/parser.h"
#include "../include/gc.h"

LineInfo* parse(string_t *line, VirtualMachine *vm) {
	LineInfo *info = lineinfo_init();

	if (string_startswith_s(vm->comment_function_name, line)) {
		info->token = vm->comment_function_name;
		return info;
	}

	int colonIndex = string_indexof(':', line);

// Safety First!
	if (colonIndex <= 0) {
		throwException(SYNTAX_ERROR);
	}

	// Getting the token
	info->token = string_substring(0, colonIndex, line);
	string_tolowercase(info->token);

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

// Parses an argument, like a string: "Hello, World" or integer: 5
parsed_arg_context_t* parse_arg(string_t *arg, VirtualMachine *vm) {
	parsed_arg_context_t *c = malloc(sizeof(parsed_arg_context_t));

	// 5 or ""
	char first_letter = tolower(string_charat(0, arg));

	if (first_letter == '"') {
		// Arg has quotes: "Hello"
		string_t *no_quotes = parse_remove_quotes(arg);

		c->data = (void *) no_quotes;
		c->type = STRING_TYPE;
		c->isInGC = false; // I think this is unnecessary

	} else if (isdigit(first_letter)) {
		// Number in args: 55
		Variable *var = var_init();
		var->data = malloc(sizeof(long int));
		*(var->data) = strtol(arg->string, NULL, 10);

		var->data = var;
		var->name = arg;
		var->type = VARIABLE_TYPE;

		c->data = (void *) var;
		c->type = VARIABLE_TYPE; // Integer in Freeze Programming Language = Long int in C
		c->isInGC = false;

	} else {
		// TODO: complete this function when you add object support
		Variable *var = varmanager_parsevariable(arg, vm->manager);

		c->data = var;
		c->type = VARIABLE_TYPE;
		c->isInGC = true;
	}

	return c;
}

// Removes quotes from strings
string_t* parse_remove_quotes(string_t *data) {
	string_t *newData = string_init();
	int firstQuote = string_indexof('"', data);
	int secondQuote = string_lastindexof('"', data);

	for (int i = firstQuote + 1; i <= secondQuote - 1; i++) {
		string_concat_c(string_charat(i, data), newData);
	}
	return newData;
}
