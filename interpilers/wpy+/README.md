# wpy+ — Python+ Interpiler (WICC)

This folder contains the **Python+ interpiler**, part of the  
**WICC (WNU Interpiler and Compiler Collection)**.  
Python+ is the first language in the **Python Series**, designed as a  
bridge from Python’s readability to C’s structure.

---

## 🌍 Project Philosophy

- **Python+** introduces C‑style discipline (`main()`, `#include`, `return success;`)  
  while keeping Python‑like readability.  
- It is **interpiled**: you can run code directly, or transpile it into C.  
- This makes Python+ the **first rung** in the WNU ladder toward systems programming.

---

## 📂 Repository Layout\

```text
wpy+/ 
├── main.c # Entry point for the interpiler
├── Makefile # Build rules
├── build.ps1 # PowerShell build script (clean, build, run)
├── wpy+.exe # Generated executable (after build)
└── README.md # This file
```

📜 License
This project is part of the WNU Project and is licensed under the GNU General Public License v3.0 or later. See the [LICENSE](LICENSE.md) file for details.

[Python+ Interpiler Choco Package](https://community.chocolatey.org/packages/wpyplus/1.0.0)