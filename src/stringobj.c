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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#include "../include/stringobj.h"
#include "../include/safedefault.h"
#include "../include/vm.h"

#define ALLOC_FACTOR 16

// String class implementation like Java Strings

// Static Prototypes
static void growMem(int howMuch, string_t *mem);
static void growArrayMem(int howMuch, string_t **mem);

// Math Functions
static int max(int x, int y);

string_t* string_init() {
	string_t *string = (string_t*) safe_calloc(1, sizeof(string_t));
	string->string = (char*) safe_calloc(ALLOC_FACTOR, sizeof(char));

	string->length = 0; // I don't know if this is necessary
	string->allocatedLength = ALLOC_FACTOR;

	safe_strncpy(string->string, "", string->allocatedLength);

	return string;
}

char string_charat(int index, string_t *story) {
	// Add safety if accessed out of bounds, return NULL
	return (index < 0 || index >= story->length) ? 0 : story->string[index];
}

int string_compareto(string_t *main, string_t *second) {
	return strncmp(main->string, second->string, main->length);
}

int string_comparetoignorecase(string_t *first, string_t *second) {
	// Very inefficient, need to implement the strcmp() myself
	string_t *firstTemp = string_copyvalueof(first->string), *secondTemp =
			string_copyvalueof(second->string);

	// Lower it
	string_tolowercase(firstTemp);
	string_tolowercase(secondTemp);

	int compared = string_compareto(firstTemp, secondTemp);

	// Free resources
	string_free(firstTemp);
	string_free(secondTemp);

	return compared;
}

bool string_equals(string_t *one, string_t *story) {
	// My own implementation because strncmp is buggy for some reason
	if (one->length != story->length)
		return false;
	else
		for (int i = 0; i < one->length; i++)
			if (one->string[i] != story->string[i])
				return false;

	return true;
}

bool string_equalsignorecase(string_t *one, string_t *story) {
	if (one->length != story->length)
		return false;
	else
		for (int i = 0; i < one->length; i++)
			if (tolower(one->string[i]) != tolower(story->string[i]))
				return false;

	return true;
}

// 5 + 1 = 6; allocated 1
void string_concat(char *text, string_t *story) {
	int textLength = strlen(text);

	// Safety
	if (textLength <= 0)
		throwException(INTEGER_OUT_OF_BOUNDS);

	if (story->length + textLength + 1 > story->allocatedLength) {
		int numAdd = (story->length + textLength + 1)
				- (story->allocatedLength)+ ALLOC_FACTOR;
		growMem(numAdd, story);
	}
	safe_strncat(story->string, text, story->allocatedLength, textLength);
	story->length += textLength;
}

void string_concat_s(string_t *text, string_t *story) {
	string_concat(text->string, story);
}

void string_concat_c(char letter, string_t *story) {

	if (story->length + 2 > story->allocatedLength) {
		int numAdd = (story->length + 1)
				- (story->allocatedLength)+ ALLOC_FACTOR;
		growMem(numAdd, story);
	}

	// Convert the char letter to string temporarily
	char temp[2];
	temp[0] = letter;
	temp[1] = '\0';

	safe_strncat(story->string, temp, story->allocatedLength, 1);
	story->length++;
}

/*
 * Must have a NULL at the end
 */
void string_addstrings(string_t *dest, ...) {
	va_list args;

	va_start(args, dest);
	while (true) {
		char *arg = va_arg(args, char*);
		if (arg == NULL)
			break;

		string_concat(arg, dest);
	}
	va_end(args);
}

void string_addstrings_s(string_t *dest, ...) {
	va_list args;

	va_start(args, dest);
	while (true) {
		string_t *arg = va_arg(args, string_t*);
		if (arg == NULL)
			break;

		string_concat_s(arg, dest);
	}
	va_end(args);
}

bool string_contains(char *find, string_t *story) {
	return strstr(story->string, find) != NULL;
}

bool string_contains_s(string_t *find, string_t *story) {
	return strstr(story->string, find->string) != NULL;
}

string_t* string_copyvalueof(char *text) {
	string_t *story = string_init();
	string_concat(text, story);

	return story;
}

bool string_startswith(char *suffix, string_t *story) {
	int suffixLength = strlen(suffix);

	// Safety
	if (suffixLength > story->length)
		throwException(INTEGER_OUT_OF_BOUNDS);

	// "ab", "ab", 12, 01
	for (int i = 0; i < suffixLength; i++)
		if (suffix[i] != string_charat(i, story))
			return false;

	return true;
}

bool string_startswith_s(string_t *suffix, string_t *story) {
	// Safety
	if (suffix->length > story->length)
		throwException(OTHER_EXCEPTION);

	// "ab", "ab", 12, 01
	for (int i = 0; i < suffix->length; i++)
		if (suffix->string[i] != story->string[i])
			return false;

	return true;
}

