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
 * test.c
 *
 *  Created on: Jun 29, 2021
 *      Author: suncloudsmoon
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <string.h>

#include "test.h"
#include "stringobj.h"

static void substringStuff();

// Global variables
string_t *obj;

// Tests if there is something wrong with the implementation of stringobj.c
void test_string() {
	 obj = string_init();

	// Initial Printing of the string to see if it works properly
	printf("Initial String: %s, Expected Result: [Nothing]\n", obj->string);

	// Initial size of string
	printf("Size of string: %d, Expected Result: %d\n", obj->length, (int) strlen(obj->string));

	// Adding Strings together
	char *toAdd = "Hello ";
	string_concat(toAdd, obj);

	// Seeing if it actually appended
	printf("After Addition of String: %s [Length: %d], Expected Result: %s [Length: %d]\n", obj->string, obj->length, toAdd, (int) strlen(toAdd));

	////////////////////////
	// Appending a character to string
	char *letterExpected = "Hello W";
	char letter = 'W';
	string_concat_c(letter, obj);

	// Seeing if the character got appended to string
	printf("Added a character: %s, Expected Result: %s\n ", obj->string, letterExpected);

	// Doing some substring stuff
	substringStuff();
}

static void substringStuff() {
	char *expected = "Hell";
	string_t *somethingnew = string_substring(0, 3, obj);
	printf("Substringed: %s, Expected: %s\n", somethingnew->string, expected);
}
