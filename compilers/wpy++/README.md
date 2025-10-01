# Python++ (wpy++)

**Python++** is a compiled, systems‑level language in the [WICC](https://github.com/WNU-Project/WICC) toolchain.  
It combines the readability of Python with the raw control of C and C++, giving developers a natural path from scripting into full systems programming — without leaving the Python family.

---

## ✨ Features

- **C‑like structure, Python‑like readability**  
  Familiar braces, semicolons, and explicit types, with Python‑inspired clarity.
- **Native executables**  
  Compiles directly to Windows executables.
  - ✅ 64‑bit builds are stable and working.
  - ⚠️ 32‑bit builds are experimental and may fail on some setups.
- **Strong typing**  
  Functions and variables require explicit types for clarity and safety.
- **Low‑level control**  
  Pointers, references, and direct memory access are part of the language design.
- **Standard library beginnings**  
  `#include <pyppstdio>` provides basic I/O like `print`.
- **Compiler options**
  - `--no32` to skip optional 32‑bit builds.
  - `--win32` (experimental) to target Win32 GDI graphics.

---

## 📦 Example

A simple “Hello, world!” program in Python++:

```pypp
#include <pyppstdio>

void main() {
    pyppstdio.print("Hello, world!");
    return 0;
}
```

Compile and run:

```bash
wpy++ hello.pypp --no32
out.exe
```

Output:

```Text
Hello, world!
```

## 🚧 Status

- Stable: Core language, parsing, code generation, 64‑bit builds.
- Work in progress: Graphics pipeline (--win32), 32‑bit support.
- Planned: Expanded standard library, cross‑platform support, more language features.

## 📜 License

wpy++ is part of the WNU Project and is licensed under the [GNU GPLv3](https://www.gnu.org/licenses/gpl-3.0.html).

## 🤝 Contributing

Contributions, bug reports, and feature requests are welcome! 
Check out the [WICC](https://github.com/WNU-Project/WICC) repository for project structure and contribution guidelines.
