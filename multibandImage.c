#include "project.h"

/*Agregado por Pablo Fonseca */

MultibandImage *CreateMultibandImage(int nx, int ny, int nbands)
{
    MultibandImage *I=(MultibandImage *)calloc(1,sizeof(MultibandImage));
    int y,x;

    I->nx = nx;
    I->ny = ny;
    I->dx = 1.0;
    I->dy = 1.0;
    I->nbands = nbands;
    sprintf(I->unid,"mm");

    I->band = (Bands **)calloc(ny,sizeof(Bands *));
    if (I->band == NULL) {
        Error(MSG1,"CreateMultibandImage");
    }
    else {
        for (y=0; y < I->ny; y++) {
            I->band[y] = (Bands *)calloc(nx,sizeof(Bands));

            if (I->band[y] == NULL)
                Error(MSG1,"CreateMultibandImage");

            for (x=0;x< I->nx; x++){
                I->band[y][x].val = (int *)calloc(nbands,sizeof(int));

                if (I->band[y][x].val == NULL)
                    Error(MSG1,"CreateMultibandImage");
            }
        }
    }
    return(I);
}

MultibandImage *ReadMultibandImage(char *filename)
{
    FILE *fp=NULL;
    char type[10];
    int  x,y,nx,ny,Imax, nbands,i;
    MultibandImage *I=NULL;
    char remarks[256];

    fp = fopen(filename,"r");
    if (fp != NULL) {
        fscanf(fp,"%s",type);
        NCFgets(remarks,255,fp);

        if (strcmp(type,"P6")==0) {
            fscanf(fp,"%d",&nx);
            fscanf(fp,"%d",&ny);
            fscanf(fp,"%d",&nbands);
            I = CreateMultibandImage(nx,ny,nbands);
            fscanf(fp,"%d",&Imax);
            fgetc(fp);
            for (y=0; y < ny; y++)
                for (x=0; x < nx; x++)
                    for(i=0; i < nbands; i++){
                        I->band[y][x].val[i]=(int)fgetc(fp);
                    }
        }
        else {
            Error("Image type invalid","ReadColormage");
        }

        fclose(fp);
    }
    else{
        Error(MSG2,"ReadColorImage");
    }

    return(I);
}

void WriteMultibandImage(MultibandImage *I, char *filename)
{
    FILE *fp=NULL;
    int  x,y, i;

    fp = fopen(filename,"w");
    if (fp != NULL) {
        fprintf(fp,"P6\n");
        fprintf(fp,"%d %d %d\n",I->nx,I->ny, I->nbands);
        fprintf(fp,"255\n");

        for (y=0; y < I->ny; y++){
            for (x=0; x < I->nx; x++){
                for(i=0; i< I->nbands; i++){
                    fputc((uchar)I->band[y][x].val[i],fp);
                }
            }
        }
        fclose(fp);
    }
    else{
        Error(MSG2,"WriteMultibandImage");
    }

}

void DestroyMultibandImage(MultibandImage **I)
{
    int y, x;

    if ((*I) != NULL) {
        for (y=0; y < (*I)->ny; y++) {
            for (x=0; x < (*I)->nx; x++) {
                free((*I)->band[y][x].val);
            }
            free((*I)->band[y]);
        }

        free((*I)->band);
        free(*I);
        *I = NULL;
    }
}

/*Fin Agregado por Pablo Fonseca */
