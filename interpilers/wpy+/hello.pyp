// Include pypstdio for printing
#include <pypstdio>
#include <pypstdio.variable>

/*  Program By
    Tadeo A Gonzalez Gonzalez
    WNU Project
*/

func main() {
    pypstdio.variable.int(int1, 10);
    pypstdio.variable.char(char1, 'A');
    pypstdio.variable.char.str(str1, "Hello from Python+!");
    pypstdio.print("Hello, world!", int1, char1, str1);
    return success;
}
