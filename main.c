#include <stdio.h>
#include <stdlib.h>
#include "mc920.h"

int main()
{
    AdjRel *adj = NULL;
    adj = CreateAdjRel(4);
    printf("Hello world!\n");
    DestroyAdjRel(&adj);
    return 0;
}
