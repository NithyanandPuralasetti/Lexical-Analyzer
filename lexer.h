#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define RED   "\033[1;31m"
#define RESET "\033[0m"

typedef struct
{
    FILE *fptr;
    char *data;
    long filesize;
} Token;

typedef enum
{
    failure,
    success
} Status;

/* Validates that input file has a .c extension */
Status validatefile(char *str);

/* Strips redundant whitespace and comments from the source buffer */
char *removespace(Token *token, char *buffer);

/* Scans source text and prints categorized lexical tokens */
void tokenize(const char *src);

/* Token classification helpers */
Status is_keyword(char *temp);
Status is_single_operator(char ch);
Status is_punctuator(char ch);

#endif