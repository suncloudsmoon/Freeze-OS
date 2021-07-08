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
	int value1, value2;
	Operator op;
} if_vars_t;

static void snippetInterpreter(VirtualMachine *vm);

// If Loop functions
static void ifloop_readUntilEnd(VirtualMachine *vm);
static bool ifloop_interpret_values(if_vars_t *vars);
static LogicGate ifloop_get_logic_gate(string_t *token, VirtualMachine *vm);
static if_vars_t* ifloop_get_values(string_t *condition, VirtualMachine *vm);

// For Loop Functions
ForLoop* forloop_init();
void forloop_free(ForLoop *loop);

// NOTE: Everything is an initialization, include the declaration, which is just something setting to NULL

void interpret_full_if_statement(LineInfo *info, VirtualMachine *vm) {

	bool isAllowed = ifloop_statement_interpret(info, vm);
	if (isAllowed) {
		ifloop_readUntilEnd(vm);
	} else {
		int readStatus = 0;
		do {
			string_t *line = string_init();

			readStatus = readLine(vm->location, line);

			LineInfo *check = splitIntoTokens(line, vm);

			if ((string_equals(check->token, vm->else_condition))
					|| (string_equals(check->token, vm->else_if_condition)
							&& ifloop_statement_interpret(check, vm))) {
				ifloop_readUntilEnd(vm);
				break;
			} else if (string_equals(vm->end_function_name, check->token)
					&& string_equalsignorecase(vm->if_condition, check->restLine[0])) {
				break;
			}

			// Freeing resources
			lineinfo_free(check);
			string_free(line);
		} while (readStatus != EOF);
	}

}

bool ifloop_statement_interpret(LineInfo *info, VirtualMachine *vm) {
	// If: 5 < 10, AND, 10 < 20
	bool isAllowed = false;
	if (info->restLineLength > 1) {
		for (int token = 0; token < info->restLineLength - 2; token += 2) {
			// First: like "5 < 10"
			if_vars_t *vars1 = ifloop_get_values(info->restLine[token], vm);
			bool eval1 = ifloop_interpret_values(vars1); // Individual statements are evaluated on their own

			// Like: AND
			LogicGate gate = ifloop_get_logic_gate(info->restLine[token + 1],
					vm);

			// Second: like 10 < 20
			if_vars_t *vars2 = ifloop_get_values(info->restLine[token + 2], vm);
			bool eval2 = ifloop_interpret_values(vars2);

			switch (gate) {
			case AND_LOGIC_GATE:
				isAllowed = eval1 && eval2;
				break;
			case OR_LOGIC_GATE:
				isAllowed = eval1 || eval2;
				break;
			default:
				break;
			}

			// Free Resources
			free(vars1);
			free(vars2);
		}
	} else {
		if (info->restLineLength == 0) {
			return false;
		}
		if_vars_t *vars1 = ifloop_get_values(info->restLine[0], vm);
		isAllowed = ifloop_interpret_values(vars1);

		free(vars1);

	}
	return isAllowed;
}

static void ifloop_readUntilEnd(VirtualMachine *vm) {
	int readStatus = 0;
	while (true) {
		string_t *line = string_init();

		readStatus = readLine(vm->location, line);

		LineInfo *info = splitIntoTokens(line, vm);

		if (string_startswith_s(vm->else_if_condition, line)
				|| string_startswith_s(vm->else_condition, line)
				|| string_startswith_s(vm->end_function_name, line)) {
			break;
		}

		interpret(info, vm);

		// Freeing Resources
		lineinfo_free(info);
		string_free(line);

		if (readStatus == EOF) {
			printf("EOF Exit\n");
			exit(0);
		}
	}

}

static bool ifloop_interpret_values(if_vars_t *vars) {
	switch (vars->op) {
	case EQUALS_OPERATOR:
		return vars->value1 == vars->value2;
	case GREATER_THAN_OPERATOR:
		return vars->value1 > vars->value2;
	case LESS_THAN_OPERATOR:
		return vars->value1 < vars->value2;
	case GREATHER_THAN_OR_EQUALS_OPERATOR:
		return vars->value1 >= vars->value2;
	case LESS_THAN_OR_EQUALS_OPERATOR:
		return vars->value1 <= vars->value2;
	default:
		return false;
	}

}

static LogicGate ifloop_get_logic_gate(string_t *token, VirtualMachine *vm) {
	LogicGate gate;
	if (string_equals(token, vm->and_logic_gate)) {
		gate = AND_LOGIC_GATE;

	} else if (string_equals(token, vm->or_logic_gate)) {
		gate = OR_LOGIC_GATE;

	} else {
		throwException(OTHER_EXCEPTION);
	}

	return gate;
}

