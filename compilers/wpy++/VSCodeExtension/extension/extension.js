// Extension for Python++ language support
const vscode = require('vscode');

/**
 * Get the currently selected compiler path from configuration
 */
function getSelectedCompiler() {
    const config = vscode.workspace.getConfiguration('python++');
    return config.get('selectedCompiler', 'C:\\WICC\\Python++\\wpy++.exe');
}

/**
 * Get available compilers from configuration
 */
function getAvailableCompilers() {
    const config = vscode.workspace.getConfiguration('python++');
    return config.get('availableCompilers', [
        {
            name: "WPY++ (Default)",
            path: "C:\\WICC\\Python++\\wpy++.exe",
            description: "Default Python++ compiler"
        }
    ]);
}

/**
 * @param {vscode.ExtensionContext} context
 */
function activate(context) {
    console.log('Python++ extension is now active!');

    // Create status bar item to show selected compiler
    const statusBarItem = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Left, 100);
    statusBarItem.command = 'python++.selectCompiler';
    statusBarItem.tooltip = 'Click to select Python++ compiler';
    
    function updateStatusBar() {
        const compiler = getSelectedCompiler();
        const compilerName = compiler.split(/[\\\/]/).pop(); // Get filename only
        statusBarItem.text = `$(tools) ${compilerName}`;
        
        // Only show when a .pypp file is active
        const activeEditor = vscode.window.activeTextEditor;
        if (activeEditor && activeEditor.document.languageId === 'python++') {
            statusBarItem.show();
        } else {
            statusBarItem.hide();
        }
    }

    // Update status bar when active editor changes
    vscode.window.onDidChangeActiveTextEditor(updateStatusBar);
    vscode.workspace.onDidChangeConfiguration(e => {
        if (e.affectsConfiguration('python++.selectedCompiler')) {
            updateStatusBar();
        }
    });
    
    updateStatusBar(); // Initial update
    context.subscriptions.push(statusBarItem);

    // Register a command for selecting Python++ compiler
    let selectCompilerCommand = vscode.commands.registerCommand('python++.selectCompiler', async () => {
        const availableCompilers = getAvailableCompilers();
        const currentCompiler = getSelectedCompiler();
        
        // Create quick pick items
        const items = availableCompilers.map(compiler => ({
            label: compiler.name,
            description: compiler.path,
            detail: compiler.description,
            compilerPath: compiler.path,
            picked: compiler.path === currentCompiler
        }));

        // Add option to browse for custom compiler
        items.push({
            label: "$(folder-opened) Browse for Compiler...",
            description: "Select a custom Python++ compiler",
            detail: "Browse filesystem for compiler executable",
            compilerPath: null
        });

        const selected = await vscode.window.showQuickPick(items, {
            placeHolder: 'Select Python++ Compiler',
            ignoreFocusOut: true,
            matchOnDescription: true
        });

        if (!selected) return;

        let compilerPath = selected.compilerPath;

        // If user chose to browse
        if (compilerPath === null) {
            const result = await vscode.window.showOpenDialog({
                canSelectFiles: true,
                canSelectFolders: false,
                canSelectMany: false,
                filters: {
                    'Executables': ['exe'],
                    'All Files': ['*']
                },
                title: 'Select Python++ Compiler'
            });

            if (result && result[0]) {
                compilerPath = result[0].fsPath;
            } else {
                return;
            }
        }

        // Update configuration
        const config = vscode.workspace.getConfiguration('python++');
        await config.update('selectedCompiler', compilerPath, vscode.ConfigurationTarget.Global);
        
        vscode.window.showInformationMessage(
            `Python++ compiler set to: ${compilerPath}`,
            'Test Compiler'
        ).then(selection => {
            if (selection === 'Test Compiler') {
                // Test the compiler
                const terminal = vscode.window.createTerminal('Python++ Compiler Test');
                terminal.show();
                terminal.sendText(`"${compilerPath}" --version || echo "Compiler test - please check if the path is correct"`);
            }
        });
    });

    context.subscriptions.push(selectCompilerCommand);

    // Register a command for compiling Python++ files
    let compileCommand = vscode.commands.registerCommand('python++.compile', () => {
        const activeEditor = vscode.window.activeTextEditor;
        if (!activeEditor) {
            vscode.window.showWarningMessage('No active Python++ file to compile');
            return;
        }

        const document = activeEditor.document;
        if (document.languageId !== 'python++') {
            vscode.window.showWarningMessage('Current file is not a Python++ file (.pypp)');
            return;
        }

        // Save the file first
        document.save().then(() => {
            const compilerPath = getSelectedCompiler();
            const terminal = vscode.window.createTerminal('Python++ Compiler');
            terminal.show();
            
            // Check if it's a graphics file or regular file
            const text = document.getText();
            if (text.includes('#include <graphics>')) {
                // Compile as graphics application
                terminal.sendText(`"${compilerPath}" "${document.fileName}" --win32`);
            } else {
                // Compile as console application
                terminal.sendText(`"${compilerPath}" "${document.fileName}"`);
            }
        });
    });

    context.subscriptions.push(compileCommand);

    // Register a command for running Python++ files
    let runCommand = vscode.commands.registerCommand('python++.run', () => {
        const activeEditor = vscode.window.activeTextEditor;
        if (!activeEditor) {
            vscode.window.showWarningMessage('No active Python++ file to run');
            return;
        }

        const document = activeEditor.document;
        if (document.languageId !== 'python++') {
            vscode.window.showWarningMessage('Current file is not a Python++ file (.pypp)');
            return;
        }

        // Save and compile first
        document.save().then(() => {
            const compilerPath = getSelectedCompiler();
            const terminal = vscode.window.createTerminal('Python++ Runner');
            terminal.show();
            
            const text = document.getText();
            const filePath = document.fileName;
            const fileDir = filePath.substring(0, filePath.lastIndexOf('\\'));
            
            // Navigate to the file's directory first
            terminal.sendText(`cd /d "${fileDir}"`);
            
            if (text.includes('#include <graphics>')) {
                // Compile and run graphics application
                terminal.sendText(`"${compilerPath}" "${filePath}" --win32 && "${fileDir}\\outgra.exe"`);
            } else {
                // Compile and run console application
                terminal.sendText(`"${compilerPath}" "${filePath}" --no32 && "${fileDir}\\out.exe"`);
            }
        });
    });

    context.subscriptions.push(runCommand);

    // Provide hover information for Python++ keywords
    let hoverProvider = vscode.languages.registerHoverProvider('python++', {
        provideHover(document, position, token) {
            const range = document.getWordRangeAtPosition(position);
            const word = document.getText(range);

            switch (word) {
                case 'pyppstdio':
                    return new vscode.Hover('**pyppstdio**: Standard I/O library for Python++\n\nProvides functions like `print()` for console output.');
                case 'graphics':
                    return new vscode.Hover('**graphics**: Graphics library for Python++\n\nProvides functions for creating windows and drawing graphics using Win32 GDI.');
                case 'main':
                    return new vscode.Hover('**main**: Entry point function\n\nThe main function where program execution begins.');
                default:
                    return null;
            }
        }
    });

    context.subscriptions.push(hoverProvider);
}

function deactivate() {}

module.exports = {
    activate,
    deactivate
};