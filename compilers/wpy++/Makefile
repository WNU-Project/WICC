# Compiler and flags
CC = C:\msys64\mingw64\bin\gcc.exe
CFLAGS = -Wall -Wextra -std=c11

# Output executable name
TARGET = wpy++.exe

# Source files
SRCS = main.c lexer.c parser.c pyppintoasm.c asmintoobject.c objectintoexe.c pyppintoasm32.c asmintoobject32.c objectintoexe32.c pyppgraphics.c gra.pyppintoasm.c gra.asmintoobject.c gra.objectintoexe.c
OBJS = $(SRCS:.c=.o)

# Default build
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lgdi32 -luser32

# Compile .c to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	del /Q $(OBJS) $(TARGET) 2>nul || rm -f $(OBJS) $(TARGET)

.PHONY: all clean