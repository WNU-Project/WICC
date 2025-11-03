# Compiler and flags
CC = C:\msys64\mingw64\bin\gcc.exe
CFLAGS = -Wall -Wextra -std=c11

# Try to detect llvm-config to get proper LLVM flags. If llvm-config is not
# available, LLVM_CFLAGS and LLVM_LIBS will be empty and you must set them
# manually or install LLVM development packages.
LLVM_CONFIG ?= llvm-config
LLVM_CFLAGS := $(shell $(LLVM_CONFIG) --cflags 2>/dev/null)
LLVM_LIBS := $(shell $(LLVM_CONFIG) --libs --system-libs 2>/dev/null)

# Output executable name
TARGET = wnucc.exe

# Source files
# If LLVM_LIBS is empty (no llvm-config), use a stub codegen that doesn't
# require LLVM. Otherwise build the real codegen.c.
ifeq ($(strip $(LLVM_LIBS)),)
SRCS = main.c lexer.c parser.c ast.c codegen_stub.c
else
SRCS = main.c lexer.c parser.c ast.c codegen.c
endif
OBJS = $(SRCS:.c=.o)

# Default build
all: info $(TARGET)

$(TARGET): $(OBJS)
	@echo Linking $(TARGET) using: $(if $(LLVM_LIBS),real LLVM libs,$(LLVM_LIBS) is empty -> using stub)
	$(CC) $(CFLAGS) -o $@ $^ $(LLVM_LIBS) -lgdi32 -luser32

# Print detected LLVM flags (for debugging)
show-llvm:
	@echo LLVM_CONFIG = $(LLVM_CONFIG)
	@echo LLVM_CFLAGS = $(LLVM_CFLAGS)
	@echo LLVM_LIBS = $(LLVM_LIBS)

# Print an informational message about what will be built
info:
	@echo "WNUP C build: using $(if $(LLVM_LIBS),real codegen (LLVM),stub codegen (no LLVM detected))"


# Compile .c to .o
%.o: %.c
	$(CC) $(CFLAGS) $(LLVM_CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	del /Q $(OBJS) $(TARGET) 2>nul || rm -f $(OBJS) $(TARGET)

.PHONY: all clean