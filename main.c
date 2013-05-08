#include <stdio.h>
#include <stdlib.h>
#include "mc920.h"

int main()
{
    int i;
    float d;
    AdjRel *adj = NULL;
    adj = CreateAdjRel(4);

    srand(time(NULL));
    for(i=0; i < 50; i++){
        d = (float) rand () / ((float) RAND_MAX + 1);
        printf("(%f -  %e) ", d, d);
    }
    double rm = (double)RAND_MAX;
    double res = rm/ (rm + 1.0);
    printf("%f - Hello world!\n", res);
    DestroyAdjRel(&adj);
    return 0;
}
