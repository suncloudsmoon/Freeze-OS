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

#include "../include/conditions.h"
#include "../include/vm.h"
#include "../include/gc.h"
#include "../include/safedefault.h"
#include "../include/interpreter.h"
#include "../include/parser.h"
#include "../deps/tinyexpr/tinyexpr.h"

// Static Functions
static void snippetInterpreter(VirtualMachine *vm);

// If Loop functions
static void ifloop_readUntilEnd(VirtualMachine *vm);
static bool ifloop_interpret_values(if_vars_t *vars);
static LogicGate ifloop_get_logic_gate(string_t *token, VirtualMachine *vm);
static if_vars_t* ifloop_get_values(string_t *condition, VirtualMachine *vm);

// For Loop Functions
for_loop_t* forloop_init();
void forloop_free(for_loop_t *loop);

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
					&& string_equalsignorecase(vm->if_condition,
							check->restLine[0])) {
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

static if_vars_t* ifloop_get_values(string_t *condition, VirtualMachine *vm) {
	if_vars_t *values = (if_vars_t*) safe_calloc(1, sizeof(if_vars_t));

	string_t *tempOperator = NULL; // Is this a memory leak?
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
	parsed_arg_context_t *firstContext = parse_arg(first, vm);
	values->value1 = ifloop_obj_to_int(firstContext);

	// Second part of the condition
	parsed_arg_context_t *secondContext = parse_arg(second, vm);
	values->value2 = ifloop_obj_to_int(secondContext);

	// Free Resources
	free(firstContext);
	free(secondContext);

	return values;
}

static long int* ifloop_obj_to_int(parsed_arg_context_t *context) {
	switch (context->type) {
	case VARIABLE_TYPE:
		Variable *var = (Variable*) context->data;
		return (long int*) var->data;
	case STRING_TYPE:
		return ifloop_string_to_num((string_t*) context->data);
	default:
		return 0; // or throw an error
	}
}

long int* ifloop_string_to_num(string_t *str) {
	long int *total = malloc(sizeof(long int));
	*total = 0;
	for (int i = 0; i < str->length; i++) {
		*total += (long int) string_charat(i, str);
	}
	return total;
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

for_loop_manager_t* formanager_init() {
	for_loop_manager_t *manager = (for_loop_manager_t*) calloc(1,
			sizeof(for_loop_manager_t));

	manager->loops = (for_loop_t**) calloc(MANAGER_ALLOC_SIZE,
			sizeof(for_loop_t*));
	manager->loops_allocated_length = MANAGER_ALLOC_SIZE;
	manager->loops_length = 0; // TODO: evaluate if this code line is even necessary

	manager->loopCounter = 0;

	return manager;
}

void formanager_add(for_loop_t *loop, for_loop_manager_t *manager) {
	if (manager->loops_length + 1 > manager->loops_allocated_length) {
		manager->loops = (for_loop_t*) realloc(manager->loops,
		MANAGER_ALLOC_SIZE * sizeof(for_loop_t*));
		manager->loops_allocated_length += 10;
	}
	manager->loops[manager->loops_length + 1] = loop;
	manager->loops_length++;
}

void formanager_free(for_loop_manager_t *manager) {
	for (int i = 0; i < manager->loops_length; i++) {
		lineinfo_free(manager->loops[i]);
	}
	free(manager); // I think this is unnecessary
}

typedef struct {
	double *dest;
	te_expr *expression;
} initialize_var_t;

// Abstract Syntax Tree for it
initialize_var_t* ast_initialize_var(string_t *arg, VirtualMachine *vm) {
	initialize_var_t *ast = (initialize_var_t *) calloc(1, sizeof(initialize_var_t));
	as
	int equalsIndex = string_indexof_s(vm->equals_operator, arg);

	// Like i -> 5
	string_t *dest_var = string_substring(0, equalsIndex, arg);
	parsed_arg_context_t *dest_var_context = parse_arg(dest_var, vm);

	if (!dest_var_context->isInGC) {
		switch (dest_var_context->type) {
		case VARIABLE_TYPE:
			varmanager_addvariable(dest_var, (Variable*) dest_var_context->data,
					VARIABLE_TYPE, vm->manager);
			break;
		case STRING_TYPE:
			varmanager_addvariable(dest_var, (string_t*) dest_var_context->data,
					STRING_TYPE, vm->manager);
			break;
		default:
			break; // or throw exception
		}
	}

	// Like 5 + 6; This is where the real arithmetic happens
	string_t *operator_vars = string_substring(equalsIndex, arg->length, arg);
	int parsing_error;
	te_expr *expression = te_compile(operator_vars->string, vm->manager->vars, vm->manager->varsLength, &parsing_error);

	// Free Resources
	string_free(dest_var);
	free(dest_var_context);

	return ast;
}

//static long int parse_arthimetic(string_t *args, VirtualMachine *vm) {
//	long int total = 0;
//
//	int previousIndex = 0;
//	char operator = '+';
//
//	// Use a for loop
//	for (int i = 0; i < args->length; i++) {
//		char letter = string_charat(i, args);
//
//		if (letter == '+' || letter == '-' || letter == '*' || letter == '/') {
//			string_t *num = string_substring(previousIndex, i, args);
//			parsed_arg_context_t *context = parse_arg(num, vm);
//
//			switch (context->type) {
//			case VARIABLE_TYPE:
//				arthimetic_add(total, operator,
//						(long int*) ((Variable*) context->data)->data);
//				break;
//			case STRING_TYPE:
//				long int *string_to_num = ifloop_string_to_num(
//						(string_t*) context->data);
//				arthimetic_add(total, operator, string_to_num);
//				free(string_to_num);
//				break;
//			default:
//				break;
//
//			}
//
//			// Index related chores
//			operator = letter;
//			previousIndex = i;
//
//			// Free resources
//			string_free(num);
//			if (!context->isInGC)
//				free(context->data);
//			free(context);
//		}
//
//	}
//	string_t *num = string_substring(previousIndex, args->length, args);
//	parsed_arg_context_t *context = parse_arg(num, vm);
//	arthimetic_add(total, operator,
//			(long int*) ((Variable*) context->data)->data);
//
//	// Free resources
//	string_free(num);
//	if (!context->isInGC)
//		free(context->data);
//	free(context);
//
//	return total;
//}

static void arthimetic_add(long int *total, char operator, long int *new_num) {
	switch (operator) {
	case '+':
		*total += *new_num;
		break;
	case '-':
		*total -= *new_num;
		break;
	case '*':
		*total *= *new_num;
		break;
	case '/':
		*total /= *new_num;
		break;
	default:
		break;
	}
}

// TODO: make an abstract syntax tree for each function; like the arithmetic engine
void forloop_setup(for_loop_t *loop, LineInfo *info, VirtualMachine *vm) {

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
	Variable *var = var_init();
	var->data = safe_calloc(1, sizeof(long));
	*(var->data) = strtol(valueInString->string, NULL, 10);

	varmanager_addvariable(variableName, loop->i, VARIABLE_TYPE, vm->manager);

// i < 5
	string_t *condition = info->restLine[1];
	if_vars_t vars = ifloop_get_values(condition, vm); // Gets the variable i from the variablemanager

// i++
	string_t *increment = info->restLine[2];
// There might be a bug here
	int plusIndex = string_indexof_s(vm->addition_operator->string, increment);
	string_t *incrementValue = string_substring(
			plusIndex + vm->addition_operator->length, increment->length,
			increment);
	loop->increment = strtol(incrementValue->string, NULL, 10);

	vm->vars->info = (LineInfo**) calloc(10, sizeof(LineInfo*)); // @suppress("Field cannot be resolved")
	vm->vars->lineLength = 0; // @suppress("Field cannot be resolved")
	vm->vars->allocatedLineLength = 10; // @suppress("Field cannot be resolved")

}

void interpret_for(LineInfo *info, VirtualMachine *vm) {
	vm->loop_manager->loopCounter++; // TODO: Initialize the loop counter to -1 to indicate no loop index
	for_loop_t *loop = vm->loop_manager->loops[vm->loop_manager->loopCounter];

// Don't know if this line is necessary tho
	if (loop == NULL) {
		loop = forloop_init();
		forloop_setup(loop, info, vm);
		formanager_add(loop, vm->loop_manager);
	}

	while (ifloop_interpret_values(loop->condition)) {

	}

}

static void snippetInterpreter(VirtualMachine *vm) {
	for (int line = 0; line < vm->vars->lineLength - 1; line++) { // @suppress("Field cannot be resolved")
		interpret(vm->vars->info[line], vm); // @suppress("Field cannot be resolved")
	}

}

for_loop_t* forloop_init() {
	for_loop_t *loop = (for_loop_t*) safe_calloc(1, sizeof(for_loop_t));
	return loop;
}

void forloop_free(for_loop_t *loop) {
// TODO: Use a local variable manager inside the for loop and free it when done after the scope!
	for (int i = 0; i < loop->lineLength; i++) { // @suppress("Field cannot be resolved")
		lineinfo_free(loop->info[i]); // @suppress("Field cannot be resolved")
	}
	string_free(loop->condition);
	free(loop->i);
	free(loop);
}
