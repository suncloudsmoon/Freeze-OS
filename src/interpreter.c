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

#include "stringobj.h"
#include "interpreter.h"

typedef struct {
    string_t *token;
    string_t *restLine;
} LineInfo;

typedef struct {
    string_t *line;
    int state;
} ReadStatus;

/// Static prototypes ///
static void readLine(FILE *stuff, ReadStatus *status);
static LineInfo* splitIntoTokens(string_t *line, VirtualMachine *vm);
static void interpret(LineInfo *info, VirtualMachine *vm);
static void exceptionPrinter(int lineNum, char *message, char *customMessage);

void ignition(FILE *stuff) {
    // Debug
    printf("Ignition Started!\n");

    VirtualMachine *vm = (VirtualMachine *) calloc(1, sizeof(VirtualMachine));
    vm->location = stuff;
    vm->lineNum = 0;

    ReadStatus *status;
    do {
        status = (ReadStatus *) calloc(1, sizeof(ReadStatus*));
        status->line = string_init();

        readLine(vm->location, status);

        // Debug
        printf("Read Line From File!\n");

        LineInfo *info = splitIntoTokens(status->line, vm);

        // Debug
        printf("Split Tokens from line!\n");

        interpret(info, vm);

        // Debug
        printf("\nInterpreted a line!\n");

//        free(status);
//        free(info);

    } while (status->state != EOF);
}

static void readLine(FILE *stuff, ReadStatus *status) {
    while (true) {
        char letter = fgetc(stuff);

        if (letter == '\n') {
            status->state = 0;
            break;
        } else if (letter == EOF) {
            status->state = EOF;
            break;
        }
        string_concat_c(letter, status->line);

        // Debug
        printf("Character from File Stream: %c\n", letter);
    }
    // Debug
    printf("All Characters from File Stream: %s\n", status->line->string);
}

static LineInfo* splitIntoTokens(string_t *line, VirtualMachine *vm) {

    int colonIndex = string_indexof(':', line);

    // Debug
    printf("Colon Index: %d\n", colonIndex);

    // Handle Errors
    if (colonIndex == 0) {
        throwException(SYNTAX_ERROR, NULL, vm);
    }

    LineInfo *info = (LineInfo *) malloc(1 * sizeof(LineInfo));


    // Debug
    printf("Done LineInfo Obj\n");

    // Getting the token
    info->token = string_substring(0, colonIndex, line);

    // Debug
    printf("Done info->token\n");
    printf("colonIndex + 1 is %d, line->length is %d\n", colonIndex + 1, line->length);

    info->restLine = string_substring(colonIndex + 1, line->length, line);

    // Debug
    printf("Token, String: %s, %s\n", info->token->string, info->restLine->string);

    return info;
}

static void interpret(LineInfo *info, VirtualMachine *vm) {

    // Debug
    printf("Token, String: %s, %s\n", info->token->string, info->restLine->string);

    if (string_equalsignorecase("print", info->token)) {
        int firstQuote = string_indexof('"', info->restLine);
        int secondQuote = string_lastindexof('"', info->restLine);

        // Debug
        printf("Quote Indexes: %d, %d\n", firstQuote, secondQuote);


        for (int i = firstQuote + 1; i < secondQuote; i++) {
            // printf("Here TO Print!\n");
            printf("%c", string_charat(i, info->restLine));
        }

    } else {
        printf("Syntax Error At Line #%d!\n", vm->lineNum);
    }
    vm->lineNum++;
}

void throwException(RuntimeException exception, char *customMessage, VirtualMachine *vm) {
    // print like Line #%d: (predefined message) and custom message\n
    switch (exception) {

    case BUFFER_OVERFLOW:
        exceptionPrinter(vm->lineNum, "Ah, a buffer overflow has occurred", customMessage);
        break;

    case NULL_POINTER:
        exceptionPrinter(vm->lineNum, "Oh No!!!! I see nothing", customMessage);
        break;

    case SYNTAX_ERROR:
        exceptionPrinter(vm->lineNum, "A syntax error has occurred", customMessage);
        break;

    default:
        break;
    }

}

static void exceptionPrinter(int lineNum, char *message, char *customMessage) {
    if (customMessage != NULL) {
        printf("Line #%d: %s and %s\n", lineNum, message, customMessage);
    } else {
        printf("Line #%d: %s\n", lineNum, message);
    }

}
