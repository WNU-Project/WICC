# Python+ Interpiler — Release Notes

## v1.0.1 — September 2025

**Refinements & Stability**

This update polishes the first working release of Python+ and lays the groundwork for upcoming character support.

### ✨ New in 1.0.1

- **Improved Lexer**
  - Correctly tokenizes `.` as `TOKEN_DOT` (fixing `pypstdio.print` parsing).
  - Properly emits `TOKEN_STRING` for `"Hello, world!"`.
  - Skips comments (`//` and `/* ... */`) without leaking tokens into the parser.
  - `#include <...>` and `#<...>` directives now reliably produce `TOKEN_INCLUDE`.
- **Parser Enhancements**
  - Recognizes `pypstdio.print` with multiple arguments (strings + identifiers).
  - Enforces correct syntax for `pypstdio.variable.int(name, value);`.
  - AST now distinguishes between `Literal` and `Identifier` children in `Print`.
- **Interpreter Updates**
  - Added a simple symbol table for integer variables.
  - `AST_VAR_DECL` stores variables in memory.
  - `AST_PRINT` resolves identifiers and prints their stored values alongside literals.
  - Example: 
 
    ```pyp
    pypstdio.variable.int(int1, 10);
    pypstdio.print("Hello, world!", int1);
    ```

    Output:  

    ```text
    Hello, world! 10
    ```

### 🛠 Fixes

- Removed stray “Unknown node type” runtime errors by handling `AST_VAR_DECL`, `AST_LITERAL`, and `AST_IDENTIFIER`.
- Cleaned up parser logic to prevent false semantic errors when includes are present.

### ⚠ Known Limitations

- Only integer variables are supported (`int`).
- No arithmetic expressions yet (`int1 + 5` not supported).
- No control flow (`if`, `while`, `for`) yet.
- Only one function (`main`) is executed.

---

## 🚧 Coming Soon — v1.0.2

**Characters Update**

- Support for `pypstdio.variable.char(name, 'X');`
- Lexer support for `'A'` style character literals.
- Parser enforcement of `(name, 'X')` argument structure.
- Interpreter support for storing and printing characters alongside integers.

---

## 🌟 Roadmap

- String variables (`pypstdio.variable.string`)
- Arithmetic expressions and operators
- Control flow (`if`, `while`, `for`)
- Interactive REPL enhancements
- Expanded standard library

---

**WNU Project — Building a language, one update at a time.**
