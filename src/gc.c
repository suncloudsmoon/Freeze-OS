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

#include "../include/gc.h"
#include "../include/vm.h"
#include "../include/stringobj.h"

Variable* var_init() {
	return (Variable*) safe_calloc(1, sizeof(Variable));
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

	// Variables Allocation
	manager->items = (Variable**) safe_calloc(MANAGER_ALLOC_SIZE,
			sizeof(Variable*));
	manager->itemsAllocatedLength = MANAGER_ALLOC_SIZE;

	// Math Engine Vars Allocation
	manager->vars = (te_variable**) safe_calloc(MANAGER_ALLOC_SIZE,
			sizeof(te_variable*));
	manager->varsAllocatedLength = MANAGER_ALLOC_SIZE;

	// Garbage Collection Indicators
	manager->gcIndex = 0;

	return manager;
}

// reference is the name of the variable
void varmanager_addvariable(string_t *reference, void *data, Type type,
		VariableManager *manager) {
	// Actual Variable Items Collector
	if (manager->itemsLength >= manager->itemsAllocatedLength) {
		// Is this safe?
		manager->items = (Variable **) safe_realloc(manager->items, MANAGER_ALLOC_SIZE * sizeof(Variable *));
		manager->itemsAllocatedLength += MANAGER_ALLOC_SIZE;
	}
	manager->items[manager->itemsLength] = var_init();
	manager->items[manager->itemsLength]->name = reference;
	manager->items[manager->itemsLength]->data = data;
	manager->items[manager->itemsLength]->type = type;
	manager->itemsLength++;

	// Add that variable to the math engine
	if (type == VARIABLE_TYPE) {
		if (manager->varsLength >= manager->varsAllocatedLength) {
			manager->vars = (te_variable **) safe_realloc(manager->vars, MANAGER_ALLOC_SIZE * sizeof(te_variable));
			manager->varsAllocatedLength += MANAGER_ALLOC_SIZE;
		}
		manager->vars[manager->varsLength] = (te_variable *) safe_calloc(1, sizeof(te_variable));
		manager->vars[manager->varsLength]->name = reference->string;
		manager->vars[manager->varsLength]->address = data;
		manager->varsLength++;
	}

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

