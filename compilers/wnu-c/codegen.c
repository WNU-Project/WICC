#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <llvm-c/Core.h>

static LLVMModuleRef the_module = NULL;
static LLVMBuilderRef builder = NULL;
static LLVMTypeRef i32_type = NULL;
static LLVMTypeRef i8_type = NULL;

// cached printf declaration
static LLVMValueRef printf_fn = NULL;

int codegen_init(const char *module_name) {
    the_module = LLVMModuleCreateWithName(module_name);
    if (!the_module) return -1;
    builder = LLVMCreateBuilder();
    i32_type = LLVMInt32Type();
    i8_type = LLVMInt8Type();
    return 0;
}

// Helper: create function with i32 () signature
static LLVMValueRef create_function(const char *name) {
    LLVMTypeRef func_type = LLVMFunctionType(i32_type, NULL, 0, 0);
    LLVMValueRef fn = LLVMAddFunction(the_module, name, func_type);
    return fn;
}

// Ensure printf declaration exists (vararg: int printf(const char*, ...))
static LLVMValueRef get_printf_decl(void) {
    if (printf_fn) return printf_fn;
    LLVMValueRef existing = LLVMGetNamedFunction(the_module, "printf");
    if (existing) { printf_fn = existing; return printf_fn; }
    LLVMTypeRef param_types[1];
    param_types[0] = LLVMPointerType(i8_type, 0);
    LLVMTypeRef printf_ty = LLVMFunctionType(i32_type, param_types, 1, /*isVarArg=*/1);
    printf_fn = LLVMAddFunction(the_module, "printf", printf_ty);
    return printf_fn;
}

static void codegen_stmt(Ast *stmt, LLVMValueRef function) {
    if (!stmt) return;
    // Return statement
    if (stmt->kind == AST_RETURN_STMT) {
        Ast *expr = stmt->child;
        if (expr && expr->kind == AST_INT_LITERAL) {
            LLVMValueRef ret = LLVMConstInt(i32_type, expr->int_value, 0);
            LLVMBuildRet(builder, ret);
        } else {
            LLVMValueRef ret = LLVMConstInt(i32_type, 0, 0);
            LLVMBuildRet(builder, ret);
        }
        return;
    }

    // Expression statement: handle simple call expressions (printf)
    if (stmt->kind == AST_EXPR_STMT && stmt->child && stmt->child->kind == AST_CALL_EXPR) {
        Ast *call = stmt->child;
        Ast *callee = call->child; // first child is callee identifier
        if (!callee || callee->kind != AST_IDENTIFIER) return;
        const char *name = callee->lexeme ? callee->lexeme : "";
        if (strcmp(name, "printf") == 0) {
            LLVMValueRef pf = get_printf_decl();
            // count args
            int nargs = 0;
            for (Ast *a = callee->next; a; a = a->next) nargs++;
            LLVMValueRef *args = (LLVMValueRef*)malloc(sizeof(LLVMValueRef) * (nargs>0?nargs:1));
            int idx = 0;
            for (Ast *a = callee->next; a; a = a->next) {
                if (a->kind == AST_STRING_LITERAL && a->lexeme) {
                    const char *s = a->lexeme;
                    size_t len = strlen(s);
                    const char *start = s;
                    size_t slen = len;
                    if (len >= 2 && s[0] == '"' && s[len-1] == '"') { start = s + 1; slen = len - 2; }
                    // Create a global string constant and use pointer
                    LLVMValueRef gs = LLVMBuildGlobalStringPtr(builder, start, "strtmp");
                    args[idx++] = gs;
                } else if (a->kind == AST_INT_LITERAL) {
                    args[idx++] = LLVMConstInt(i32_type, a->int_value, 0);
                } else {
                    args[idx++] = LLVMConstInt(i32_type, 0, 0);
                }
            }
            LLVMBuildCall(builder, pf, args, nargs, "calltmp");
            free(args);
        }
        return;
    }
}

static void codegen_function(Ast *fn) {
    if (!fn || fn->kind != AST_FUNC_DEF) return;
    const char *name = fn->name ? fn->name : "fn";
    LLVMValueRef llvm_fn = create_function(name);
    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(llvm_fn, "entry");
    LLVMPositionBuilderAtEnd(builder, entry);

    // find body (child)
    Ast *body = fn->child;
    int did_return = 0;
    if (body && body->kind == AST_COMPOUND_STMT) {
        // iterate statements
        Ast *st = body->child;
        for (; st; st = st->next) {
            if (st->kind == AST_RETURN_STMT) {
                codegen_stmt(st, llvm_fn);
                did_return = 1;
                break;
            }
            // other statements currently ignored
        }
    }
    // If no return was emitted, add a default return 0
    if (!did_return) {
        LLVMValueRef ret = LLVMConstInt(i32_type, 0, 0);
        LLVMBuildRet(builder, ret);
    }
}

int codegen_codegen_ast(Ast *tu) {
    if (!the_module || !builder) return -1;
    if (!tu || tu->kind != AST_TRANSLATION_UNIT) return -1;
    Ast *c = tu->child;
    for (; c; c = c->next) {
        if (c->kind == AST_FUNC_DEF) codegen_function(c);
    }
    return 0;
}

void codegen_print_ir(void) {
    if (!the_module) return;
    char *s = LLVMPrintModuleToString(the_module);
    if (s) {
        printf("%s\n", s);
        LLVMDisposeMessage(s);
    }
}

int codegen_write_ir(const char *path) {
    if (!the_module) return -1;
    char *s = LLVMPrintModuleToString(the_module);
    if (!s) return -1;
    FILE *f = fopen(path, "w");
    if (!f) { LLVMDisposeMessage(s); return -1; }
    fprintf(f, "%s", s);
    fclose(f);
    LLVMDisposeMessage(s);
    return 0;
}

void codegen_dispose(void) {
    if (builder) { LLVMDisposeBuilder(builder); builder = NULL; }
    if (the_module) { LLVMDisposeModule(the_module); the_module = NULL; }
}
