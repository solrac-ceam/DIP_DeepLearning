#ifndef MULTIBANDKERNEL_H_
#define MULTIBANDKERNEL_H_


#include "mc920.h"

/*Agregado por Pablo Fonseca */

typedef struct _multibandkernel {
    AdjRel *A;      /* relação de adjacência  */
    float  **w;     /* pesos dos adjacentes   */
    int nbands;     /* numero de bandas */
} MultibandKernel;

AdjRel             *Rectangular(int nx, int ny);
MultibandKernel    *CreateMultibandKernel(AdjRel *A, int nbands);
int                 MaximumDisplacementMK(MultibandKernel *K);
MultibandKernel    *CreateMultibandRndKernel(int nx, int ny, int nbands);
void                DestroyMultibandKernel(MultibandKernel **K);

/*Fin de Agregado por Pablo Fonseca */

/* Agregado por Carlos Alfaro*/
MultibandKernel *readMultibandKernel(char *filename);
void writeMultibandKernel(MultibandKernel *K, char *filename);
MultibandKernel **generateKernelBank(int nx, int ny, int nbands, int n);

#endif // MULTIBANDKERNEL_H_
