# Python++ VS Code Extension

This extension provides comprehensive support for the Python++ programming language in Visual Studio Code.

## Features

- **Syntax Highlighting**: Full syntax highlighting for Python++ (.pypp) files
- **IntelliSense**: Code completion and hover information
- **Code Snippets**: Useful code snippets for common Python++ constructs
- **Auto-completion**: Bracket matching and auto-closing pairs
- **Build Integration**: Compile and run Python++ files directly from VS Code

## Language Support

### Syntax Highlighting
- Keywords: `if`, `else`, `while`, `for`, `return`, `func`, `void`, `int`, `char`, `string`
- Special keywords: `success`, `failure`
- Libraries: `pyppstdio`, `graphics`
- Comments: Line comments (`//`) and block comments (`/* */`)
- Strings and numbers
- Operators and punctuation

### Code Snippets

Type these prefixes and press Tab to insert code snippets:

- `main` - Create a main function
- `print` - Print statement
- `include-stdio` - Include pyppstdio library
- `include-graphics` - Include graphics library
- `gfx-init` - Initialize graphics window
- `gfx-clear` - Clear graphics with color
- `gfx-text` - Draw text on graphics
- `gfx-rect` - Draw rectangle on graphics
- `gfx-loop` - Start graphics event loop
- `if` - If statement
- `ifelse` - If-else statement
- `while` - While loop
- `for` - For loop

## Commands

### Compile Python++ File (`Ctrl+Shift+B`)
Compiles the current Python++ file using the wpy++ compiler. Automatically detects if it's a graphics or console application.

### Compile and Run (`F5`)
Compiles and immediately runs the Python++ program.

## Getting Started

1. Install this extension
2. Make sure you have the wpy++ compiler in your PATH
3. Create a new file with `.pypp` extension
4. Start coding in Python++!

### Example Console Program

```python++
#include <pyppstdio>

void main() {
    pyppstdio.print("Hello, World!");
}
```

### Example Graphics Program

```python++
#include <graphics>

void main() {
    graphics.Init("My Window", 800, 600);
    graphics.Clear("white");
    graphics.DrawText(100, 100, "Hello, Graphics!");
    graphics.Loop();
}
```

## Requirements

- Visual Studio Code 1.85.0 or higher
- wpy++ compiler (from the WICC project)

## Release Notes

### 1.0.0
- Initial release
- Basic syntax highlighting
- Code snippets
- Build and run integration
- Hover information

## Contributing

This extension is part of the WICC (WNU-Project) initiative. For issues and contributions, please visit the [GitHub repository](https://github.com/WNU-Project/WICC).

## License

This project is licensed under the same terms as the WICC project.