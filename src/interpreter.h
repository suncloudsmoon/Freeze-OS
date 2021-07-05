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

#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED

#include "vm.h"
#include "conditions.h"
#include "stringobj.h"

// Interpreter logic
/*
 * Starts the virtual machine
 */
void ignition(VirtualMachine *vm);
int readLine(FILE *stuff, string_t *line);
LineInfo* splitIntoTokens(string_t *line, VirtualMachine *vm);
bool interpret(LineInfo *info, VirtualMachine *vm);

// Line Info Stuff
LineInfo* lineinfo_init();
void lineinfo_addsplits(string_t *data, LineInfo *info);
void lineinfo_free(LineInfo *l);

Method* method_init(string_t *name, ReturnType returnType);
void method_free(Method *method);

// Functions in the Freeze Programming Language
void interpret_print(LineInfo *info, VirtualMachine *vm);
void interpret_write(LineInfo *info);
void interpret_print_special(LineInfo *info);

// Helper methods for parsing print and more
string_t* removeQuotes(string_t *data);

#endif // INTERPRETER_H_INCLUDED
