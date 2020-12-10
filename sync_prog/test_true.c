#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    for(int i = 1; i <= 400000; ++i)
    {
        fprintf(stderr, "VALUE: %d\n", i);
    }
    return 0;
}
