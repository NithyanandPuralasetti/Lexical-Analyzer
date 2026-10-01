#include "lexer.h"

Token token;

/* Validates input arguments, reads the source file, and triggers tokenization */
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf(RED "Usage: ./a.out <filename.c>\n" RESET);
        return 1;
    }

    if (!validatefile(argv[1]))
    {
        printf(RED "Invalid file format. Please provide a .c file\n" RESET);
        return 1;
    }

    token.fptr = fopen(argv[1], "r");
    if (token.fptr == NULL)
    {
        printf(RED "Error: Unable to open file '%s'\n" RESET, argv[1]);
        return 1;
    }

    // Determine file size and read contents into memory
    fseek(token.fptr, 0, SEEK_END);
    token.filesize = ftell(token.fptr);
    rewind(token.fptr);

    token.data = malloc(token.filesize + 1);
    if (token.data == NULL)
    {
        printf(RED "Memory allocation failed\n" RESET);
        fclose(token.fptr);
        return 1;
    }

    size_t size = fread(token.data, 1, token.filesize, token.fptr);
    token.data[size] = '\0';
    fclose(token.fptr);

    // Buffer to hold preprocessed code (comments & extra spaces removed)
    char *buffer = malloc(token.filesize + 1);
    if (buffer == NULL)
    {
        printf(RED "Memory allocation failed\n" RESET);
        free(token.data);
        return 1;
    }

    if (removespace(&token, buffer))
    {
        free(token.data);
        token.data = buffer;
    }
    else
    {
        free(buffer);
    }

    tokenize(token.data);

    free(token.data);
    return 0;
}

/* Verifies whether the provided filename ends with .c */
Status validatefile(char *str)
{
    int len = strlen(str);
    if (len >= 2 && strcmp(str + len - 2, ".c") == 0)
    {
        return success;
    }
    return failure;
}