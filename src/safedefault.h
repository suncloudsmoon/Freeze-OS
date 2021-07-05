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
 * safedefault.h
 *
 *  Created on: Jun 29, 2021
 *      Author: suncloudsmoon
 */

#ifndef SRC_SAFEDEFAULT_H_
#define SRC_SAFEDEFAULT_H_

#include "stringobj.h"

// Memory Safety Functions
void* safe_calloc(size_t elements, size_t type_size);
void* safe_realloc(void *data, size_t extra_size);

// Character and String Functions
/*
 * To handle errors if there is an unknown character in file stream.
 */
char safe_fgetc(FILE *stream);

/*
 * Throws an exception if strncat returns a negative value
 */
void safe_strncat(char *dest, size_t destLength, char *src, size_t srcLength);
void safe_strncpy(char *dest, size_t destLength, char *src, size_t srcLength);

// File Handling Functions
FILE* safe_fopen(const char *filepath, const char *mode);
void safe_fputs(const char *str, FILE *stream);
void safe_fclose(FILE *stream);

// Print Functions
void safe_printf(const char *format, ...);

#endif /* SRC_SAFEDEFAULT_H_ */
