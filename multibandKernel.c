#include "multibandKernel.h"

/*Agregado por Pablo Fonseca */

AdjRel *RectangularKernel(int nx, int ny)
{
    AdjRel *A=NULL;
    int     dx,dy,n,i;

    n=nx*ny; //Revisar si es +1

    A = CreateAdjRel(n);
    i=0;
    for (dy=0; dy < ny; dy++)
        for (dx=0; dx < nx; dx++) {
            A->adj[i].dx = dx-nx/2;
            A->adj[i].dy = dy-ny/2;
            A->adj[i].dz = 0;
            i++;
        }

    return(A);
}

/*Agregado por Pablo Fonseca */

MultibandKernel *CreateMultibandKernel(AdjRel *A, int nbands)
{
    MultibandKernel *K=NULL;
    int i,j;

    /* Cria kernel */

    K = (MultibandKernel *)calloc(1,sizeof(MultibandKernel ));
    K->nbands = nbands;
    K->A = CreateAdjRel(A->n);

    K->w = (float **)calloc(A->n,sizeof(float*));
    for(j=0; j< A->n; j++)
        K->w[j] = AllocFloatArray(nbands);      /* Copia a relação de adjacência */

    for (i=0; i < A->n; i++)
        K->A->adj[i] = A->adj[i];

    return(K);
}

int MaximumDisplacementMK(MultibandKernel *K)
{
    int i,dmax=INT_MIN; /* dmax = -infinito */

    for (i=0; i < K->A->n; i++) {
        if (dmax < abs(K->A->adj[i].dx))
            dmax = abs(K->A->adj[i].dx);
        if (dmax < abs(K->A->adj[i].dy))
            dmax = abs(K->A->adj[i].dy);
        if (dmax < abs(K->A->adj[i].dz))
            dmax = abs(K->A->adj[i].dz);
    }

    return(dmax);
}

MultibandKernel *CreateMultibandRndKernel(int nx, int ny, int nbands)
{
    float d;
    int i,dz;
    float sum=0.0, squaredsum=0.0, mean, l2norm;
    AdjRel *A = RectangularKernel(nx,ny);
    MultibandKernel *K = CreateMultibandKernel(A, nbands);

    for(i=0; i < K->A->n; i++){
        for(dz=0; dz < nbands; dz++){
            d = (float) rand () / ((float) RAND_MAX + 1);
            //d = (float) rand();
            sum += d;
            K->w[i][dz] = d;
        }
    }

    mean = sum/(nx*ny*nbands);

    for(i=0; i < K->A->n; i++){
        for(dz=0; dz < nbands; dz++){
            K->w[i][dz] = K->w[i][dz] - mean;
            squaredsum += K->w[i][dz] * K->w[i][dz];
        }
    }

    l2norm = sqrt(squaredsum);

    for(i=0; i < K->A->n; i++){
        for(dz=0; dz < nbands; dz++){
            K->w[i][dz] = K->w[i][dz] / l2norm;
        }
    }

    DestroyAdjRel(&A);
    return K;
}

void DestroyMultibandKernel(MultibandKernel **K)
{
    int i;

    MultibandKernel *aux=*K;

    if (aux!=NULL){
        for(i=0; i < aux->A->n; i++) {
            free(aux->w[i]);
        }
        free(aux->w);
        DestroyAdjRel(&aux->A);
        free(aux);
        *K = NULL;
    }
}

/* Agregado por Carlos Alfaro*/

MultibandKernel *readMultibandKernel(char *filename)
{
    FILE *fp = NULL;
    AdjRel *A = NULL;
    MultibandKernel *K = NULL;
    int n, nbands, i, j;

    fp = fopen(filename, "r");
    if (fp != NULL) {
    	fscanf(fp, "%d %d", &n, &nbands);
    	A = CreateAdjRel(n);
    	K = CreateMultibandKernel(A, nbands);

    	for (i=0; i < n; i++) {
    	    fscanf(fp, "%d %d", &K->A->adj[i].dx, &K->A->adj[i].dy);
    	    for (j=0; j < nbands; j++) {
                fscanf(fp, "%f", &K->w[i][j]);
    	    }
    	}

    	DestroyAdjRel(&A);
    	fclose(fp);
    }
    else {
        Error(MSG2, "readMultibandKernel");
    }
    return K;
}

void writeMultibandKernel(MultibandKernel *K, char *filename)
{
    FILE *fp = NULL;
    int i , j;


    fp = fopen(filename, "w");
    if ( fp != NULL ) {
        fprintf(fp, "%d %d\n", K->A->n, K->nbands);
        for ( i=0; i < K->A->n ; i++ ) {
            fprintf(fp, "%d %d ", K->A->adj[i].dx, K->A->adj[i].dy);
            for ( j=0; j < (K->nbands - 1) ; j++ ) {
                fprintf(fp, "%.12f ", K->w[i][j]);
            }
            fprintf(fp, "%.12f\n", K->w[i][j]);
        }
        fclose(fp);
    } else {
        Error(MSG2, "writeMultibandKernel");
    }
}

MultibandKernel **generateKernelBank(int nx, int ny, int nbands, int n)
{
    int i;

    MultibandKernel **kernelBank = (MultibandKernel **)calloc(n,sizeof(MultibandKernel *));

    for(i=0; i<n; i++){
        kernelBank[i] = CreateMultibandRndKernel(nx,ny,nbands);
    }

    return kernelBank;
}
