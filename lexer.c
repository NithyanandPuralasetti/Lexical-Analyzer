#include "lexer.h"

const char *keywords[32] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "int", "long", "register", "return", "short", "signed", "sizeof", "static",
    "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"
};

const char single_operators[16] = {
    '+', '-', '*', '/', '%', '=', '>', '<',
    '&', '|', '^', '~', '!', '?', ':', '.'
};

const char *double_operators[21] = {
    "++", "--", "+=", "-=", "*=", "/=", "%=",
    "==", "!=", ">=", "<=", "&&", "||",
    "<<", ">>", "&=", "|=", "^=", "<<=", ">>=", "->"
};

const char punctuators[8] = {'(', ')', '{', '}', '[', ']', ';', ','};

// Stack for bracket and parenthesis tracking
char open_symbols[100];
int open_lines[100];
int count = 0;

/* Removes extra whitespace, single-line (//), and multi-line (/*) comments */
char *removespace(Token *token, char *buffer)
{
    int i = 0, j = 0;

    while (token->data[j] != '\0')
    {
        // Preserve intentional newlines, remove redundant consecutive ones
        if (token->data[j] == '\n')
        {
            if (i > 0 && buffer[i - 1] != '\n')
            {
                buffer[i++] = '\n';
            }
            j++;
            continue;
        }

        // Compress multiple spaces into a single space
        if (isspace((unsigned char)token->data[j]))
        {
            if (i > 0 && buffer[i - 1] != ' ' && buffer[i - 1] != '\n')
            {
                buffer[i++] = ' ';
            }
            j++;
            continue;
        }

        // Strip single-line comments
        if (token->data[j] == '/' && token->data[j + 1] == '/')
        {
            j += 2;
            while (token->data[j] != '\n' && token->data[j] != '\0')
            {
                j++;
            }
            continue;
        }

        // Strip multi-line comments
        if (token->data[j] == '/' && token->data[j + 1] == '*')
        {
            j += 2;
            while (token->data[j] != '\0' && !(token->data[j] == '*' && token->data[j + 1] == '/'))
            {
                j++;
            }
            if (token->data[j] != '\0')
            {
                j += 2;
            }
            continue;
        }

        buffer[i++] = token->data[j++];
    }

    buffer[i] = '\0';
    return buffer;
}

