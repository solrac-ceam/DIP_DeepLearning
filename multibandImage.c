#include "multibandImage.h"

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
                I->band[y][x].val = (float *)calloc(nbands,sizeof(float));

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
    int  x,y,nx,ny, nbands,i;
    MultibandImage *I=NULL;
    char remarks[256];

    fp = fopen(filename,"rb");
    if (fp != NULL) {
        fscanf(fp,"%s",type);
        NCFgets(remarks,255,fp);

        if (strcmp(type,"MBImg")==0) {
            fscanf(fp,"%d",&nx);
            fscanf(fp,"%d",&ny);
            fscanf(fp,"%d",&nbands);
            I = CreateMultibandImage(nx,ny,nbands);
            fgetc(fp);
            for (y=0; y < ny; y++)
                for (x=0; x < nx; x++)
                    for(i=0; i < nbands; i++){
                        fread(&I->band[y][x].val[i], sizeof(float), 1, fp);
                    }
        }
        else {
            Error("Image type invalid","ReadMultibandImage");
        }

        fclose(fp);
    }
    else{
        Error(MSG2,"ReadMultibandImage");
    }

    return(I);
}

void WriteMultibandImage(MultibandImage *I, char *filename)
{
    FILE *fp=NULL;
    int  x,y, i;

    fp = fopen(filename,"wb");
    if (fp != NULL) {
        fprintf(fp,"MBImg\n");
        fprintf(fp,"%d %d %d\n", I->nx, I->ny, I->nbands);

        for (y=0; y < I->ny; y++){
            for (x=0; x < I->nx; x++){
                for(i=0; i< I->nbands; i++){
                    fwrite(&I->band[y][x].val[i], sizeof(float), 1, fp);
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

MultibandImage *ReadGrayImageIntoMultibandImage(char *filename)
{
    FILE *fp=NULL;
    char type[10];
    uchar *data=NULL;
    int  x,y,nx,ny,Imax;
    MultibandImage *I=NULL;
    char remarks[256];

    fp = fopen(filename,"r");
    if (fp != NULL) {
        fscanf(fp,"%s",type);
        NCFgets(remarks,255,fp);

        if ((strcmp(type,"P2")==0)|| (strcmp(type,"P5")==0)) {
            fscanf(fp,"%d",&nx);
            fscanf(fp,"%d",&ny);

            I = CreateMultibandImage(nx,ny,1);
            fscanf(fp,"%d",&Imax);

            if (strcmp(type,"P2")==0) {
                for (y=0; y < ny; y++)
                    for (x=0; x < nx; x++)
                        fscanf(fp,"%f",&I->band[y][x].val[0]);
            }
            else {
                if (strcmp(type,"P5")==0) {
                    data = AllocUCharArray(nx*ny);
                    fread(data,sizeof(uchar),nx*ny,fp);
                    for (y=0; y < ny; y++)
                        for (x=0; x < nx; x++)
                            I->band[y][x].val[0] = data[x+y*nx];
                    free(data);
                }
            }
        }
        else {
            Error("Image type invalid","ReadGrayImageIntoMultibandImage");
        }
        fclose(fp);
    }
    else {
        Error(MSG2,"ReadGrayImageIntoMultibandImage");
    }

    return(I);
}

MultibandImage   *AppendMultibandImageHowBand(MultibandImage *imgSource, MultibandImage *imgTarget, int band)
{
    //falta implementar XD
    return imgTarget;
}

MultibandImage   *AppendMultibandImages(MultibandImage *img1, MultibandImage *img2)
{
    int band, y, x;
    int offset;
    MultibandImage *result = CreateMultibandImage(img1->nx, img1->ny, img1->nbands + img2->nbands);
    for(band=0; band<img1->nbands; band++)
      for (y=0; y < img1->ny; y++)
        for (x=0; x< img1->nx; x++){
            result->band[y][x].val[band] = img1->band[y][x].val[band];
        }
    offset = img1->nbands;
    for(band=0; band<img2->nbands; band++)
      for (y=0; y < img2->ny; y++)
        for (x=0; x< img2->nx; x++){
            result->band[y][x].val[offset+band] = img2->band[y][x].val[band];
        }
    return result;
}

MultibandImage   *AppendManyMultibandImages(MultibandImage **images, int n)
{
    int i,band, tbands=0, y, x;
    int offset = 0;
    MultibandImage *result;

    for(i=0; i<n; i++)
        tbands += images[i]->nbands;

    result = CreateMultibandImage(images[0]->nx, images[0]->ny, tbands);

    for(i=0; i<n; i++){
        for(band=0; band<images[i]->nbands; band++){
          for (y=0; y < images[i]->ny; y++){
            for (x=0; x< images[i]->nx; x++){
                result->band[y][x].val[offset+band] = images[i]->band[y][x].val[band];
            }
          }
        }
        offset += images[i]->nbands;
    }

    return result;
}

void  Write2CSV(MultibandImage **images, int n, char * filename, char* tag)
{
    int i,band, y, x, count=1;
    FILE *fp=NULL;
    fp = fopen(filename,"a");
    printf("%s\n", tag);
    tag[4] = '\0';
    printf("%d\n", atoi(tag));
    fprintf(fp,"%d", atoi(tag));
    for(i=0; i<n; i++){
      for(band=0; band<images[i]->nbands; band++){
        for (y=0; y < images[i]->ny; y++){
           for (x=0; x< images[i]->nx; x++){
               if(images[i]->band[y][x].val[band]!=0.0){
                    fprintf(fp," %d:%f", count, images[i]->band[y][x].val[band]);
               }
               count++;
            }
          }
        }
    }

    fprintf(fp,"\n");
    fclose(fp);

}

void WriteOneBandOfMultibandImageToGrayImage(MultibandImage *I, int z, char *filename)
{
    FILE *fp=NULL;
    uchar *data=NULL;
    int  x,y;
    float Imax,Imin;
    char *image_band_filename;

    if(z >= I->nbands ) return;

    Imax = FLT_MIN; Imin = FLT_MAX;
    for (y=0; y < I->ny; y++)
        for (x=0; x < I->nx; x++){
            if (I->band[y][x].val[z] > Imax)
                Imax = I->band[y][x].val[z];
            if (I->band[y][x].val[z] < Imin)
                Imin = I->band[y][x].val[z];
        }

    image_band_filename = AllocCharArray(strlen(filename)+14);
    sprintf(image_band_filename, "%s_band_%.3d.pgm", filename, z);

    fp = fopen(image_band_filename,"wb");
    if (fp != NULL) {
        if (((Imax-Imin) >= 0)&&((Imax-Imin) < 256)){
            fprintf(fp,"P5\n");
            fprintf(fp,"%d %d\n",I->nx,I->ny);
            fprintf(fp,"%d\n",(int)Imax);

            data = AllocUCharArray(I->nx*I->ny);
            for (y=0; y < I->ny; y++)
                for (x=0; x < I->nx; x++)
                    data[x+y*I->nx] = (uchar) (I->band[y][x].val[z]-Imin + 0.5);

            fwrite(data,sizeof(uchar),I->nx*I->ny,fp);
            free(data);
        }
        else {
            fprintf(fp,"P2\n");
            fprintf(fp,"%d %d\n",I->nx,I->ny);
            fprintf(fp,"%d\n", (int)(Imax-Imin + 0.5));

            for (y=0; y < I->ny; y++){
                for (x=0; x < I->nx; x++)
                    fprintf(fp,"%d ", (int)(I->band[y][x].val[z]-Imin + 0.5));
                if (y%17==0)
                    fprintf(fp,"\n");
            }
        }
        fclose(fp);
    }
    else {
        Error(MSG2,"WriteOneBandOfMultibandImageToGrayImage");
    }
}

void WriteMultibandImageToGrayImages(MultibandImage *mbImg, char *filename)
{
    int i;
    struct stat st;

    if (stat(filename, &st) == -1) {
        if (mkdir(filename, 0777) == -1){
            Error("cannot create directory.", "WriteMultibandImageToGrayImages");
        }
    }


    for (i=0; i < mbImg->nbands; i++) {
        WriteOneBandOfMultibandImageToGrayImage(mbImg, i, filename);
    }
}
