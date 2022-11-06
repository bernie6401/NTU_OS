#include "syscall.h"
main() {
    int i;
    for(i = 0; i < 5; i++) {
        Sleep(10000000);
        PrintInt(10);
    }
    return 0;
}