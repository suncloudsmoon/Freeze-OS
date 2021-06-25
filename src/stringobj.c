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

// String object like Java Strings

// Static Prototypes
static void growMem(int howMuch, string_t *mem);
static void growArrayMem(int howMuch, string_t **mem);

string_t* string_init() {
    string_t *string = (string_t *) calloc(1, sizeof(string_t));
    string->string = (char *) calloc(10, sizeof(char));
    string->allocatedLength = 10;

    return string;
}

char string_charat(int index, string_t *story) {
    // Add safety if accessed out of bounds, return NULL
    if (index >= story->length || index < 0) {
        return NULL;
    } else {
        return story->string[index];
    }
}

int string_compareto(string_t *main, string_t *second) {
    return strcmp(main->string, second->string);
}

int string_comparetoignorecase(string_t *main, string_t *second) {
    return strcmp(tolower(main->string), tolower(second->string));
}

// 5 + 1 = 6; allocated 1
void string_concat(char *text, string_t *story) {
    int textLength = strlen(text);

    if (story->length + textLength > story->allocatedLength) {
        int numAdd = (story->length + textLength) - story->allocatedLength + 10;
        growMem(numAdd, story);
        memset(story->string, 0, numAdd * sizeof(char));

        story->allocatedLength += numAdd;
    }
    strcat(story->string, text);
    story->length += textLength;
}

void string_concat_c(char letter, string_t *story) {

    if (story->length + 1 > story->allocatedLength) {
        int numAdd = (story->length + 1) - story->allocatedLength + 10;
        growMem(numAdd, story);
        //memset(story->string, 0, numAdd);
        story->allocatedLength += numAdd;
    }

    // Convert the char to string temporarly
    char temp[2];
    temp[0] = letter;
    temp[1] = '\0';

    strncat(story->string, temp, 1);
    story->length++;
}

bool string_contains(char *s, string_t *story) {
    return strstr(s, story->string) != NULL;
}

bool string_equalsignorecase(char *one, string_t *story) {
    return strcmp(tolower(one), tolower(story->string)) == 0;
}

string_t* string_copyvalueof(char *text) {
    string_t *story = string_init();
    string_concat(text, story);

    return story;
}

// Later implementation
bool string_endswith(char *suffix, string_t *story) {

}

// First occurrence of the :
int string_indexof(char ch, string_t *story) {

    for (int i = 0; i < story->length; i++) {
        if (ch == string_charat(i, story)) {
            return i;
        }
    }

    return -1;
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

// Inclusive to exclusive
string_t* string_substring(int beginIndex, int endIndex, string_t *story) {
    // Safety First!
    if (beginIndex < 0 || endIndex > story->length) {
        // Throw an exception with line number
        return NULL;
    }

    string_t *newString = string_init();

    char tempString[story->length - beginIndex + 1];
    int tempCounter = 0;

    for (int i = beginIndex; i < endIndex; i++) {
        tempString[tempCounter] = string_charat(i, story);
        tempCounter++;
    }
    string_concat(tempString, newString);

    return newString;
}

string_t** string_split(char *regex, string_t *story) {
    int allocatedMem = 10;
    int memCounter = 0;

    string_t **tokens = (string_t **) calloc(allocatedMem, sizeof(string_t *));

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

void string_free(string_t *story) {
    free(story->string);
    free(story);
}

/**
 * gets() specially made for the string object
 */
void getsa(string_t *story) {
    char input;
    while (input = getchar()) {
        if (input == '\n' || input == '\0' || input == EOF) {
            break;
        } else {
            string_concat(&input, story);
        }
    }
}


// Static Methods
static void growMem(int howMuch, string_t *mem) {
    mem->string = (char *) realloc(mem->string, howMuch);
}

static void growArrayMem(int howMuch, string_t **mem) {
    mem = (string_t **) realloc(mem, howMuch);
}
