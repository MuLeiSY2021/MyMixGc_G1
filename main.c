#include <stdio.h>
#include <time.h>
#include "MyMalloc.h"

int main() {
    VirtualMemory_p virtualMemory = newVirtualMemory();
    srand((unsigned int) time(NULL));
    for (int i = 0; i < 64; ++i) {

        long tmp = (long )myMalloc(rand()%12,virtualMemory);
        printf("0x%lx",tmp);
        if(i % 2 == 0) {
            myFree((void *)tmp,virtualMemory);
            printf(" FREE");
        }
        putchar('\n');
        virtualMemoryDis(virtualMemory);
    }
    return 0;
}
