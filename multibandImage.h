#ifndef _MULTIBANDIMAGE_H_
#define _MULTIBANDIMAGE_H_


/*Agregado por Pablo Fonseca */

typedef struct _bands {
  float *val;     /* Vector of Bands */
} Bands;

typedef struct _multibandImage {
  int   nx,ny;      /* dimensoes da imagem */
  int   nbands;     /* numero de bandas*/
  Bands **band;      /* matriz com as basdas dos pixels */
  float dx,dy;      /* tamanho do pixel em unidades de comprimento */
  char  unid[10];   /* unidade de comprimento */
} MultibandImage;

MultibandImage   *CreateMultibandImage(int nx, int ny, int nbands);
void              DestroyMultibandImage(MultibandImage **I);
MultibandImage   *ReadMultibandImage(char *filename);
void              WriteMultibandImage(MultibandImage *I, char *filename);
/*Fin de Agregado por Pablo Fonseca */

MultibandImage   *ReadGrayImageIntoMultibandImage(char *filename);
MultibandImage   *AppendMultibandImageHowBand(MultibandImage *imgSource, MultibandImage *imgTarget, int band);

MultibandImage   *AppendManyMultibandImages(MultibandImage **images, int n);
MultibandImage   *AppendMultibandImages(MultibandImage *img1, MultibandImage *img2);
void  Write2CSV(MultibandImage **images, int n, char * filename);
#endif // _MULTIBANDIMAGE_H_
