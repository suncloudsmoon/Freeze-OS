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

#include "stringobj.h"
#include "interpreter.h"
#include "safedefault.h"

const unsigned ALLOC_FACTOR = 16;

// String class implementation like Java Strings

// Static Prototypes
static void growMem(int howMuch, string_t *mem);
static void growArrayMem(int howMuch, string_t **mem);

string_t* string_init() {
	string_t *string = (string_t*) calloc(1, sizeof(string_t));
	string->string = (char*) calloc(ALLOC_FACTOR, sizeof(char));
	string->allocatedLength = ALLOC_FACTOR;

	strcpy(string->string, "");

	return string;
}

char string_charat(int index, string_t *story) {
	// Add safety if accessed out of bounds, return NULL
	if (index < 0 || index >= story->length) {
		return 0;
	} else {
		return story->string[index];
	}
}

int string_compareto(string_t *main, string_t *second) {
	return strcmp(main->string, second->string);
}

int string_comparetoignorecase(string_t *first, string_t *second) {

	string_t *firstTemp = string_tolowercase(first);
	string_t *secondTemp = string_tolowercase(second);

	bool compared = strcmp(firstTemp->string, secondTemp->string);

	// Free resources
	string_free(firstTemp);
	string_free(secondTemp);

	return compared;
}

// 5 + 1 = 6; allocated 1
void string_concat(char *text, string_t *story) {
	int textLength = strlen(text);

	// Safety
	if (textLength <= 0) {
		throwException(INTEGER_OUT_OF_BOUNDS, NULL);
	}

	if (story->length + textLength + 1 > story->allocatedLength) {
		int numAdd = (story->length + textLength + 1) - (story->allocatedLength)
				+ ALLOC_FACTOR;
		growMem(numAdd, story);
	}
	safe_strcat(story, text);
	story->length += textLength;
}

void string_concat_s(string_t *text, string_t *story) {
	string_concat(text->string, story);
}

void string_concat_c(char letter, string_t *story) {

	if (story->length + 2 > story->allocatedLength) {
		int numAdd = (story->length + 1) - (story->allocatedLength)
				+ ALLOC_FACTOR;
		growMem(numAdd, story);
	}

	// Convert the char letter to string temporarily
	char temp[2];
	temp[0] = letter;
	temp[1] = '\0';

	safe_strcat(story, temp);
	story->length++;
}

bool string_contains(char *find, string_t *story) {
	return strstr(story->string, find) != NULL;
}

bool string_contains_s(string_t *find, string_t *story) {
	return strstr(story->string, find->string);
}

bool string_equals(string_t *one, string_t *story) {
	return strcmp(one->string, story->string) == 0;
}

bool string_equalsignorecase(string_t *one, string_t *story) {
	return string_comparetoignorecase(one, story) == 0;
}

string_t* string_copyvalueof(char *text) {
	string_t *story = string_init();
	string_concat(text, story);

	return story;
}

bool string_startswith(char *suffix, string_t *story) {
	int suffixLength = strlen(suffix);

	// Safety
	if (suffixLength >= story->length) {
		throwException(OTHER_EXCEPTION, NULL);
	}

	// "ab", "ab", 12, 01
	for (int i = 0; i < suffixLength; i++) {
		if (suffix[i] != string_charat(i, story)) {
			return false;
		}
	}
	return true;
}

bool string_startswith_s(string_t *suffix, string_t *story) {
	int suffixLength = suffix->length;

		// Safety
		if (suffixLength >= story->length) {
			throwException(OTHER_EXCEPTION, NULL);
		}

		// "ab", "ab", 12, 01
		for (int i = 0; i < suffixLength; i++) {
			if (string_charat(i, suffix) != string_charat(i, story)) {
				return false;
			}
		}
		return true;
}

// Checks if the suffix is found at the end of story
bool string_endswith(char *suffix, string_t *story) {
	int suffixLength = strlen(suffix);

	// Safety
	if (suffixLength >= story->length) {
		throwException(OTHER_EXCEPTION, NULL);
	}

	// "ab", "ab", 12, 01
	for (int i = story->length - suffixLength - 1; i < story->length; i++) {
		if (suffix[i] != string_charat(i, story)) {
			return false;
		}
	}
	return true;
}

bool string_endswith_s(string_t *suffix, string_t *story) {
	int suffixLength = suffix->length;

	// Safety
	if (suffixLength >= story->length) {
		throwException(OTHER_EXCEPTION, NULL);
	}

	// "ab", "ab", 12, 01
	for (int i = story->length - suffixLength - 1; i < story->length; i++) {
		if (string_charat(i, suffix) != string_charat(i, story)) {
			return false;
		}
	}
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

int string_lastindexof(char ch, string_t *story) {
	int index = 0;
	for (int i = 0; i < story->length; i++) {
		if (ch == story->string[i]) {
			index = i;
		}
	}

	return index;
}

void string_replace(char oldChar, char newChar, string_t *story) {
	for (int i = 0; i < story->length; i++) {
		if (story->string[i] == oldChar) {
			story->string[i] = newChar;
		}
	}
}

/**
 * gets() specially made for the string object
 */
void getsa(string_t *story) {
	char input;
	while (true) {
		input = getchar();
		if (input == '\n' || input == '\0' || input == EOF) {
			break;
		} else {
			string_concat_c(input, story);
		}
	}
}

// Inclusive to exclusive
string_t* string_substring(int beginIndex, int endIndex, string_t *story) {
	// Safety First!
	if (beginIndex < 0 || endIndex > story->length) {
		// TODO: Throw an exception with line number
		return NULL;
	}

	string_t *newString = string_init();

	char tempString[(endIndex - beginIndex) + 1];
	int tempCounter = 0;
	for (int i = beginIndex; i < endIndex; i++) {
		tempString[tempCounter] = string_charat(i, story);
		tempCounter++;
	}
	tempString[tempCounter] = '\0';

	string_concat(tempString, newString);

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

string_t* string_tolowercase(string_t *story) {
	string_t *temp = string_init();

	for (int i = 0; i < story->length; i++) {
		string_concat_c(tolower(story->string[i]), temp);
	}

	return temp;
}

// Static Methods
static void growMem(int howMuch, string_t *mem) {

	char *newStr = (char*) calloc(mem->allocatedLength + howMuch, sizeof(char));
	strcpy(newStr, mem->string);
	free(mem->string);

	mem->string = newStr;
	mem->allocatedLength += howMuch;
}

static void growArrayMem(int howMuch, string_t **mem) {
	mem = (string_t**) realloc(mem, howMuch);
}

void string_free(string_t *story) {
	free(story->string);
	free(story);
}
