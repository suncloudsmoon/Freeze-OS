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
 * gc.c
 *
 *  Created on: Jul 3, 2021
 *      Author: suncloudsmoon
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "gc.h"
#include "vm.h"
#include "stringobj.h"

Variable* var_init() {
	Variable *var = (Variable*) safe_calloc(1, sizeof(Variable));
	return var;
}

void var_free(Variable *var) {
	string_free(var->name);
	free(var->data);
	free(var);
}

// Initializing the variable manager, which will allocate memory to the struct & of its contents
VariableManager* varmanager_init() {
	VariableManager *manager = (VariableManager*) safe_calloc(1,
			sizeof(VariableManager));
	manager->items = (Variable**) safe_calloc(10, sizeof(Variable*));
	manager->itemsAllocatedLength = 10;
	manager->itemsLength = 0;
	manager->gcIndex = 0;

	return manager;
}

// reference is the name of the variable
void varmanager_addvariable(string_t *reference, void *data, Type type,
		VariableManager *manager) {
	manager->itemsLength++;
	manager->items[manager->itemsLength - 1] = var_init();
	manager->items[manager->itemsLength - 1]->name = reference;
	manager->items[manager->itemsLength - 1]->data = data;
	manager->items[manager->itemsLength - 1]->type = type;
}

// Removes a variable from memory
void varmanager_removevariable(string_t *reference, VariableManager *manager) {
	for (int i = 0; i < manager->itemsLength; i++)
		if (string_equals(reference, manager->items[i]->name))
			var_free(manager->items[i]);

}

// Returns data about the variable
Variable* varmanager_parsevariable(string_t *reference,
		VariableManager *manager) {
	for (int i = 0; i < manager->itemsLength; i++)
		if (string_equals(reference, manager->items[i]->name))
			return manager->items[i];

	return NULL;
}

void varmanager_free(VariableManager *manager) {
	// TODO: Complete this method
	for (int i = 0; i < manager->itemsLength; i++)
		var_free(manager->items[i]);
}

VariableContext getVariableContext(string_t *token) {
	return string_contains("\"", token) ? IS_STRING : IS_SOME_OBJECT;
}
