#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

int main(void)
{
    srand(time(NULL));
    unsigned long datum = rand();
    printf("RAND DAT: %lu\n", datum);
    char num_p = 56;
    datum <<= 8;
    printf("SHIFTED DAT: %lu\n", datum);
    datum |= num_p; 
    printf("BITWISE-OR DAT: %lu\n", datum);
    datum &= UCHAR_MAX;
    printf("DATA: %lu\n", datum);
    return 0;
}