# Python++ (wpy++)

**Python++** is the compiled systems‑level language in the [WICC](https://github.com/WNU-Project/WICC) toolchain.  
It bridges the readability of Python with the raw control of C and C++, giving developers a way to grow from scripting into full systems programming without leaving the Python family.

---

## ✨ Features

- **C‑like structure, Python‑like readability**  
  Familiar braces, semicolons, and types, but with Python‑inspired clarity.
- **Compiled to native executables**  
  Targets both 64‑bit (working) and 32‑bit (experimental) Windows builds.
- **Strong typing**  
  Variables and functions require explicit types.
- **Low‑level control**  
  Pointers, references, and direct memory access are part of the language.
- **Standard library beginnings**  
  `#include <pyppstdio>` provides basic I/O like `print`.

---

## 📦 Example

```pypp
#include <pyppstdio>

void main() {
    pyppstdio.print("Hello, world!");
    return 0;
}
```
