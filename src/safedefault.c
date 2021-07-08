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
 * safedefault.c
 *
 *  Created on: Jun 29, 2021
 *      Author: suncloudsmoon
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <string.h>
#include <stdarg.h>

#include "safedefault.h"
#include "vm.h"

// Memory Safety Functions

void* safe_calloc(size_t elements, size_t type_size) {
	void *newPointer = calloc(elements, type_size);
	if (newPointer == NULL)
		throwException(OTHER_EXCEPTION);

	return newPointer;
}

void* safe_realloc(void *data, size_t extra_size) {
	void *newPointer = realloc(data, extra_size);
	if (newPointer == NULL)
		throwException(OTHER_EXCEPTION);

	return newPointer;
}

// Character and String Functions

char safe_fgetc(FILE *stream) {
	char letter = (char) fgetc(stream); // I don't know if this typecasting is necessary
	if (ferror(stream) != 0)
		throwException(OTHER_EXCEPTION);

	return letter;
}

void safe_strncat(char *dest, size_t destLength, char *src, size_t srcLength) {
	if (strncat_s(dest, destLength, src, srcLength) != 0)
		throwException(OTHER_EXCEPTION);
}

void safe_strncpy(char *dest, size_t destLength, char *src, size_t srcLength) {
	if (strncpy_s(dest, destLength, src, srcLength) != 0)
		throwException(OTHER_EXCEPTION);
}

// File Handling Functions

FILE* safe_fopen(const char *filepath, const char *mode) {
	FILE *opened = fopen(filepath, mode);
	if (opened == NULL)
		throwException(OTHER_EXCEPTION);

	return opened;
}

void safe_fputs(const char *str, FILE *stream) {
	if (fputs(str, stream) == EOF)
		throwException(OTHER_EXCEPTION);
}

void safe_fclose(FILE *stream) {
	if (fclose(stream) == EOF)
		throwException(OTHER_EXCEPTION);
}

// Print Functions

void safe_printf(const char *format, ...) {
	va_list args; // va = variable, so it is variable list!
	va_start(args, format);

	if (vprintf(format, args) < 0)
		throwException(OTHER_EXCEPTION);

	va_end(args);
}
