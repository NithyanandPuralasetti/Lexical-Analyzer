# C Lexical Analyzer

A simple command-line tool written in C that scans C source code files and breaks them down into individual tokens (the building blocks of a program). It cleans up comments and extra spaces, labels every token, and catches common coding and syntax mistakes.

---

## What It Does

- **Cleans Code First:**
  - Removes single-line (`//`) and multi-line (`/* ... */`) comments.
  - Cleans up extra spaces and blank lines while keeping line numbers accurate.
- **Identifies Tokens:**
  - **Keywords:** Recognizes all 32 standard C keywords like `int`, `float`, `if`, `else`, `return`, and `while`.
  - **Variables and Arrays:** Tells the difference between regular variable names and arrays (such as `arr[10]` or `matrix[3][3]`).
  - **Numbers:** Handles whole numbers (like `42`) and decimal numbers (like `3.14` or `.5`).
  - **Characters and Strings:** Correctly reads text inside quotes (`"..."`) and characters (`'...'`), including escape characters like `\n` and `\t`.
  - **Operators:** Handles single symbols (`+`, `-`, `=`, `*`) as well as combined operators (`++`, `==`, `&&`, `<<=`, `->`).
  - **Brackets and Symbols:** Tracks parentheses `()`, curly braces `{}`, square brackets `[]`, semicolons, and commas.
- **Finds Common Errors:**
  - Catches strings left open without a closing quote (`"hello...`).
  - Catches variable names that incorrectly start with numbers (like `9player`).
  - Flags numbers with more than one decimal point (like `12.34.56`).
  - Warns if `#include` lines are missing `<>` or `""`.
  - Tracks open and closing brackets, pointing out mismatched pairs (like `( 5 + 2 ]`) or missing closing brackets with exact line numbers.

---

## Project Files

```text
├── main.c        # Handles command-line arguments and reads the source file
├── lexer.c       # Removes comments and contains the core token-checking logic
├── lexer.h       # Definitions, constants, and function prototypes
├── test.c        # Sample C file used to test different tokens and syntax errors
└── README.md     # Project documentation
```

---

## How to Compile and Run

### Requirements
- A GCC compiler
- Any terminal (Linux, macOS, or WSL / MinGW on Windows)

### 1. Compile the Code
Run this in your terminal:

```bash
gcc main.c lexer.c -o lexer
```

### 2. Run with a C File
Pass the C file you want to inspect:

```bash
./lexer test.c
```

---

## Example Output

When you run it against `test.c`, the program prints a clean table showing the line number, token type, and the code snippet, alongside any errors found:

```text
LINE NO    | TOKEN TYPE           | LEXEME              
--------------------------------------------------
Error [Line 1]: Lexical error - Missing closing '>' in #include
1          | PREPROCESSOR         | #define MAX_ITEMS 100
2          | KEYWORD              | int                 
2          | IDENTIFIER           | main                
2          | SPECIAL SYMBOL       | (                   
2          | SPECIAL SYMBOL       | )                   
3          | SPECIAL SYMBOL       | {                   
4          | KEYWORD              | int                 
4          | IDENTIFIER           | student_count       
4          | OPERATOR             | =                   
4          | DECIMAL CONSTANT     | 10                  
4          | SPECIAL SYMBOL       | ;                   
5          | KEYWORD              | float               
5          | IDENTIFIER           | milk_price          
5          | OPERATOR             | =                   
5          | FLOAT CONSTANT       | 5.75                
5          | SPECIAL SYMBOL       | ;                   
Error [Line 6]: Lexical error - Multi-character constant 'Abn'
...
Error [Line 28]: Lexical error - Invalid identifier '9player_score'
Error [Line 29]: Lexical error - Invalid float literal '12.34.56'
Error [Line 30]: Lexical error - Unterminated string literal
Error [Line 34]: Mismatched delimiter ']' (opened '(' at Line 34)
Error [Line 3]: Missing closing delimiter for '{'
```

---

## Author

- **P. Nithyanand**
- GitHub: [@NithyanandPuralasetti](https://github.com/NithyanandPuralasetti)
