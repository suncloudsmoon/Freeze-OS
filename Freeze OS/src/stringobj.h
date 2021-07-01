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

#ifndef STRINGOBJ_H_INCLUDED
#define STRINGOBJ_H_INCLUDED

#include <stdbool.h>

 // String object like Java Strings
 typedef struct {
    char *string;
    int length;
    int allocatedLength;
} string_t;

string_t* string_init();

char string_charat(int index, string_t *story);

int string_compareto(string_t *main, string_t *second);

int string_comparetoignorecase(string_t *main, string_t *second);

void string_concat(char *text, string_t *story);

void string_concat_c(char letter, string_t *story);

bool string_contains(char *s, string_t *story);

bool string_equalsignorecase(string_t *one, string_t *story);

string_t* string_copyvalueof(char *text);

bool string_endswith(char *suffix, string_t *story);

int string_indexof(char ch, string_t *story);

bool string_isempty(string_t *story);

int string_lastindexof(char ch, string_t *story);

int string_length(string_t *story);

/**
 * Replaces a char with another char. For example, if you want to replace 'H' with 'Z' in "Hello World", the result would be
 * "Zello World".
 * @return Returns true if there is any char to be replaced in the first place
 */
void string_replace(char oldChar, char newChar, string_t *story);

/**
 * Beginning (inclusive) to the end of the string.
 * @return A new String that contains the substring
 */
string_t* string_substring(int beginIndex, int endIndex, string_t *story);

string_t** string_split(char *regex, string_t *story);

string_t* string_tolowercase(string_t *story);

void string_free(string_t *story);

/**
 * gets() specially made for the string object
 */
void getsa(string_t *story);

#endif // STRINGOBJ_H_INCLUDED
