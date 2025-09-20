# WICC — WNU Interpiler and Compiler Collection

**WICC** is the official compiler suite of the **WNU Project**.  
Just as GCC (GNU Compiler Collection) unified the C family, WICC unifies the **Python Series** — a ladder of languages designed to guide learners from Python’s readability to C’s raw power.

---

## 🌍 Philosophy

WICC is not just a toolchain. It is a **bridge** — a Bridge — between worlds:

- From **Python**’s simplicity to **C**’s discipline.
- From **interpreted scripting** to **compiled systems programming**.
- From **beginners** to **builders of movements**.

Each language in the Python Series is a rung on the ladder. WICC ensures they all share one foundation.

---

## 🐍 The Python Series Roadmap

### 1. Python+ (Interpiled)
- The **entry point** for Python users.
- Structured like C, readable like Python.
- Requires `#include <pypstdio>` for I/O.
- Entry point: `func main() { ... }`.
- Returns `success` or `failure` instead of numeric codes.
- Executed via **interpiler** (interpreter + transpiler).

### 2. Python++ (Compiled)
- The **systems-level upgrade**.
- Adds pointers, references, and low-level control.
- Strong typing enforced.
- Compiles to machine code via WICC backend.
- Philosophy: *Python meets C++*.

### 3. Python# (Interpiled)
- The **modern application language**.
- Async/await built in.
- Native support for GUIs, networking, and game engines.
- Interpiled for flexibility and rapid development.
- Philosophy: *Python for apps, games, and enterprise*.

---

## ⚙️ WICC Architecture

- **Frontends**:  
  - `wicc-python+`  
  - `wicc-python++`  
  - `wicc-python#`

- **Middle Layer (IR)**:  
  A shared intermediate representation for optimization and cross-language consistency.

- **Backends**:  
  - C code generation (bridge to systems programming).  
  - Bytecode interpreter (fast prototyping).  
  - Native machine code (future goal).

---

## 🚀 Example Usage

```bash
# Run Python+ directly
wicc-python+ hello.p+ --run

# Transpile Python+ to C
wicc-python+ hello.p+ -o hello.c

# Compile Python++ to native
wicc-python++ game.p++ -o game.exe

# Run Python# with runtime
wicc-python# app.p# --run
```
