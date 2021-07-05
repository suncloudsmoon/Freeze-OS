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
 * conditions.c
 *
 *  Created on: Jul 3, 2021
 *      Author: suncloudsmoon
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "conditions.h"
#include "vm.h"
#include "gc.h"
#include "safedefault.h"
#include "interpreter.h"

static void snippetInterpreter(VirtualMachine *vm);

// Everything is an initialization, include the declaration, which is just something setting to NULL
void interpret_varinitialization(string_t *name, string_t *valueInString,
		VariableManager *vars) {
	// Add to gc
}

void interpret_artimetic(string_t *beforeValue, string_t *operator,
		string_t *afterValue, VariableManager *vars) {

}

void interpret_if(LineInfo *info, VirtualMachine *vm) {

}

void interpret_for(LineInfo *info, VirtualMachine *vm) {
	vm->vars = (ForLoop*) calloc(1, sizeof(ForLoop));

// i -> 0 (like i = 0)
	string_t *initialization = info->restLine[0];
	int equalsIndex = string_indexof_s(vm->setting_equal_operator->string,
			initialization);

	string_t *variableName = string_substring(0,
			string_indexof_s(vm->setting_equal_operator->string,
					initialization), initialization);

	string_t *valueInString = string_substring(
			equalsIndex + vm->setting_equal_operator->length,
			initialization->length, initialization);

	// Need to allocate first?
	vm->vars->i = (long*) safe_calloc(1, sizeof(long));
	*(vm->vars->i) = strtol(valueInString->string, NULL, 10);

	varmanager_addvariable(variableName, vm->vars->i, INTEGER_TYPE,
			vm->manager);

// i < 5
	// TODO: Pass this to the If loop
	string_t *condition = info->restLine[1];

	string_t *operator = string_init(); // for now
	if (string_contains_s(vm->equals_operator, condition)) {
		string_concat_s(vm->equals_operator, operator);

	} else if (string_contains_s(vm->greather_than_or_equals_operator,
			condition)) {
		string_concat_s(vm->greather_than_or_equals_operator, operator);

	} else if (string_contains_s(vm->less_than_or_equals_operator, condition)) {
		string_concat_s(vm->less_than_or_equals_operator, operator);

	} else if (string_contains_s(vm->greater_than_operator, condition)) {
		string_concat_s(vm->greater_than_operator, operator);

	} else if (string_contains_s(vm->less_than_operator, condition)) {
		string_concat_s(vm->less_than_operator, operator);
	}
	vm->vars->condition = operator;

	int conditionIndex = string_indexof_s(operator->string, condition);
	string_t *conditionValue;
	if (string_equals(vm->less_than_or_equals_operator, operator)
			|| string_equals(vm->greather_than_or_equals_operator, operator)) {
		conditionValue = string_substring(conditionIndex + 2, condition->length,
				condition);
	} else {
		conditionValue = string_substring(conditionIndex + 1, condition->length,
				condition);
	}
	vm->vars->conditionArgument = strtol(conditionValue->string, NULL, 10);

// i++
	string_t *increment = info->restLine[2];
	// There might be a bug here
	int plusIndex = string_indexof_s(vm->addition_operator->string, increment);
	string_t *incrementValue = string_substring(
			plusIndex + vm->addition_operator->length, increment->length,
			increment);
	vm->vars->increment = strtol(incrementValue->string, NULL, 10);

	vm->vars->info = (LineInfo**) calloc(10, sizeof(LineInfo*)); // @suppress("Field cannot be resolved")
	vm->vars->lineLength = 0; // @suppress("Field cannot be resolved")
	vm->vars->allocatedLineLength = 10; // @suppress("Field cannot be resolved")

	// Reading lines inside the for loop
	int readStatus = 0;
	string_t *threeLetters;
	do {
		string_t *line = string_init();
		readStatus = readLine(vm->location, line);

		if (vm->vars->lineLength + 1 > vm->vars->allocatedLineLength) { // @suppress("Field cannot be resolved")
			vm->vars->info = (LineInfo**) realloc(vm->vars->info, // @suppress("Field cannot be resolved")
					10 * sizeof(LineInfo*));

			vm->vars->allocatedLineLength += 10; // @suppress("Field cannot be resolved")
		}

		threeLetters = string_substring(0, 3, line);

		vm->vars->lineLength++; // @suppress("Field cannot be resolved")
		vm->vars->info[vm->vars->lineLength - 1] = splitIntoTokens(line, vm); // @suppress("Field cannot be resolved")

		string_free(line);

	} while (readStatus != EOF
			&& !string_equalsignorecase(vm->end_function_name, threeLetters));

	// Reading from existing lines
	long *i = vm->vars->i;
	if (string_equals(vm->vars->condition, vm->greater_than_operator)) {
		for (; *i < vm->vars->conditionArgument; *i += vm->vars->increment) {
			snippetInterpreter(vm);
		}

	} else if (string_equals(vm->vars->condition,
			vm->less_than_or_equals_operator)) {
		for (; *i <= vm->vars->conditionArgument; *i += vm->vars->increment) {
			snippetInterpreter(vm);
		}

	} else if (string_equals(vm->vars->condition, vm->greater_than_operator)) {
		for (; *i > vm->vars->conditionArgument; *i += vm->vars->increment) {
			snippetInterpreter(vm);
		}

	} else if (string_equals(vm->vars->condition,
			vm->greather_than_or_equals_operator)) {
		// ) is the same thing as >=, but in a shorter form
		for (; *i >= vm->vars->conditionArgument; *i += vm->vars->increment) {
			snippetInterpreter(vm);
		}

	} else if (string_equals(vm->vars->condition, vm->equals_operator)) {
		for (; *i == vm->vars->conditionArgument; *i += vm->vars->increment) {
			snippetInterpreter(vm);
		}
	}

	varmanager_removevariable(variableName, vm->manager);
	for (int i = 0; i < vm->vars->lineLength; i++) { // @suppress("Field cannot be resolved")
		lineinfo_free(vm->vars->info[i]); // @suppress("Field cannot be resolved")
	}

	if (readStatus == EOF) {
		printf("\nEncountered a EOF, exiting the program!\n");
		exit(0);
	}
}

static void snippetInterpreter(VirtualMachine *vm) {
	for (int line = 0; line < vm->vars->lineLength - 1; line++) { // @suppress("Field cannot be resolved")
		interpret(vm->vars->info[line], vm); // @suppress("Field cannot be resolved")
	}

}
