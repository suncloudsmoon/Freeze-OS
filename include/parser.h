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
 * parser.h
 *
 *  Created on: Jul 18, 2021
 *      Author: suncloudsmoon
 */

#ifndef SRC_PARSER_H_
#define SRC_PARSER_H_

#include "stringobj.h"
#include "vm.h"

// I think you can free this directly
typedef struct {
	void *data;
	Type type;

	bool isInGC; // Whether the variable is in garbage collection
} parsed_arg_context_t;

LineInfo* parse(string_t *line, VirtualMachine *vm);
parsed_arg_context_t* parse_arg(string_t *arg, VirtualMachine *vm);
string_t* parse_remove_quotes(string_t *data);

#endif /* SRC_PARSER_H_ */