/* Performs lexical analysis and prints tokens, literals, and syntax errors */
void tokenize(const char *src)
{
    count = 0;
    int i = 0;
    int line = 1;

    printf("\n%-10s | %-20s | %-20s\n", "LINE NO", "TOKEN TYPE", "LEXEME");
    printf("--------------------------------------------------\n");

    while (src[i] != '\0')
    {
        if (src[i] == '\n')
        {
            line++;
            i++;
            continue;
        }

        if (isspace((unsigned char)src[i]))
        {
            i++;
            continue;
        }

        // Preprocessor directives
        if (src[i] == '#')
        {
            char temp[128];
            int k = 0;
            while (src[i] != '\0' && src[i] != '\n')
            {
                if (k < 127)
                    temp[k++] = src[i++];
                else
                    i++;
            }
            temp[k] = '\0';

            if (strncmp(temp, "#include", 8) == 0)
            {
                char *open = strchr(temp, '<');
                char *close = strchr(temp, '>');
                char *first_quote = strchr(temp, '"');
                char *second_quote = first_quote ? strchr(first_quote + 1, '"') : NULL;

                if (open && !close)
                    printf(RED "Error [Line %d]: Lexical error - Missing closing '>' in #include\n" RESET, line);
                else if (first_quote && !second_quote)
                    printf(RED "Error [Line %d]: Lexical error - Missing closing '\"' in #include\n" RESET, line);
                else if (!open && !first_quote)
                    printf(RED "Error [Line %d]: Lexical error - Expected '<' or '\"' in #include\n" RESET, line);
                else
                    printf("%-10d | %-20s | %-20s\n", line, "PREPROCESSOR", temp);
            }
            else
            {
                printf("%-10d | %-20s | %-20s\n", line, "PREPROCESSOR", temp);
            }
            continue;
        }

        // String literals
        if (src[i] == '"')
        {
            char temp[128];
            int k = 0;
            temp[k++] = src[i++];
            while (src[i] != '\0' && src[i] != '"' && src[i] != '\n')
            {
                temp[k++] = src[i++];
            }
            if (src[i] != '"')
            {
                printf(RED "Error [Line %d]: Lexical error - Unterminated string literal\n" RESET, line);
            }
            else
            {
                temp[k++] = src[i++];
                temp[k] = '\0';
                printf("%-10d | %-20s | %-20s\n", line, "STRING LITERAL", temp);
            }
            continue;
        }

        // Character constants
        if (src[i] == '\'')
        {
            char temp[32];
            int k = 0;
            temp[k++] = src[i++];

            if (src[i] == '\'')
            {
                i++;
                printf(RED "Error [Line %d]: Lexical error - Empty character constant\n" RESET, line);
                continue;
            }

            if (src[i] == '\\')
            {
                temp[k++] = src[i++];
                if (src[i] != '\0' && src[i] != '\n')
                    temp[k++] = src[i++];
            }
            else if (src[i] != '\'' && src[i] != '\0' && src[i] != '\n')
            {
                temp[k++] = src[i++];
            }

            if (src[i] != '\'')
            {
                while (src[i] != '\0' && src[i] != '\'' && src[i] != '\n')
                {
                    if (k < 30)
                        temp[k++] = src[i];
                    i++;
                }
                if (src[i] == '\'')
                {
                    temp[k++] = src[i++];
                    temp[k] = '\0';
                    printf(RED "Error [Line %d]: Lexical error - Multi-character constant '%s'\n" RESET, line, temp);
                }
                else
                {
                    printf(RED "Error [Line %d]: Lexical error - Unterminated character literal\n" RESET, line);
                }
            }
            else
            {
                temp[k++] = src[i++];
                temp[k] = '\0';
                printf("%-10d | %-20s | %-20s\n", line, "CHARACTER CONSTANT", temp);
            }
            continue;
        }

        // Keywords, arrays, and identifiers
        if (isalpha((unsigned char)src[i]) || src[i] == '_')
        {
            char temp[128];
            int k = 0;
            while (isalnum((unsigned char)src[i]) || src[i] == '_')
            {
                temp[k++] = src[i++];
            }
            temp[k] = '\0';

            if (is_keyword(temp))
            {
                printf("%-10d | %-20s | %-20s\n", line, "KEYWORD", temp);
            }
            else
            {
                int peek = i;
                while (src[peek] == ' ')
                    peek++;

                if (src[peek] == '[')
                {
                    char arr[128];
                    snprintf(arr, sizeof(arr), "%s", temp);
                    int index = strlen(arr);
                    while (src[peek] == '[')
                    {
                        arr[index++] = src[peek++];
                        while (src[peek] != '\0' && src[peek] != ']' && src[peek] != ';')
                        {
                            if (src[peek] != ' ')
                                arr[index++] = src[peek];
                            peek++;
                        }
                        if (src[peek] == ']')
                            arr[index++] = src[peek++];
                    }
                    arr[index] = '\0';
                    printf("%-10d | %-20s | %-20s\n", line, "ARRAY", arr);
                    i = peek;
                    continue;
                }
                printf("%-10d | %-20s | %-20s\n", line, "IDENTIFIER", temp);
            }
            continue;
        }

        // Numerical constants
        if (isdigit((unsigned char)src[i]) || (src[i] == '.' && isdigit((unsigned char)src[i + 1])))
        {
            char temp[64];
            int k = 0, is_float = 0, dot_count = 0;

            while (isdigit((unsigned char)src[i]) || src[i] == '.')
            {
                if (src[i] == '.')
                {
                    is_float = 1;
                    dot_count++;
                }
                if (k < 63)
                    temp[k++] = src[i++];
            }

            if (isalpha((unsigned char)src[i]) || src[i] == '_')
            {
                while (isalnum((unsigned char)src[i]) || src[i] == '_')
                {
                    if (k < 63)
                        temp[k++] = src[i++];
                }
                temp[k] = '\0';
                printf(RED "Error [Line %d]: Lexical error - Invalid identifier '%s'\n" RESET, line, temp);
                continue;
            }

            temp[k] = '\0';

            if (dot_count > 1)
            {
                printf(RED "Error [Line %d]: Lexical error - Invalid float literal '%s'\n" RESET, line, temp);
                continue;
            }

            if (is_float)
                printf("%-10d | %-20s | %-20s\n", line, "FLOAT CONSTANT", temp);
            else
                printf("%-10d | %-20s | %-20s\n", line, "DECIMAL CONSTANT", temp);

            continue;
        }

        // Multi-character operators
        int matched_double = 0;
        for (int d = 0; d < 21; d++)
        {
            if (src[i] == double_operators[d][0] && src[i + 1] == double_operators[d][1])
            {
                printf("%-10d | %-20s | %s\n", line, "DOUBLE OPERATOR", double_operators[d]);
                i += 2;
                matched_double = 1;
                break;
            }
        }
        if (matched_double)
            continue;

        // Single-character operators
        if (is_single_operator(src[i]))
        {
            printf("%-10d | %-20s | %c\n", line, "OPERATOR", src[i]);
            i++;
            continue;
        }

        // Punctuators and delimiters
        if (is_punctuator(src[i]))
        {
            char ch = src[i];
            if (ch == '(' || ch == '{' || ch == '[')
            {
                if (count < 100)
                {
                    open_symbols[count] = ch;
                    open_lines[count] = line;
                    count++;
                }
            }
            else if (ch == ')' || ch == '}' || ch == ']')
            {
                if (count == 0)
                {
                    printf(RED "Error [Line %d]: Unmatched closing delimiter '%c'\n" RESET, line, ch);
                }
                else
                {
                    count--;
                    char last_open = open_symbols[count];
                    int last_line = open_lines[count];
                    if ((ch == ')' && last_open != '(') ||
                        (ch == '}' && last_open != '{') ||
                        (ch == ']' && last_open != '['))
                    {
                        printf(RED "Error [Line %d]: Mismatched delimiter '%c' (opened '%c' at Line %d)\n" RESET,
                               line, ch, last_open, last_line);
                    }
                }
            }

            printf("%-10d | %-20s | %c\n", line, "SPECIAL SYMBOL", ch);
            i++;
            continue;
        }

        // Unrecognized symbols
        printf("%-10d | %-20s | %c\n", line, "UNKNOWN", src[i]);
        printf(RED "Error [Line %d]: Lexical error - Unrecognized symbol '%c'\n" RESET, line, src[i]);
        i++;
    }

    // Verify all opened delimiters were closed
    for (int k = 0; k < count; k++)
    {
        printf(RED "Error [Line %d]: Missing closing delimiter for '%c'\n" RESET, open_lines[k], open_symbols[k]);
    }
}

/* Checks if string matches any of the 32 C keywords */
Status is_keyword(char *temp)
{
    for (int i = 0; i < 32; i++)
    {
        if (strcmp(temp, keywords[i]) == 0)
        {
            return success;
        }
    }
    return failure;
}

/* Checks if character is a valid single-character operator */
Status is_single_operator(char ch)
{
    for (int i = 0; i < 16; i++)
    {
        if (ch == single_operators[i])
        {
            return success;
        }
    }
    return failure;
}

/* Checks if character is a recognized punctuator or bracket */
Status is_punctuator(char ch)
{
    for (int i = 0; i < 8; i++)
    {
        if (ch == punctuators[i])
        {
            return success;
        }
    }
    return failure;
}