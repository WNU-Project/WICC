const vscode = require('vscode');

/** @param {vscode.ExtensionContext} context */
function activate(context) {
    console.log('Python+ language support activated');

    const selector = { language: 'python+', scheme: 'file' };

    // Simple completion provider: basic keywords
    const keywords = ['#include', 'return', 'void', 'int', 'print'];
    const completionProvider = vscode.languages.registerCompletionItemProvider(selector, {
        provideCompletionItems(document, position) {
            return keywords.map(k => new vscode.CompletionItem(k, vscode.CompletionItemKind.Keyword));
        }
    });
    context.subscriptions.push(completionProvider);

    // Hover provider for include pseudo-module
    const hoverProvider = vscode.languages.registerHoverProvider(selector, {
        provideHover(document, position) {
            const word = document.getText(document.getWordRangeAtPosition(position));
            if (word === 'pyppstdio') {
                return new vscode.Hover('pyppstdio: standard I/O utilities for Python+ (print, input)');
            }
            return null;
        }
    });
    context.subscriptions.push(hoverProvider);
}

function deactivate() {}

module.exports = { activate, deactivate };