static if_vars_t* ifloop_get_values(string_t *condition, VirtualMachine *vm) {
	if_vars_t *values = (if_vars_t*) safe_calloc(1, sizeof(if_vars_t));

	string_t *tempOperator; // Is this a memory leak?
	if (string_contains_s(vm->equals_operator, condition)) {
		values->op = EQUALS_OPERATOR;
		tempOperator = vm->equals_operator;

	} else if (string_contains_s(vm->greater_than_or_equals_operator,
			condition)) {
		values->op = GREATHER_THAN_OR_EQUALS_OPERATOR;
		tempOperator = vm->greater_than_or_equals_operator;

	} else if (string_contains_s(vm->less_than_or_equals_operator, condition)) {
		values->op = LESS_THAN_OR_EQUALS_OPERATOR;
		tempOperator = vm->less_than_or_equals_operator;

	} else if (string_contains_s(vm->greater_than_operator, condition)) {
		values->op = GREATER_THAN_OPERATOR;
		tempOperator = vm->greater_than_operator;

	} else if (string_contains_s(vm->less_than_operator, condition)) {
		values->op = LESS_THAN_OPERATOR;
		tempOperator = vm->less_than_operator;
	}

	int conditionIndex = string_indexof_s(tempOperator->string, condition);

	string_t *first = string_substring(0, conditionIndex, condition);
	string_t *second = string_substring(conditionIndex + tempOperator->length,
			condition->length, condition);

	// First part of the condition
	if (getVariableContext(first) == IS_STRING) {
		// Do something
	} else {
		Variable *var = varmanager_parsevariable(first, vm->manager);
		if (var == NULL) {
			// It must be a number, no exceptions for now
			values->value1 = strtol(first->string, NULL, 10);
		} else {
			if (var->type == INTEGER_TYPE) {
				values->value1 = *((int*) var->data);
			} else {
				throwException(OTHER_EXCEPTION);
			}

		}
	}

	// Second part of the condition
	if (getVariableContext(second) == IS_STRING) {
		// Do something
	} else {
		Variable *var = varmanager_parsevariable(second, vm->manager);
		if (var == NULL) {
			// It must be a number, no exceptions for now
			values->value2 = strtol(second->string, NULL, 10);
		} else {
			if (var->type == INTEGER_TYPE) {
				values->value2 = *((int*) var->data);
			} else {
				throwException(OTHER_EXCEPTION);
			}

		}
	}

	return values;
}

void interpret_for(LineInfo *info, VirtualMachine *vm) {
	vm->vars = forloop_init();

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

	} else if (string_contains_s(vm->greater_than_or_equals_operator,
			condition)) {
		string_concat_s(vm->greater_than_or_equals_operator, operator);

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
			|| string_equals(vm->greater_than_or_equals_operator, operator)) {
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
	do {
		string_t *line = string_init();
		readStatus = readLine(vm->location, line);

		if (vm->vars->lineLength + 1 > vm->vars->allocatedLineLength) { // @suppress("Field cannot be resolved")
			vm->vars->info = (LineInfo**) realloc(vm->vars->info, // @suppress("Field cannot be resolved")
					10 * sizeof(LineInfo*));

			vm->vars->allocatedLineLength += 10; // @suppress("Field cannot be resolved")
		}

		vm->vars->lineLength++; // @suppress("Field cannot be resolved")
		vm->vars->info[vm->vars->lineLength - 1] = splitIntoTokens(line, vm); // @suppress("Field cannot be resolved")

		if (string_equals(vm->end_function_name,
				vm->vars->info[vm->vars->lineLength - 1]->token)
				&& string_equalsignorecase(vm->for_function_name,
						vm->vars->info[vm->vars->lineLength - 1]->restLine[0])) {
			break;
		}
		string_free(line);

	} while (readStatus != EOF);

	// Reading from existing lines
	long *i = vm->vars->i;
	if (string_equals(vm->vars->condition, vm->less_than_operator)) {
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
			vm->greater_than_or_equals_operator)) {
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

	// forloop_free(vm->vars);

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

ForLoop* forloop_init() {
	ForLoop *loop = (ForLoop*) safe_calloc(1, sizeof(ForLoop));
	return loop;
}

void forloop_free(ForLoop *loop) {
	// TODO: Use a local variable manager inside the for loop and free it when done after the scope!
	for (int i = 0; i < loop->lineLength; i++) { // @suppress("Field cannot be resolved")
		lineinfo_free(loop->info[i]); // @suppress("Field cannot be resolved")
	}
	string_free(loop->condition);
	free(loop->i);
	free(loop);
}
