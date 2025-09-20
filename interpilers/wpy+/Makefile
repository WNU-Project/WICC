# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Output executable name
TARGET = wpy+.exe

# Source files
SRCS = main.c lexer.c parser.c interpiler.c
OBJS = $(SRCS:.c=.o)

# Default build
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile .c to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	del /Q $(OBJS) $(TARGET) 2>nul || rm -f $(OBJS) $(TARGET)

.PHONY: all clean