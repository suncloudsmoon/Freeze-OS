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
 * gc.h
 *
 *  Created on: Jul 3, 2021
 *      Author: suncloudsmoon
 */

#ifndef SRC_GC_H_
#define SRC_GC_H_

#include "stringobj.h"

#define ARGUMENTLIMIT 1000

typedef enum Type {
	INTEGER_TYPE = 1, FLOAT_TYPE = 2, OBJECT_TYPE = 3, NULL_TYPE = 4
} Type;

typedef enum {
	INTEGER_ARGUMENT_TYPE = 1,
	FLOAT_ARGUMENT_TYPE = 2,
	OBJECT_ARGUMENT_TYPE = 3,
	VARARGS_ARGUMENT_TYPE = 4 // String is also an object like Java
} ArgumentType;

typedef enum {
	VOID_RETURN_TYPE = 0,
	INTEGER_RETURN_TYPE = 1,
	FLOAT_RETURN_TYPE = 2,
	OBJECT_RETURN_TYPE = 3
} ReturnType;

typedef enum {
	IS_STRING = 0,
	IS_SOME_OBJECT = 1
} VariableContext;

typedef struct {
	string_t *name;
	void *data;
	Type type;
} Variable;

// Memory management
typedef struct VariableManager {
	Variable **items;

	// Counters
	int itemsLength;
	int itemsAllocatedLength;

	int gcIndex; // If garbage collection reaches index 10, then index will be reset & garbage collection
} VariableManager; // This is for global varialbe manager, a local one will be implemented later

// Variable Functions
Variable* var_init();
void var_free(Variable *var);

VariableManager* varmanager_init();
void varmanager_addvariable(string_t *reference, void *data, Type type,
		VariableManager *manager);
void varmanager_removevariable(string_t *reference,
		VariableManager *manager);
Variable* varmanager_parsevariable(string_t *reference,
		VariableManager *manager);
void varmanager_free(VariableManager *manager);

VariableContext getVariableContext(string_t *token);

#endif /* SRC_GC_H_ */
