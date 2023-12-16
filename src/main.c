#include <stdio.h>
#include <string.h>

#include "windex.h"

int main()
{
    reset();
    while (1)
    {
        update(getchar());
        printf("Buffer: %s\n", sentence.buffer);
    }

    return 0;
}