bool string_startswith_ignorecase_s(string_t *suffix, string_t *story) {
	// Safety
		if (suffix->length > story->length)
			throwException(OTHER_EXCEPTION);

		// "ab", "ab", 12, 01
		for (int i = 0; i < suffix->length; i++)
			if (tolower(suffix->string[i]) != tolower(story->string[i]))
				return false;

		return true;
}

// Checks if the suffix is found at the end of story
bool string_endswith(char *suffix, string_t *story) {
	int suffixLength = strlen(suffix);

	// Safety
	if (suffixLength >= story->length)
		throwException(OTHER_EXCEPTION);

	// "ab", "ab", 12, 01
	for (int i = story->length - suffixLength - 1; i < story->length; i++)
		if (suffix[i] != string_charat(i, story))
			return false;

	return true;
}

bool string_endswith_s(string_t *suffix, string_t *story) {
	// Safety
	if (suffix->length > story->length)
		throwException(OTHER_EXCEPTION);

	// "ab", "ab", 12, 01
	for (int i = story->length - suffix->length - 1; i < story->length; i++)
		if (string_charat(i, suffix) != string_charat(i, story))
			return false;

	return true;
}

// Returns the index of the first occurrence of a char
int string_indexof(char ch, string_t *story) {
	for (int i = 0; i < story->length; i++)
		if (ch == string_charat(i, story))
			return i;

	return -1;
}

int string_indexof_s(char *ch, string_t *story) {
	return (int) strcspn(story->string, ch);
}

bool string_isempty(string_t *story) {
	return story->length == 0;
}

void string_set(char *text, string_t *story) {
	strncpy(story->string, text, story->allocatedLength);
	story->length = strlen(text);
}

int string_lastindexof(char ch, string_t *story) {
	int index = 0;
	for (int i = 0; i < story->length; i++)
		if (ch == story->string[i])
			index = i;

	return index;
}

int string_lastindexof_s(char *ch, string_t *story) {
	string_t *chString = string_copyvalueof(ch);

	if (chString->length > story->length)
		throwException(INTEGER_OUT_OF_BOUNDS);

	for (int i = 0; i < story->length - chString->length; i++)
		for (int j = 0; j < chString->length; j++)
			if (chString->string[j] == story->string[i + j])
				return i;

	return -1;
}

void string_replace(char oldChar, char newChar, string_t *story) {
	for (int i = 0; i < story->length; i++)
		if (story->string[i] == oldChar)
			story->string[i] = newChar;
}

// Inclusive to exclusive
string_t* string_substring(int beginIndex, int endIndex, string_t *story) {
	// Safety First!
	if (beginIndex < 0 || endIndex > story->length)
		throwException(INTEGER_OUT_OF_BOUNDS);

	string_t *newString = string_init();
	for (int index = beginIndex; index < endIndex; index++)
		string_concat_c(story->string[index], newString);

	return newString;
}

string_t** string_split(char *regex, string_t *story) {
	int allocatedMem = 10;
	int memCounter = 0;

	string_t **tokens = (string_t**) calloc(allocatedMem, sizeof(string_t*));

	char *smaller = strtok(story->string, regex);
	while (smaller != NULL) {
		if (memCounter >= allocatedMem - 1) {
			growArrayMem(10, tokens);
			allocatedMem += 10;
		}
		tokens[memCounter] = string_copyvalueof(strtok(story->string, regex));
		memCounter++;
	}
	return tokens;
}

void string_tolowercase(string_t *story) {
	for (int i = 0; i < story->length; i++)
		story->string[i] = tolower(story->string[i]);
}

void string_free(string_t *story) {
	free(story->string);
	free(story);
}

// Static Methods
static void growMem(int howMuch, string_t *mem) {

	int numAdd = mem->allocatedLength + howMuch;
	char *newStr = (char*) safe_calloc(numAdd, sizeof(char));

	safe_strncpy(newStr, mem->string, numAdd);
	free(mem->string);

	mem->string = newStr; // I think this will create a memory leak
	mem->allocatedLength += howMuch;
}

static void growArrayMem(int howMuch, string_t **mem) {
	mem = (string_t**) realloc(mem, howMuch * sizeof(string_t*));
}

////////////////// Functions not directly connected to the string object //////////////////
/*
 * gets() specially made for the string object
 */
void getsa(string_t *story) {
	// TODO: This function is broken :(
	while (true) {
		char input = getchar();
		if (input == '\n' || input == '\0' || input == EOF) {
			break;
		} else {
			string_concat_c(input, story);
		}
	}
}

static int max(int x, int y) {
	return (x > y) ? x : y;
}
